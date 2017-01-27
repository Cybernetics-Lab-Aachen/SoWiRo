// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OptitrackTrackerComponent.generated.h"

UCLASS(ClassGroup = "OptiTrack", meta = (BlueprintSpawnableComponent))
class OPTITRACKNATNET_API UOptitrackTrackerComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
		/** ID of the rigid body used to drive this actor's position/rotation. */
		UPROPERTY(EditAnywhere, Category = Optitrack)
		int32 TrackingId;

	/**
	* If your scene contains multiple client origin objects, you can specify
	* which one to use. If unset, this defaults to the first client origin
	* that's found in the world.
	*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Optitrack)
	class AOptitrackClientOrigin* TrackingOrigin;

	UPROPERTY(BlueprintReadOnly, Category = Optitrack)
	FVector position;

	UPROPERTY(BlueprintReadOnly, Category = Optitrack)
	FRotator orientation;

protected:
	// UActorComponent overrides
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};