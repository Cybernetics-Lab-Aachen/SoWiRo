//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#include "OptitrackNatNetPrivatePCH.h"
#include "OptitrackClientOrigin.h"

#include "OptitrackNatnet.h"
#include "OptitrackRigidBody.h"

#include <NatNetClient.h>

#include <string>


AOptitrackClientOrigin::AOptitrackClientOrigin( const FObjectInitializer& ObjectInitializer )
	: Super( ObjectInitializer )
	, Client( nullptr )
{
	ServerAddress = "127.0.0.1";
	ClientAddress = "127.0.0.1";
	ConnectionType = EOptitrackClientConnectionType::Multicast;

	RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>( this, TEXT( "RootSceneComponent" ) );

#if WITH_EDITORONLY_DATA
	RootComponent->bVisualizeComponent = true;
#endif
}


bool AOptitrackClientOrigin::GetLatestRigidBodyState( int32 RbId, FOptitrackRigidBodyState& OutRbState )
{
	if ( GetLatestRigidBodyStateUntransformed( RbId, OutRbState ) )
	{
		const FTransform optitrackRbTransform( OutRbState.Orientation, OutRbState.Position );

		const FTransform adjustedRbTransform = optitrackRbTransform * GetTransform();
		OutRbState.Position = adjustedRbTransform.GetLocation();
		OutRbState.Orientation = adjustedRbTransform.GetRotation();

		return true;
	}
	else
	{
		return false;
	}
}


bool AOptitrackClientOrigin::GetLatestRigidBodyStateUntransformed( int32 RbId, FOptitrackRigidBodyState& OutRbState )
{
	bool bResult = false;

	// NOTE: This lock blocks NatNet frame delivery for the duration.
	// See comment in NatNetDataCallback.
	FrameUpdateLock.Lock();

	if ( FOptitrackRigidBodyState* pState = LatestRigidBodyStates.Find( RbId ) )
	{
		OutRbState = *pState;
		bResult = true;
	}

	FrameUpdateLock.Unlock();

	return bResult;
}


//static
AOptitrackClientOrigin* AOptitrackClientOrigin::FindDefaultClientOrigin( UWorld* World )
{
	AOptitrackClientOrigin* returnClientOrigin = nullptr;

	int numClientOrigins = 0;
	for ( TActorIterator<AOptitrackClientOrigin> ClientOriginIter( World ); ClientOriginIter; ++ClientOriginIter )
	{
		++numClientOrigins;

		if ( numClientOrigins == 1 )
		{
			returnClientOrigin = *ClientOriginIter;
		}
	}

	if ( numClientOrigins == 0 )
	{
		GLog->Logf( TEXT( "AOptitrackClientOrigin::GetDefaultClientOrigin: Unable to locate any client origin actors!" ) );
	}
	else if ( numClientOrigins > 1 )
	{
		GLog->Logf( TEXT( "AOptitrackClientOrigin::GetDefaultClientOrigin: Multiple client origin actors; using first available (%s)" ), *returnClientOrigin->GetName() );
	}

	return returnClientOrigin;
}


//static
AOptitrackClientOrigin* AOptitrackClientOrigin::FindHmdClientOrigin( UWorld* World )
{
	AOptitrackClientOrigin* returnClientOrigin = nullptr;

	int numEligibleClientOrigins = 0;
	for ( TActorIterator<AOptitrackClientOrigin> ClientOriginIter( World ); ClientOriginIter; ++ClientOriginIter )
	{
		if ( ClientOriginIter->HmdRigidBodyId != 0 )
		{
			++numEligibleClientOrigins;

			if ( numEligibleClientOrigins == 1 )
			{
				returnClientOrigin = *ClientOriginIter;
			}
		}
	}

	if ( numEligibleClientOrigins == 0 )
	{
		GLog->Logf( TEXT( "AOptitrackClientOrigin::GetDefaultClientOrigin: Unable to locate any client origin actors that specify the HMD rigid body ID!" ) );
	}
	else if ( numEligibleClientOrigins > 1 )
	{
		GLog->Logf( TEXT( "AOptitrackClientOrigin::GetDefaultClientOrigin: Multiple client origin actors specify an HMD rigid body ID; using first available (%s)" ), *returnClientOrigin->GetName() );
	}

	return returnClientOrigin;
}


