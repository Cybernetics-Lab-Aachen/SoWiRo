//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

using Path = System.IO.Path;
using UnrealBuildTool;


public class OptitrackNatnet : ModuleRules
{
    public OptitrackNatnet( TargetInfo Target )
    {
        Definitions.Add( "NATNETLIB_IMPORTS" );

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


        /****************************************/

        // If you update this path, ensure the DLL runtime delay load path in FOptitrackNatnetModule::StartupModule stays in sync.
        string NatNetPath = Path.GetFullPath( Path.Combine( ModuleDirectory, "..", "..", "ThirdParty", "NatNetSDK" ) );
        PublicSystemIncludePaths.Add( System.IO.Path.Combine( NatNetPath, "include" ) );

        if ( Target.Platform == UnrealTargetPlatform.Win32 || Target.Platform == UnrealTargetPlatform.Win64 )
        {
            string NatNetLibBinPath = Path.Combine( NatNetPath, "lib", Target.Platform == UnrealTargetPlatform.Win32 ? "Win32" : "Win64" );
            PublicLibraryPaths.Add( NatNetLibBinPath );
            PublicAdditionalLibraries.Add( "NatNetLib.lib" );
            PublicDelayLoadDLLs.Add( "NatNetLib.dll" );
            RuntimeDependencies.Add( new RuntimeDependency( Path.Combine( NatNetLibBinPath, "NatNetLib.dll" ) ) );
        }
        else if ( Target.Platform == UnrealTargetPlatform.Android )
        {
            //AdditionalPropertiesForReceipt.Add( new ReceiptProperty( "AndroidPlugin", Path.Combine( ModuleDirectory, "NatNet_APL.xml" ) ) );

            // toolchain will filter
            string NatNetLibBinPath = Path.Combine( NatNetPath, "lib", "Android" );
            PublicLibraryPaths.Add( Path.Combine( NatNetLibBinPath, "arm64-v8a" ) );
            //PublicLibraryPaths.Add( Path.Combine( NatNetLibBinPath, "armeabi" ) );
            PublicLibraryPaths.Add( Path.Combine( NatNetLibBinPath, "armeabi-v7a" ) );
            PublicLibraryPaths.Add( Path.Combine( NatNetLibBinPath, "mips" ) );
            PublicLibraryPaths.Add( Path.Combine( NatNetLibBinPath, "mips64" ) );
            PublicLibraryPaths.Add( Path.Combine( NatNetLibBinPath, "x86" ) );
            PublicLibraryPaths.Add( Path.Combine( NatNetLibBinPath, "x86_64" ) );

            PublicAdditionalLibraries.Add( "NatNetLib" );
        }
    }
}
