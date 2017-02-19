//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#pragma once

#include <NatNetTypes.h>

#include "OptitrackClientOrigin.generated.h"


/**
* Represents a pose for a tracked rigid body.
*/
USTRUCT()
struct FOptitrackRigidBodyState
{
	GENERATED_USTRUCT_BODY()

	/**
	 * Populated with the value of FPlatformTime::Seconds() on frame delivery.
	 * Can be compared to determine the freshness of the associated pose data.
	 */
	UPROPERTY()
	double DeliveryTimePlatformSeconds;

	/** Position in tracking space. */
	UPROPERTY()
	FVector Position;

	/** Orientation in tracking space. */
	UPROPERTY()
	FQuat Orientation;
};


UENUM()
enum class EOptitrackClientConnectionType : uint8
{
	Multicast = 0,
	Unicast,
};


/**
*
*/
UCLASS()
class OPTITRACKNATNET_API AOptitrackClientOrigin : public AActor
{
	GENERATED_UCLASS_BODY()

public:
	/** The network address of the NatNet server to connect to. */
	UPROPERTY( EditAnywhere, Category=Optitrack )
	FString ServerAddress;

	/** The address of the local network interface to use. */
	UPROPERTY( EditAnywhere, Category=Optitrack )
	FString ClientAddress;

	/** The NatNet client type to instantiate. Must match the setting of the NatNet server. */
	UPROPERTY( EditAnywhere, Category=Optitrack )
	EOptitrackClientConnectionType ConnectionType;

	/** If non-zero, the rigid body with the specified ID is used to provide input to compatible HMD implementations. */
	UPROPERTY( EditAnywhere, Category=Optitrack )
	int32 HmdRigidBodyId;

	/**
	* Retrieves latest available FOptitrackRigidBodyState for the rigid body
	* identified by the @RbId parameter. Note that this data can be stale if
	* the rigid body in question hasn't streamed any new data recently.
	* The offset/rotation/scale of this origin actor are applied to the pose.
	*
	* @param OutRbState Receives latest available rigid body state (if any).
	* @return True if any rigid body state was available for the specified ID,
	*         otherwise false.
	*/
	bool GetLatestRigidBodyState( int32 RbId, FOptitrackRigidBodyState& OutRbState );

	/**
	* Retrieves latest available FOptitrackRigidBodyState for the rigid body
	* identified by the @RbId parameter. Note that this data can be stale if
	* the rigid body in question hasn't streamed any new data recently.
	*
	* This "Untransformed" version only applies the global WorldToMeters scale,
	* and does not apply the offset/rotation/scale of this origin actor.
	*
	* @param OutRbState Receives latest available rigid body state (if any).
	* @return True if any rigid body state was available for the specified ID,
	*         otherwise false.
	*/
	bool GetLatestRigidBodyStateUntransformed( int32 RbId, FOptitrackRigidBodyState& OutRbState );

	/**
	* Helper for the common case where only a single client origin is present.
	* Used to provide a default for any UOptitrackRigidBody components which
	* don't otherwise specify a particular origin to use.
	* Note: This is expensive and the result should be cached.
	* @return The first AOptitrackClientOrigin actor found in @World.
	*/
	static AOptitrackClientOrigin* FindDefaultClientOrigin( UWorld* World );

	/**
	* Finds the first AOptitrackClientOrigin specifying an HMD rigid body ID.
	* Note: This is expensive and the result should be cached.
	* @return The first suitable AOptitrackClientOrigin actor found in @World.
	*/
	static AOptitrackClientOrigin* FindHmdClientOrigin( UWorld* World );

protected:
	// AActor overrides
	virtual void PreInitializeComponents() override;
	virtual void EndPlay( const EEndPlayReason::Type EndPlayReason ) override;

private:
	void InitializeClient();
	void ShutdownClient();

	static void NATNET_CALLCONV NatNetDataCallback( sFrameOfMocapData* NewFrame, void* UserData );

	class NatNetClient* Client;

	/** Controls access to @LatestRigidBodyStates map. */
	FCriticalSection FrameUpdateLock;

	/** Copied from AWorldSettings::WorldToMeters for use in the NatNet callback (which happens on another thread). */
	float CachedWorldToMeters;

	/** Key is rigid body ID, value is latest pose available for that rigid body. */
	UPROPERTY()
	TMap< int32, FOptitrackRigidBodyState > LatestRigidBodyStates;
};
