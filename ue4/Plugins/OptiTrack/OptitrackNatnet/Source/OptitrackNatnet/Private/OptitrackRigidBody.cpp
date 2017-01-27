//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#include "OptitrackNatnetPrivatePCH.h"
#include "OptitrackRigidBody.h"

#include "OptitrackClientOrigin.h"


UOptitrackRigidBodyComponent::UOptitrackRigidBodyComponent( const FObjectInitializer& ObjectInitializer )
: Super( ObjectInitializer )
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.TickGroup = TG_PostPhysics;

    bTickInEditor = true;
    bAutoActivate = true;
    bWantsBeginPlay = true;
}


void UOptitrackRigidBodyComponent::BeginPlay()
{
    Super::BeginPlay();

    if ( TrackingOrigin == nullptr )
    {
        TrackingOrigin = AOptitrackClientOrigin::FindDefaultClientOrigin( GetWorld() );
    }
}


void UOptitrackRigidBodyComponent::TickComponent( float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

    if ( TrackingOrigin == nullptr || GetOwner() == nullptr || GetOwner()->GetRootComponent() == nullptr )
    {
        return;
    }

    FOptitrackRigidBodyState rbState;
    if ( TrackingOrigin->GetLatestRigidBodyState( TrackingId, rbState ) )
    {
        GetOwner()->GetRootComponent()->SetWorldLocation( rbState.Position );
        GetOwner()->GetRootComponent()->SetWorldRotation( rbState.Orientation );
    }
}
