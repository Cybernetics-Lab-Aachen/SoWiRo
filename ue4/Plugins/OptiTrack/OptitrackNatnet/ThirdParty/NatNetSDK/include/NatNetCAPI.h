//======================================================================================================
// Copyright 2016, NaturalPoint Inc.
//======================================================================================================

#pragma once

#include "NatNetTypes.h"


#if defined( __cplusplus )
extern "C" {
#endif


NATNET_API void NATNET_CALLCONV NatNet_GetVersion( unsigned char outVersion[4] );
NATNET_API void NATNET_CALLCONV NatNet_SetLogCallback( NatNetLogCallback pfnLogCallback );

NATNET_API void NATNET_CALLCONV NatNet_DecodeID( int compositeId, int* pOutEntityId, int* pOutMemberId );
NATNET_API ErrorCode NATNET_CALLCONV NatNet_DecodeTimecode( unsigned int timecode, unsigned int timecodeSubframe, int* pOutHour, int* pOutMinute, int* pOutSecond, int* pOutFrame, int* pOutSubframe );
NATNET_API ErrorCode NATNET_CALLCONV NatNet_TimecodeStringify( unsigned int timecode, unsigned int timecodeSubframe, char* outBuffer, int outBufferSize );

// Helper that performs a deep copy from pSrc into pDst.
// Some members of pDst will be dynamically allocated; use NatNet_FreeFrame( pDst ) to clean them up.
NATNET_API ErrorCode NATNET_CALLCONV NatNet_CopyFrame( sFrameOfMocapData* pSrc, sFrameOfMocapData* pDst );

// Frees the dynamic members of a frame copy created using NatNet_CopyFrame.
// Do not call this on any frame that was not the destination of such a copy.
NATNET_API ErrorCode NATNET_CALLCONV NatNet_FreeFrame( sFrameOfMocapData* pFrame );


#if defined( __cplusplus )
} // extern "C"
#endif
