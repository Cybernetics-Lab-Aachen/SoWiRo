//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#include "OptitrackNatnetPrivatePCH.h"

#include "OptitrackNatnet.h"

#include <Core.h>
#include <ModuleManager.h>
#include <IPluginManager.h>

#include <NatNetClient.h>


#define LOCTEXT_NAMESPACE "OptitrackNatnet"


class FOptitrackNatnetModule : public IOptitrackNatnetPlugin
{
public:
	// IModuleInterface implementation
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	// IOptitrackNatnetPlugin implementation
	virtual NatNetClient* CreateMulticastClient() override;
	virtual NatNetClient* CreateUnicastClient() override;
	virtual void DestroyClient( class NatNetClient* pClient ) override;

private:
	NatNetClient* InternalCreateClient( ConnectionType connType );

	/** Handle to the delay-loaded library. */
	void* NatNetLibHandle;
};


#if !UE_BUILD_SHIPPING
static void __cdecl NatNetLogCallback( int level, char* message )
{
	FString tbuf = ANSI_TO_TCHAR( message );
	const TCHAR* levelStr = TEXT( "" );
	switch ( level )
	{
		case Verbosity_Info:    levelStr = TEXT( " Info:" ); break;
		case Verbosity_Warning: levelStr = TEXT( " Warning:" ); break;
		case Verbosity_Error:   levelStr = TEXT( " Error:" ); break;
		case Verbosity_Debug:   levelStr = TEXT( " Debug:" ); break;
	}

	GLog->Logf( TEXT( "NatNet:%s %s" ), levelStr, *tbuf );
}
#endif // !UE_BUILD_SHIPPING


// This code will execute after your module is loaded into memory; the exact timing depends on the per-module LoadingPhase specified in the .uplugin file.
void FOptitrackNatnetModule::StartupModule()
{
	// Note: These paths correspond to the RuntimeDependency specified in the .Build.cs script.
	const FString PluginBaseDir = IPluginManager::Get().FindPlugin( "OptitrackNatnet" )->GetBaseDir();

#if PLATFORM_WINDOWS && PLATFORM_64BITS
	const FString LibraryPath = FPaths::Combine( *PluginBaseDir, TEXT( "ThirdParty/NatNetSDK/lib/x64/NatNetLib.dll" ) );
#elif PLATFORM_WINDOWS && PLATFORM_32BITS
	const FString LibraryPath = FPaths::Combine( *PluginBaseDir, TEXT( "ThirdParty/NatNetSDK/lib/NatNetLib.dll" ) );
#else
#	error Unsupported platform!
#endif

	NatNetLibHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle( *LibraryPath ) : nullptr;

	if ( NatNetLibHandle )
	{
#if !UE_BUILD_SHIPPING
		NatNetClient* pClient = new NatNetClient();

		uint8_t clientVer[4];
		pClient->NatNetVersion( clientVer );
		GLog->Logf( TEXT( "FOptitrackNatnetModule: NatNetLib loaded, version %d.%d.%d.%d" ), clientVer[0], clientVer[1], clientVer[2], clientVer[3] );

		pClient->Uninitialize();
		delete pClient;
#endif // !UE_BUILD_SHIPPING
	}
	else
	{
		FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT( "NatNetLibLoadError", "Error: Failed to load NatNetLib.dll" ) );
	}
}


// This function may be called during shutdown to clean up your module.
// For modules that support dynamic reloading, we call this function before unloading the module.
void FOptitrackNatnetModule::ShutdownModule()
{
	// Unload the DLL.
	if ( NatNetLibHandle != nullptr )
	{
		FPlatformProcess::FreeDllHandle( NatNetLibHandle );
		NatNetLibHandle = nullptr;
	}
}


NatNetClient* FOptitrackNatnetModule::CreateMulticastClient()
{
	return InternalCreateClient( ConnectionType_Multicast );
}


NatNetClient* FOptitrackNatnetModule::CreateUnicastClient()
{
	return InternalCreateClient( ConnectionType_Unicast );
}


NatNetClient* FOptitrackNatnetModule::InternalCreateClient( ConnectionType connType )
{
	NatNetClient* pClient = new NatNetClient( connType );

#if !UE_BUILD_SHIPPING
	pClient->SetVerbosityLevel( Verbosity_Info );
	pClient->SetMessageCallback( NatNetLogCallback );
#endif // !UE_BUILD_SHIPPING

	return pClient;
}


void FOptitrackNatnetModule::DestroyClient( NatNetClient* pClient )
{
	delete pClient;
}


#undef LOCTEXT_NAMESPACE


IMPLEMENT_MODULE( FOptitrackNatnetModule, OptitrackNatnet )