void AOptitrackClientOrigin::PreInitializeComponents()
{
	// TODO: Do we need to support this changing at runtime?
	CachedWorldToMeters = GetWorld()->GetWorldSettings()->WorldToMeters;

	InitializeClient();

	Super::PreInitializeComponents();
}


void AOptitrackClientOrigin::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	Super::EndPlay( EndPlayReason );

	ShutdownClient();
}


void AOptitrackClientOrigin::InitializeClient()
{
	check( IOptitrackNatnetPlugin::IsAvailable() );

	check( Client == nullptr );

	switch ( ConnectionType )
	{
		case EOptitrackClientConnectionType::Unicast:
			Client = IOptitrackNatnetPlugin::Get().CreateUnicastClient();
			break;
		case EOptitrackClientConnectionType::Multicast:
		default:
			Client = IOptitrackNatnetPlugin::Get().CreateMulticastClient();
			break;
	}

	check( Client != nullptr );

	Client->SetFrameReceivedCallback( &AOptitrackClientOrigin::NatNetDataCallback, this );

	const std::string strClientAddr( TCHAR_TO_ANSI( *ClientAddress ) );
	const std::string strServerAddr( TCHAR_TO_ANSI( *ServerAddress ) );

	sNatNetClientConnectParams connectParams;
	connectParams.localAddress = strClientAddr.c_str();
	connectParams.serverAddress = strServerAddr.c_str();
	const ErrorCode InitializeResult = Client->Connect( connectParams );
	if ( !ensureMsgf( InitializeResult == ErrorCode_OK, TEXT( "NatNetClient::Initialize failed with return code %d" ), InitializeResult ) )
	{
		ShutdownClient();
		return;
	}
}


void AOptitrackClientOrigin::ShutdownClient()
{
	if ( Client != nullptr )
	{
		FrameUpdateLock.Lock();

		Client->Disconnect();
		check( IOptitrackNatnetPlugin::IsAvailable() );
		IOptitrackNatnetPlugin::Get().DestroyClient( Client );
		Client = nullptr;

		FrameUpdateLock.Unlock();
	}
}


//static
void NATNET_CALLCONV AOptitrackClientOrigin::NatNetDataCallback( sFrameOfMocapData* NewFrame, void* UserData )
{
	AOptitrackClientOrigin* This = reinterpret_cast<AOptitrackClientOrigin*>(UserData);

	if ( !ensure( This != nullptr ) )
	{
		return;
	}

	// TODO: In the event of FrameUpdateLock contention, rather than blocking,
	// the frame NatNet is attempting to deliver is dropped. In practice,
	// contention should be infrequent, and the NatNet stream's frame rate is
	// generally much higher than Unreal.
	// A more sophisticated locking (or lock-free) scheme would be preferable.

	if ( This->FrameUpdateLock.TryLock() )
	{
		const double deliveryTime = FPlatformTime::Seconds();
		const float coordScalingFactor = This->CachedWorldToMeters;

		for ( int i = 0; i < NewFrame->nRigidBodies; ++i )
		{
			const sRigidBodyData& rbData = NewFrame->RigidBodies[i];

			const bool bTrackedThisFrame = (rbData.params & 0x01) != 0;
			if ( bTrackedThisFrame == false )
			{
				continue;
			}

			// Relative to OptiTrack volume origin, with conversions for: scale, Y-up to Z-up, right-handed to left-handed. (X basis is preserved.)
			const FVector position( rbData.x * coordScalingFactor, rbData.z * coordScalingFactor, rbData.y * coordScalingFactor );
			const FQuat orientation( rbData.qx, rbData.qz, rbData.qy, -rbData.qw );

			FOptitrackRigidBodyState rbState;
			rbState.DeliveryTimePlatformSeconds = deliveryTime;
			rbState.Position = position;
			rbState.Orientation = orientation;

			This->LatestRigidBodyStates.Emplace( rbData.ID, rbState );
		}

		This->FrameUpdateLock.Unlock();
	}
}
