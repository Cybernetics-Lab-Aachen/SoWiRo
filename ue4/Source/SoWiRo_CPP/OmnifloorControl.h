// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma comment (lib, "Ws2_32.lib")

#include "AllowWindowsPlatformTypes.h"
#include "Components/ActorComponent.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include "OmnifloorControl.generated.h"


UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOWIRO_CPP_API UOmnifloorControl : public UActorComponent
{
	GENERATED_BODY()

	SOCKET udpSocket;
	sockaddr_storage udpDestination;

	float currentSpeed = 0.0;

	void sendSpeed(float speed);

	int resolveHelper(const char* hostname, int family, int port, sockaddr_storage* pAddr);

public:
	// Sets default values for this component's properties
	UOmnifloorControl();
	~UOmnifloorControl();

	UPROPERTY(BlueprintReadWrite)
		float Speed = 0.0;

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
		FString Host = "137.226.188.32";

	UPROPERTY(EditAnywhere)
		float MaxSpeed = 1.0;

	UPROPERTY(EditAnywhere)
		int32 Port = 11000;

};
