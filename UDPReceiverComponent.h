
#pragma once

#include "CoreMinimal.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "GameFramework/Actor.h"
#include "UDPReceiverComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CESIUMFORUNREALSAMPLES_API AUDPReceiverComponent : public AActor
{
	GENERATED_BODY()
	
public:	
	AUDPReceiverComponent();

	void ReceiveData(const FArrayReaderPtr& ArrayReader, const FIPv4Endpoint& EndPt);

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	APawn* TargetCesiumPawn;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	struct TUnrealRxData
	{
		uint64_t Heartbeat;
		double   OwnshipLatitude;
		double   OwnshipLongitude;
		float    OwnshipAltitude;
		float    OwnshipPitch;
		float    OwnshipRoll;
		float    OwnshipYaw;
	};

	FSocket* ListenSocket;
	TSharedPtr<FUdpSocketReceiver> UDPReceiver;
	TUnrealRxData mData;
	uint64_t      mPrevHeartbeat;
	int           mTimeout;

};
