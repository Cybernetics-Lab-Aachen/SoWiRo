// Fill out your copyright notice in the Description page of Project Settings.

#include "OptitrackNatnetPrivatePCH.h"
#include "OptitrackTrackerComponent.h"

#include "OptitrackClientOrigin.h"

UOptitrackTrackerComponent::UOptitrackTrackerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bTickInEditor = true;
	bAutoActivate = true;
	bWantsBeginPlay = true;
}


void UOptitrackTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (TrackingOrigin == nullptr)
	{
		TrackingOrigin = AOptitrackClientOrigin::FindDefaultClientOrigin(GetWorld());
	}
}


void UOptitrackTrackerComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TrackingOrigin == nullptr || GetOwner() == nullptr || GetOwner()->GetRootComponent() == nullptr)
	{
		return;
	}

	FOptitrackRigidBodyState rbState;
	if (TrackingOrigin->GetLatestRigidBodyState(TrackingId, rbState))
	{
		position = rbState.Position;
		orientation = rbState.Orientation.Rotator();
	}
}

