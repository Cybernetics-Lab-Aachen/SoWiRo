//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#include "OptitrackNatnetPrivatePCH.h"

#include "OptitrackNatnet.h"

#include <Core.h>
#include <ModuleManager.h>
#include <IPluginManager.h>

#include <NatNetCAPI.h>
#include <NatNetClient.h>


#define LOCTEXT_NAMESPACE "OptitrackNatnet"


#if PLATFORM_WINDOWS
#	define NATNET_LOAD_DLL 1
#elif PLATFORM_ANDROID
#	define NATNET_LOAD_DLL 0
#else
#	error Unsupported platform!
#endif


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
	NatNetClient* InternalCreateClient( ConnectionType connectionType );

#if NATNET_LOAD_DLL
	/** Handle to the delay-loaded library. */
	void* NatNetLibHandle;
#endif
};


#if !UE_BUILD_SHIPPING
static void NATNET_CALLCONV NatNetLog( Verbosity verbosity, const char* message )
{
	FString tbuf = ANSI_TO_TCHAR( message );
	const TCHAR* levelStr = TEXT( "" );

//	if ( verbosity >= NatNetVerbosity_Debug )
//		return;

	switch ( verbosity )
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
#if NATNET_LOAD_DLL
	// Note: These paths correspond to the RuntimeDependency specified in the .Build.cs script.
	const FString PluginBaseDir = IPluginManager::Get().FindPlugin( "OptitrackNatnet" )->GetBaseDir();

#if PLATFORM_WINDOWS && PLATFORM_64BITS
	const FString LibraryPath = FPaths::Combine( *PluginBaseDir, TEXT( "ThirdParty/NatNetSDK/lib/Win64/NatNetLib.dll" ) );
#elif PLATFORM_WINDOWS && PLATFORM_32BITS
	const FString LibraryPath = FPaths::Combine( *PluginBaseDir, TEXT( "ThirdParty/NatNetSDK/lib/Win32/NatNetLib.dll" ) );
#else
#	error Path to NatNet shared library not specified for this platform!
#endif

	NatNetLibHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle( *LibraryPath ) : nullptr;

	if ( NatNetLibHandle == nullptr )
	{
		FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT( "NatNetLibLoadError", "Error: Failed to load NatNetLib.dll" ) );
	}
#endif // NATNET_LOAD_DLL

#if !UE_BUILD_SHIPPING
	uint8_t clientVer[4];
	NatNet_GetVersion( clientVer );
	GLog->Logf( TEXT( "FOptitrackNatnetModule: NatNetLib loaded, version %d.%d.%d.%d" ), clientVer[0], clientVer[1], clientVer[2], clientVer[3] );
#endif // !UE_BUILD_SHIPPING
}


// This function may be called during shutdown to clean up your module.
// For modules that support dynamic reloading, we call this function before unloading the module.
void FOptitrackNatnetModule::ShutdownModule()
{
#if NATNET_LOAD_DLL
	// Unload the DLL.
	if ( NatNetLibHandle != nullptr )
	{
		FPlatformProcess::FreeDllHandle( NatNetLibHandle );
		NatNetLibHandle = nullptr;
	}
#endif // NATNET_LOAD_DLL
}


NatNetClient* FOptitrackNatnetModule::CreateMulticastClient()
{
	return InternalCreateClient( ConnectionType_Multicast );
}


NatNetClient* FOptitrackNatnetModule::CreateUnicastClient()
{
	return InternalCreateClient( ConnectionType_Unicast );
}


NatNetClient* FOptitrackNatnetModule::InternalCreateClient( ConnectionType connectionType )
{
	NatNetClient* pClient = new NatNetClient( connectionType );

#if !UE_BUILD_SHIPPING
	NatNet_SetLogCallback( NatNetLog );
#endif // !UE_BUILD_SHIPPING

	return pClient;
}


void FOptitrackNatnetModule::DestroyClient( NatNetClient* pClient )
{
	delete pClient;
}


#undef LOCTEXT_NAMESPACE


IMPLEMENT_MODULE( FOptitrackNatnetModule, OptitrackNatnet )
