#pragma once

#include <windows.h>

// フックする関数のプロトタイプを定義
typedef MCIERROR (WINAPI *PFMCISENDCOMMANDA)(MCIDEVICEID, UINT, DWORD, DWORD );
typedef DWORD (WINAPI *PFTIMEGETTIME)(VOID);

extern PFMCISENDCOMMANDA p_mciSendCommandA;
extern FARPROC p_mciGetDeviceIDA;
extern FARPROC p_mmioAdvance;
extern FARPROC p_mmioAscend;
extern FARPROC p_mmioClose;
extern FARPROC p_mmioDescend;
extern FARPROC p_mmioGetInfo;
extern FARPROC p_mmioOpenA;
extern FARPROC p_mmioRead;
extern FARPROC p_mmioSeek;
extern FARPROC p_mmioSetInfo;
extern FARPROC p_mmioWrite;
extern PFTIMEGETTIME p_timeGetTime;

