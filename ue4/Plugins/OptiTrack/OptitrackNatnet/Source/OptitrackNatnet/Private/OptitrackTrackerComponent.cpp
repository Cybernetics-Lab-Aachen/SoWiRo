//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#include "OptitrackNatnetPrivatePCH.h"
#include "OptitrackTrackerComponent.h"

#include "MessageLog.h"
#include "UObjectToken.h"

#include "OptitrackClientOrigin.h"


#define LOCTEXT_NAMESPACE "OptitrackNatnet"


UOptitrackTrackerComponent::UOptitrackTrackerComponent( const FObjectInitializer& ObjectInitializer )
: Super( ObjectInitializer )
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = TG_PostPhysics;

	bTickInEditor = false;
	bAutoActivate = true;
	bWantsBeginPlay = true;
}


void UOptitrackTrackerComponent::BeginPlay()
{
	Super::BeginPlay();

	if ( TrackingOrigin == nullptr )
	{
		TrackingOrigin = AOptitrackClientOrigin::FindDefaultClientOrigin( GetWorld() );
	}

	if ( Cast<AOptitrackClientOrigin>( GetOwner() ) )
	{
		FMessageLog( "PIE" ).Error()
			->AddToken( FUObjectToken::Create( GetOwner() ) )
			->AddToken( FTextToken::Create( LOCTEXT( "PIE_RigidBodyComponent_OriginOwner", "OptiTrack Rigid Body component should not be attached to an OptiTrack Client Origin actor; disabling component." ) ) );

		TrackingOrigin = nullptr;
	}
}


void UOptitrackTrackerComponent::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	if ( TrackingOrigin == nullptr || GetOwner() == nullptr || GetOwner()->GetRootComponent() == nullptr )
	{
		return;
	}

	FOptitrackRigidBodyState rbState;
	if ( TrackingOrigin->GetLatestRigidBodyState( TrackingId, rbState ) )
	{
		Position = rbState.Position;
		Orientation = rbState.Orientation.Rotator();
	}
}
