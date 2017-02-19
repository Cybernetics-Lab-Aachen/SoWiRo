//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#pragma once

#include "OptitrackTrackerComponent.generated.h"


/**
* A simple component that drives the position and rotation of its actor's
* root component according to the pose of a specified tracked rigid body.
*/
UCLASS( ClassGroup="OptiTrack", meta=(BlueprintSpawnableComponent) )
class UOptitrackTrackerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	/** ID of the rigid body used to drive this actor's position/rotation. */
	UPROPERTY( EditAnywhere, Category=Optitrack )
	int32 TrackingId;

	/**
	* If your scene contains multiple client origin objects, you can specify
	* which one to use. If unset, this defaults to the first client origin
	* that's found in the world.
	*/
	UPROPERTY( EditAnywhere, AdvancedDisplay, Category=Optitrack )
	class AOptitrackClientOrigin* TrackingOrigin;

	/**
	* Stores the trackers position relative to the origin.
	*/
	UPROPERTY(BlueprintReadOnly, Category = Optitrack)
	FVector Position;

	/**
	* Stores the trackers orientation relative to the origin.
	*/
	UPROPERTY(BlueprintReadOnly, Category = Optitrack)
	FRotator Orientation;

protected:
	// UActorComponent overrides
	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
};
