
#include "UDPReceiverComponent.h"
#include "EngineUtils.h"
#include "CesiumGeoreference.h"

AUDPReceiverComponent::AUDPReceiverComponent()
   : mData{},
     mPrevHeartbeat(0),
     mTimeout(0)
{
   PrimaryActorTick.bCanEverTick = true;
   PrimaryActorTick.bStartWithTickEnabled = true;
}

void AUDPReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

   FIPv4Address Addr(0, 0, 0, 0);
   int32 Port = 4321;

   FIPv4Endpoint Endpoint(Addr, Port);
   ListenSocket = FUdpSocketBuilder(TEXT("UDPReceiverSocket"))
      .AsNonBlocking()
      .AsReusable()
      .BoundToEndpoint(Endpoint)
      .WithReceiveBufferSize(2 * 1024 * 1024);

   if (ListenSocket)
   {
      UDPReceiver = MakeShareable(new FUdpSocketReceiver(ListenSocket, FTimespan::FromMilliseconds(100), TEXT("UDPReceiver")));
      UDPReceiver->OnDataReceived().BindUObject(this, &AUDPReceiverComponent::ReceiveData);
      UDPReceiver->Start();
   }

   UE_LOG(LogTemp, Warning, TEXT("Begin Play"));
}

void AUDPReceiverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

   if (UDPReceiver.IsValid())
   {
      UDPReceiver->Stop();
      UDPReceiver.Reset();
   }

   if (ListenSocket)
   {
      ListenSocket->Close();
      ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
   }

   UE_LOG(LogTemp, Warning, TEXT("End Play"));
}

void AUDPReceiverComponent::ReceiveData(const FArrayReaderPtr& ArrayReader, const FIPv4Endpoint& EndPt)
{
   if (ArrayReader->TotalSize() != sizeof(TUnrealRxData))
      return;

   ArrayReader->Serialize(&mData, sizeof(TUnrealRxData));
}

void AUDPReceiverComponent::Tick(float DeltaTime)
{
   if (!TargetCesiumPawn)
   {
      UE_LOG(LogTemp, Warning, TEXT("TargetCesiumPawn is null"));
      return;
   }

   APlayerController* PC = GetWorld()->GetFirstPlayerController();
   if (!PC)
   {
      UE_LOG(LogTemp, Warning, TEXT("PC is null"));
      return;
   }

   if (mData.Heartbeat)
   {
      ACesiumGeoreference* georef = ACesiumGeoreference::GetDefaultGeoreference(TargetCesiumPawn);

      FVector lat_lon_alt = FVector(mData.OwnshipLongitude, mData.OwnshipLatitude, mData.OwnshipAltitude * 0.3048);
      FVector unreal_pos = georef->TransformLongitudeLatitudeHeightPositionToUnreal(lat_lon_alt);
      FRotator orientation(mData.OwnshipPitch, mData.OwnshipYaw - 90.0, mData.OwnshipRoll);

      // Move the pawn
      TargetCesiumPawn->DisableInput(PC);
      PC->UnPossess();
      PC->SetViewTarget(TargetCesiumPawn);
      TargetCesiumPawn->SetActorLocationAndRotation(unreal_pos, orientation);

      if (mData.Heartbeat != mPrevHeartbeat)
      {
         mTimeout = 0;
      }
      else
      {
         mTimeout++;
         if (mTimeout > 60)
            mData.Heartbeat = 0;
      }
      mPrevHeartbeat = mData.Heartbeat;
   }
   else
   {
      PC->Possess(TargetCesiumPawn);
      TargetCesiumPawn->EnableInput(PC);
      Super::Tick(DeltaTime);
   }
}

