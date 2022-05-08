#include "TSModPatchProcessMemory.h"
#include "TSModCommand.h"
#include "BGMmm.h"

#include "lua/lua_interface.h"

#include <windows.h>


PFMCISENDCOMMANDA p_mciSendCommandA;
FARPROC p_mciGetDeviceIDA;
FARPROC p_mmioAdvance;
FARPROC p_mmioAscend;
FARPROC p_mmioClose;
FARPROC p_mmioDescend;
FARPROC p_mmioGetInfo;
FARPROC p_mmioOpenA;
FARPROC p_mmioRead;
FARPROC p_mmioSeek;
FARPROC p_mmioSetInfo;
FARPROC p_mmioWrite;
PFTIMEGETTIME p_timeGetTime;

HINSTANCE h_original_winmm;


// luacom用 の BOOL APIENTRY DllMain( HANDLE hModule, 
extern BOOL APIENTRY tLLuaCom_DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved );


BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved ) {
 switch( ul_reason_for_call )
 {
 case DLL_PROCESS_ATTACH:

	// Luaインタプリタロード
	LoadLuaInterpreter();

	// TSMod.iniで書いたパッチの適応
	TDModPatchProcessMemory();

	 // １か２ならINMM
	if ( TSModCommand::Environment::cmdBGMMode == 1 || TSModCommand::Environment::cmdBGMMode == 2 ) {
		 h_original_winmm = LoadLibrary( "_inmm.dll" );
		 if ( h_original_winmm ) {
			 p_mciSendCommandA = (PFMCISENDCOMMANDA)GetProcAddress( h_original_winmm, "_ciSendCommandA" );
			 p_mciGetDeviceIDA = GetProcAddress( h_original_winmm, "_ciGetDeviceIDA" );
			 p_mmioAdvance = GetProcAddress( h_original_winmm, "_mioAdvance" );
			 p_mmioAscend = GetProcAddress( h_original_winmm, "_mioAscend" );
			 p_mmioClose = GetProcAddress( h_original_winmm, "_mioClose" );
			 p_mmioDescend = GetProcAddress( h_original_winmm, "_mioDescend" );
			 p_mmioGetInfo = GetProcAddress( h_original_winmm, "_mioGetInfo" );
			 p_mmioOpenA = GetProcAddress( h_original_winmm, "_mioOpenA" );
			 p_mmioRead = GetProcAddress( h_original_winmm, "_mioRead" );
			 p_mmioSeek = GetProcAddress( h_original_winmm, "_mioSeek" );
			 p_mmioSetInfo = GetProcAddress( h_original_winmm, "_mioSetInfo" );
			 p_mmioWrite = GetProcAddress( h_original_winmm, "_mioWrite" );
			 p_timeGetTime = (PFTIMEGETTIME)GetProcAddress( h_original_winmm, "_imeGetTime" );
		 }

		 // それ以外(通常０）は、デフォルトのwinmm
	 } else {
		 h_original_winmm = LoadLibrary( "winmm.dll" );
		 if ( h_original_winmm ) {
			 p_mciSendCommandA = (PFMCISENDCOMMANDA)GetProcAddress( h_original_winmm, "mciSendCommandA" );
			 p_mciGetDeviceIDA = GetProcAddress( h_original_winmm, "mciGetDeviceIDA" );
			 p_mmioAdvance = GetProcAddress( h_original_winmm, "mmioAdvance" );
			 p_mmioAscend = GetProcAddress( h_original_winmm, "mmioAscend" );
			 p_mmioClose = GetProcAddress( h_original_winmm, "mmioClose" );
			 p_mmioDescend = GetProcAddress( h_original_winmm, "mmioDescend" );
			 p_mmioGetInfo = GetProcAddress( h_original_winmm, "mmioGetInfo" );
			 p_mmioOpenA = GetProcAddress( h_original_winmm, "mmioOpenA" );
			 p_mmioRead = GetProcAddress( h_original_winmm, "mmioRead" );
			 p_mmioSeek = GetProcAddress( h_original_winmm, "mmioSeek" );
			 p_mmioSetInfo = GetProcAddress( h_original_winmm, "mmioSetInfo" );
			 p_mmioWrite = GetProcAddress( h_original_winmm, "mmioWrite" );
			 p_timeGetTime = (PFTIMEGETTIME)GetProcAddress( h_original_winmm, "timeGetTime" );

		 } else {
			 return false;
		 }
	 }

	 // luacom用
//	 tLLuaCom_DllMain( hModule, ul_reason_for_call, lpReserved);

     break;
 case DLL_THREAD_ATTACH:
     break;
 case DLL_THREAD_DETACH:
     break;
 case DLL_PROCESS_DETACH:

//	 tLLuaCom_DllMain( hModule, ul_reason_for_call, lpReserved);

 	// luaインタプリタ解放
	FreeLuaInterpreter();

	FreeLibrary( h_original_winmm );
     break;
 default:
     break;
 }
 return TRUE;
 }
