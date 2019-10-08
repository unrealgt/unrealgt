// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include <Queue.h>

class FSocket;

/**
 *
 */
class UNREALGT_API FGTThreadedTCPStreamer : public FRunnable {
public:
  FGTThreadedTCPStreamer(FString IPAddress, uint32 Port);

  ~FGTThreadedTCPStreamer();

  void SendMessage(const TArray<uint8> &MessagePayload);

  bool Init() override;
  uint32 Run() override;
  void Stop() override;
  void Exit() override;

private:
  FSocket *Socket;

  FString IPAddress;
  uint32 Port;

  bool bIsSocketConnected;

  bool bShouldStop;

  TQueue<TArray<uint8>, EQueueMode::Mpsc> MessageRequests;

  FRunnableThread *Thread;

  bool TryConnect();
};
