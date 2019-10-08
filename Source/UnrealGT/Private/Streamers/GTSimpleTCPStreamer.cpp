// Fill out your copyright notice in the Description page of Project Settings.

#include "GTSimpleTCPStreamer.h"

#include <SocketSubsystem.h>
#include <Sockets.h>
#include <IPAddress.h>
#include <Engine/World.h>
#include "GTThreadedTCPStreamer.h"

UGTSimpleTCPStreamer::UGTSimpleTCPStreamer()
    : IPAddress("127.0.0.1")
    , Port(8999)
{

}

void UGTSimpleTCPStreamer::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    Super::OnComponentDestroyed(bDestroyingHierarchy);

    if (TCPStreamer)
    {
        // this will call stop and wait for completion
        delete TCPStreamer;
    }
}

void UGTSimpleTCPStreamer::BeginPlay()
{
    Super::BeginPlay();

    TCPStreamer = new FGTThreadedTCPStreamer(IPAddress, Port);
}

void UGTSimpleTCPStreamer::OnDataReady(const TArray<uint8>& Data, const FDateTime& TimeStamp)
{
    TCPStreamer->SendMessage(Data);
}

