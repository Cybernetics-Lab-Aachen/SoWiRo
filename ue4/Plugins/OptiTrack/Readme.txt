OptiTrack Unreal Engine 4 Plugin
================================

The OptiTrack Unreal Engine Plugin allows you to stream real-time rigid body
and HMD tracking data into Unreal Engine 4.

For more information, refer to the documentation at the following URL:

    http://wiki.optitrack.com/index.php?title=OptiTrack_Unreal_Engine_4_Plugin



Installation
------------

The included plugins, contained within the "OptiTrack" directory, should be
extracted to your engine plugins directory. By default, this is:

    C:\Program Files (x86)\Epic Games\4.13\Engine\Plugins

You should end up with a directory hierarchy such as the following:

    C:\Program Files (x86)\Epic Games\4.13\Engine\
    |-- Plugins
        |-- OptiTrack
            |-- OptitrackNatnet
                |-- OptitrackNatnet.uplugin
                |-- ...



Version History / Changelog
---------------------------

1.3.0 (2016-10-14)
-----
- Unreal Engine 4.13 compatibility.
- Improved handling of stale pose data for untracked rigid bodies.
- Improved detection and handling of client connection errors.
- Fixed a potential race condition during shutdown.


1.2.0 (2016-08-23)
-----
- Unreal Engine 4.12 compatibility.
- Gear VR support.


1.1.0 (2016-05-26)
-----
- Rift plugin updated for UE 4.11.1+ and Oculus SDK 1.3+. Supports Rift CV1.
- Added a check and error message when attaching a rigid body component to a
  client origin actor, which would otherwise cause bad behavior.
- Fixed rigid body pose timestamps getting truncated (double -> float).
- Use FCriticalSection instead of CRITICAL_SECTION.


1.0.0 (2016-03-14)
-----
- Initial release.
