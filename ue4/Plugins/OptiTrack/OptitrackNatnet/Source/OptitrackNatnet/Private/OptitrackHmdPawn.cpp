//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#include "OptitrackNatNetPrivatePCH.h"
#include "OptitrackHmdPawn.h"

#include "OptitrackClientOrigin.h"
#include "OptitrackRigidBody.h"


AOptitrackHmdPawn::AOptitrackHmdPawn( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
{
	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>( this, TEXT( "RootSceneComponent" ) );

#if WITH_EDITORONLY_DATA
	RootComponent->bVisualizeComponent = true;
#endif

	RigidBodyTrackingComponent = ObjectInitializer.CreateDefaultSubobject<UOptitrackRigidBodyComponent>( this, TEXT( "RigidBodyTrackingComponent" ) );

	BaseEyeHeight = 0.0f;
}

void AOptitrackHmdPawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	if ( RigidBodyTrackingComponent )
	{
		// If the client origin hasn't been set explicitly, attempt to find an appropriate default.
		if ( RigidBodyTrackingComponent->TrackingOrigin == nullptr )
		{
			RigidBodyTrackingComponent->TrackingOrigin = AOptitrackClientOrigin::FindHmdClientOrigin( GetWorld() );
		}

		// If the rigid body tracking ID hasn't been set explicitly, attempt to set it from the client origin's HMD RB ID.
		if ( RigidBodyTrackingComponent->TrackingOrigin && RigidBodyTrackingComponent->TrackingId == 0 )
		{
			RigidBodyTrackingComponent->TrackingId = RigidBodyTrackingComponent->TrackingOrigin->HmdRigidBodyId;
		}
	}
}
