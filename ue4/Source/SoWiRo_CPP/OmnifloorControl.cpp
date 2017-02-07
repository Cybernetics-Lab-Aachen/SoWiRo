// Fill out your copyright notice in the Description page of Project Settings.

#include "SoWiRo_CPP.h"
#include "OmnifloorControl.h"
#include <algorithm>


// Sets default values for this component's properties
UOmnifloorControl::UOmnifloorControl()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


UOmnifloorControl::~UOmnifloorControl() {
	Speed = 0.0;
	sendSpeed(Speed);
}


void UOmnifloorControl::sendSpeed(float speed) {
	currentSpeed = speed;
	//UE_LOG(LogTemp, Warning, TEXT("Sending... %f"), speed);
	sendto(udpSocket, (const char*)&speed, 4, 0, (sockaddr*)&udpDestination, sizeof(udpDestination));
}


int UOmnifloorControl::resolveHelper(const char* hostname, int family, int port, sockaddr_storage* pAddr) {
	char service[5] = {'1', '1', '0', '0', '0'};
	//snprintf(service, 5, "%d", port);
	int result;
	addrinfo* result_list = NULL;
	addrinfo hints = {};
	hints.ai_family = family;
	hints.ai_socktype = SOCK_DGRAM;
	result = getaddrinfo(hostname, service, &hints, &result_list);
	if (result == 0) {
		memcpy(pAddr, result_list->ai_addr, result_list->ai_addrlen);
		freeaddrinfo(result_list);
	}
	return result;
}


// Called when the game starts
void UOmnifloorControl::BeginPlay()
{
	Super::BeginPlay();
	Speed = 0.0;
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in addrListen = {};
	addrListen.sin_family = AF_INET;
	bind(udpSocket, (sockaddr*)&addrListen, sizeof(addrListen));
	resolveHelper(TCHAR_TO_ANSI(*Host), AF_INET, Port, &udpDestination);
}


// Called every frame
void UOmnifloorControl::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	float speed = std::max(std::min(Speed, MaxSpeed), 0.0f);
	// Calculate acceleration

	// Check Active
	//if (!IsActive())
//		speed = 0.0;
	sendSpeed(speed);
}
