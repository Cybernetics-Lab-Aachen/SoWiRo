//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#pragma once

#include "OptitrackHmdPawn.generated.h"


/**
* Minimal example Pawn class whose position is driven by a tracked rigid body.
* If not explicitly specified, it attempts to determine the appropriate client
* origin object and rigid body ID to use automatically.
*/
UCLASS()
class OPTITRACKNATNET_API AOptitrackHmdPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:

protected:
	// AActor overrides
	virtual void PreInitializeComponents() override;

private:
	UPROPERTY( VisibleAnywhere, Category=Optitrack )
	class UOptitrackRigidBodyComponent* RigidBodyTrackingComponent;
};
