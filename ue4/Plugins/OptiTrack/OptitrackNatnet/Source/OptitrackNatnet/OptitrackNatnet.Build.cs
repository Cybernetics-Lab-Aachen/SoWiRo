//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

using Path = System.IO.Path;
using UnrealBuildTool;


public class OptitrackNatnet : ModuleRules
{
    public OptitrackNatnet( TargetInfo Target )
    {
        PublicIncludePaths.AddRange(
            new string[] {
                "OptitrackNatnet/Public",
                // ... add public include paths required here ...
            }
            );


        PrivateIncludePaths.AddRange(
            new string[] {
                "OptitrackNatnet/Private",
                // ... add other private include paths required here ...
            }
            );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Projects",
                "CoreUObject",
                "Engine",
                // ... add other public dependencies that you statically link with here ...
            }
            );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                // ... add private dependencies that you statically link with here ...
            }
            );


        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
            );


        AddThirdPartyPrivateStaticDependencies( Target
                // ... add any third party modules that your module depends on here ...
            );


        /****************************************/

        // If you update this path, ensure the DLL runtime delay load path in FOptitrackNatnetModule::StartupModule stays in sync.
        string NatNetPath = Path.GetFullPath( Path.Combine( ModuleDirectory, "..", "..", "ThirdParty", "NatNetSDK" ) );
        string NatNetLibBinPath = "";

        if ( Target.Platform == UnrealTargetPlatform.Win64 )
        {
            NatNetLibBinPath = Path.Combine( NatNetPath, "lib", "x64" );
        }
        else if ( Target.Platform == UnrealTargetPlatform.Win32 )
        {
            NatNetLibBinPath = Path.Combine( NatNetPath, "lib" );
        }

        PublicSystemIncludePaths.Add( System.IO.Path.Combine( NatNetPath, "include" ) );
        PublicLibraryPaths.Add( NatNetLibBinPath );
        PublicAdditionalLibraries.Add( "NatNetLib.lib" );
        PublicDelayLoadDLLs.Add( "NatNetLib.dll" );
        RuntimeDependencies.Add( new RuntimeDependency( Path.Combine( NatNetLibBinPath, "NatNetLib.dll" ) ) );
    }
}
