// Fill out your copyright notice in the Description page of Project Settings.

#include "GTThreadedTCPStreamer.h"

#include <IPAddress.h>
#include <SocketSubsystem.h>
#include <Sockets.h>
#include <RunnableThread.h>

FGTThreadedTCPStreamer::FGTThreadedTCPStreamer(FString IPAddress, uint32 Port)
    : Socket(nullptr)
    , IPAddress(IPAddress)
    , Port(Port)
    , bIsSocketConnected(false)
    , bShouldStop(false)
{
    Thread = FRunnableThread::Create(this, TEXT("TCPStreamerThread"));
}

 FGTThreadedTCPStreamer::~FGTThreadedTCPStreamer()
{
     if (Thread)
     {
         Thread->Kill(true);
         delete Thread;
         Thread = nullptr;
     }
 }

void FGTThreadedTCPStreamer::SendMessage(const TArray<uint8>& MessagePayload)
{
    // Dont accept requests if we dont have a connection
    if (!bIsSocketConnected)
    {
        return;
    }
    MessageRequests.Enqueue(MessagePayload);
}

bool FGTThreadedTCPStreamer::Init()
{
    TryConnect();
    return true;
}

uint32 FGTThreadedTCPStreamer::Run()
{
    while (!bShouldStop)
    {
        // Important that tryconnect is called first before checking the queue
        if (TryConnect() && !MessageRequests.IsEmpty())
        {
            TArray<uint8> Data;
            MessageRequests.Dequeue(Data);
            int32 ByteCount = Data.Num();
            int32 TotalMessageLength = ByteCount + sizeof(ByteCount);
            // Little to Big Endian
            int32 ByteCountBigEndian = (ByteCount & 0x000000FFU) << 24 | (ByteCount & 0x0000FF00U) << 8 | (ByteCount & 0x00FF0000U) >> 8 |
                                       (ByteCount & 0xFF000000U) >> 24;

            uint8* Packet = (uint8*)FMemory::Malloc(TotalMessageLength);
            FMemory::Memcpy(Packet, &ByteCountBigEndian, sizeof(int32));
            FMemory::Memcpy(Packet + sizeof(int32), Data.GetData(), Data.Num());
            int32 BytesSend = 0;
            int32 TotalBytesSend = 0;
            while (TotalBytesSend != TotalMessageLength)
            {
                Socket->Send(Packet + TotalBytesSend, TotalMessageLength - TotalBytesSend, BytesSend);
                if (BytesSend == -1)
                {
                    bIsSocketConnected = false;
                    Socket->Close();
                    ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
                    break;
                }
                TotalBytesSend += BytesSend;
            }
            FMemory::Free(Packet);
        }
        // If we still dont have a connection sleep a bit longer
        if (!bIsSocketConnected)
        {
            FPlatformProcess::SleepNoStats(0.5f);
        }
        else
        {
            FPlatformProcess::SleepNoStats(1 / 120.f);
        }
    }

    return 0;
}

void FGTThreadedTCPStreamer::Stop()
{
    bShouldStop = true;
}

void FGTThreadedTCPStreamer::Exit()
{
    if (Socket)
    {
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
    }
}

bool FGTThreadedTCPStreamer::TryConnect()
{
    if (!bIsSocketConnected || !Socket)
    {
        Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);
        TSharedRef<FInternetAddr> InternetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
        bool bIsIPValid;
        InternetAddr->SetIp(*IPAddress, bIsIPValid);
        InternetAddr->SetPort(Port);
        bIsSocketConnected = Socket->Connect(*InternetAddr);
        return bIsSocketConnected;
    }
    return true;
}
