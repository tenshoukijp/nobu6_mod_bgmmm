#pragma once

#ifdef NO_OUTPUT_DEBUG_STRING
#undef OutputDebugString
#define OutputDebugString  __noop
#endif


double getOSVersion();