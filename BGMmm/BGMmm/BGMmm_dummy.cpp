#include "WinVersion.h"
#include "BGMmm.h"
#include "TSModCommand.h"

#include <windows.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <mmsystem.h>

#include <map>

using namespace std;

// #define BGMmmDEBUG 1

#if BGMmmDEBUG

void OutputDebugStream( const char *format, ... ) {

	char szBufDebug[4096] = "";

    va_list arg;

    va_start(arg, format);
    vsprintf_s(szBufDebug, format, arg);
    va_end(arg);

    OutputDebugString( szBufDebug );

}

#else

// 完全無効化
#define OutputDebugStream __noop

#endif



/*
#define MCI_OPEN                        0x0803
#define MCI_CLOSE                       0x0804
#define MCI_ESCAPE                      0x0805
#define MCI_PLAY                        0x0806
#define MCI_SEEK                        0x0807
#define MCI_STOP                        0x0808
#define MCI_PAUSE                       0x0809
#define MCI_INFO                        0x080A
#define MCI_GETDEVCAPS                  0x080B
#define MCI_SPIN                        0x080C
#define MCI_SET                         0x080D
#define MCI_STEP                        0x080E
#define MCI_RECORD                      0x080F
#define MCI_SYSINFO                     0x0810
#define MCI_BREAK                       0x0811
#define MCI_SAVE                        0x0813
#define MCI_STATUS                      0x0814		← n6paudioの中で頻繁にMCI_STATUSをチェックしている。ループ判定やループ処理はn6paudio.dllの中でのみ行われている。
#define MCI_CUE                         0x0830
#define MCI_REALIZE                     0x0840
#define MCI_WINDOW                      0x0841
#define MCI_PUT                         0x0842
#define MCI_WHERE                       0x0843
#define MCI_FREEZE                      0x0844
#define MCI_UNFREEZE                    0x0845
#define MCI_LOAD                        0x0850
#define MCI_CUT                         0x0851
#define MCI_COPY                        0x0852
#define MCI_PASTE                       0x0853
#define MCI_UPDATE                      0x0854
#define MCI_RESUME                      0x0855
#define MCI_DELETE                      0x0856
*/

/*
MM_MCINOTIFY							0x3B9
*/


map<MCIDEVICEID, int> mapMCIDEVICEID;


void msg_mciGetDeviceIDA() { OutputDebugStream("jmp p_mciGetDeviceIDA"); }
void msg_mmioAdvance() { OutputDebugStream("jmp p_mmioAdvance"); }
void msg_mmioAscend() { OutputDebugStream("jmp p_mmioAscend"); }
void msg_mmioClose() { OutputDebugStream("jmp p_mmioClose"); }
void msg_mmioDescend() { OutputDebugStream("jmp p_mmioDescend"); }
void msg_mmioGetInfo() { OutputDebugStream("jmp p_mmioGetInfo"); }
void msg_mmioOpenA() { OutputDebugStream("jmp p_mmioOpenA"); }
void msg_mmioRead() { OutputDebugStream("jmp p_mmioRead"); }
void msg_mmioSeek() { OutputDebugStream("jmp p_mmioSeek"); }
void msg_mmioSetInfo() { OutputDebugStream("jmp p_mmioSetInfo"); }
void msg_mmioWrite() { OutputDebugStream("jmp p_mmioWrite"); }
void msg_timeGetTime() { /*OutputDebugStream("jmp p_timeGetTime");*/ }


static BOOL isLastPlayUMsg = FALSE;			// 最後にプレイしているという命令だった？;
static int iLastBGMLengthSecond = 0;		// 最後に再生していた曲の長さ(秒)
static DWORD preTimeGetTime = 0;
static BOOL IsShouldBeRePlayLastestBGM = FALSE; // 最後のBGMをもう一度再生するべきか？


// CDではなく、直接再生するためのデバイス
static MCI_OPEN_PARMS open2 = {0};
static MCI_PLAY_PARMS play2 = {0};

extern MCIERROR WINAPI Play_NotCD_ButFromFile(MCIDEVICEID& IDDevice, UINT& uMsg, DWORD& fdwCommand, DWORD& dwParam ); // 元値を書き換える可能性があるので、全て参照で


MCIERROR WINAPI d_mciSendCommandA(MCIDEVICEID IDDevice, UINT uMsg, DWORD fdwCommand, DWORD dwParam ) {

	// 先にオリジナルに渡す。
	MCIERROR mirror = p_mciSendCommandA(IDDevice, uMsg, fdwCommand, dwParam );

	// １か２ならINMM。INMMなら何も考えなくても良い。INMMが上手くやるだろう。
	if ( TSModCommand::Environment::cmdBGMMode == 1 || TSModCommand::Environment::cmdBGMMode == 2 ) {
		OutputDebugStream("TSModCommand::Environment::cmdBGMModeは1 or 2");
		return mirror;
	}

	//--------------------------------- _INMM 以外の場合。ループしないというバグを除去する。

	if ( uMsg != MCI_STATUS ) {
		OutputDebugStream("jmp p_mciSendCommandA:");
		OutputDebugStream("デバイス識別子:%d\n", IDDevice);
		OutputDebugStream("コマンドメッセージ%x\n", uMsg);
		OutputDebugStream("フラグ%x", fdwCommand);
		OutputDebugStream("エラー%d", mirror );
		OutputDebugStream("パラメタ結果:%d\n---------------------------------------\n", dwParam);

	}
	/*
	if ( uMsg == MCI_STATUS ) {
		MCI_STATUS_PARMS *mciStatus;
		mciStatus = (MCI_STATUS_PARMS *)dwParam;

		if ( mciStatus->dwItem == MCI_CDA_STATUS_TYPE_TRACK ) {
			OutputDebugString("トラックかどうかの判定!!" );
		}
		
		// こういったトラックかどうかの判定をなんとかクリアしなければならない。
		mciStatus.dwItem  = MCI_CDA_STATUS_TYPE_TRACK;
		mciStatus.dwTrack = 1;
		mciSendCommand(mciDeviceId, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD_PTR)&mciStatus);
		BOOL isREAULT = (mciStatus.dwReturn == MCI_CDA_TRACK_AUDIO)
	}
	*/


	// オープン時
	if ( uMsg == MCI_OPEN ) {

		MCI_OPEN_PARMS *mop = (MCI_OPEN_PARMS *)dwParam;
		// オープンの種類のメッセージが送られてきた時
		if ( mop ) {
			// デバイスタイプがCDの時、
			if ( (LPSTR)(mop->lpstrDeviceType) == (LPSTR)MCI_DEVTYPE_CD_AUDIO ) {

				mapMCIDEVICEID[(MCIDEVICEID)mop->wDeviceID] = 1; // オープンの時に与えられたデバイスIDを保持
				OutputDebugStream("★MCI-OPEN-CD★");
				OutputDebugStream("%s", mop->lpstrAlias);
				; // CDがオープンされた!!。
			}
		}
	}

	// DVDタイプ
	if ( TSModCommand::Environment::cmdBGMMode == 3 ) {
		Play_NotCD_ButFromFile(IDDevice, uMsg, fdwCommand, dwParam );
	}


	if ( TSModCommand::Environment::cmdBGMMode == 0 ) {

		// トラック長を得る
		// 再生時にパラメータを受け取る
		if ( uMsg == MCI_PLAY ) {
			// プレイの時のパラメータ
			MCI_PLAY_PARMS *mpp = (MCI_PLAY_PARMS *)dwParam;
			if ( mpp ) {
				OutputDebugStream("開始%d - 終了%d\n", (int)mpp->dwFrom, (int)mpp->dwTo ); // このFromが音楽再生番号そのものである。(TSmod.txtや神パッチの番号と一致する)

				if ( mpp->dwFrom > 0 && mpp->dwTo > 0 ) {
					// トラック長を得る
					MCI_SET_PARMS set;
					MCI_STATUS_PARMS status;

					set.dwTimeFormat = MCI_FORMAT_MSF;
					p_mciSendCommandA(IDDevice, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID)&set);

					status.dwItem = MCI_STATUS_LENGTH;
					status.dwTrack = mpp->dwFrom; // 再生トラック目の長さを得る場合
					p_mciSendCommandA(IDDevice, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD)(LPVOID)&status);

					/// 独特なフォーマットなので、秒に変換
					int nTime = MCI_MSF_MINUTE(status.dwReturn) * 60 + MCI_MSF_SECOND(status.dwReturn);

					if ( (mpp->dwTo - mpp->dwFrom) == 1) { // トラックがちゃんとい１つ指定してある。
						iLastBGMLengthSecond = nTime; // 直近で再生した曲の秒数として保存

						OutputDebugStream("長さ秒%d", nTime);
					} else {
						iLastBGMLengthSecond = 0;
					}
				}
			}
		}

		// ストップやシーク系のどれかが来た。再生に関する重要な命令として受け入れる
		if ( uMsg == MCI_SET || uMsg == MCI_PAUSE || uMsg == MCI_STOP || uMsg == MCI_SEEK ) {
			isLastPlayUMsg = FALSE;
			IsShouldBeRePlayLastestBGM = FALSE;

			// タイマーカウント終了
			preTimeGetTime = p_timeGetTime();

		// 再生が来た。再生コマンドの更新
		} else if ( uMsg == MCI_PLAY && iLastBGMLengthSecond ) {
			IsShouldBeRePlayLastestBGM = FALSE;

			// 最後に再生したを真とする。
			isLastPlayUMsg = TRUE;

			// タイマーカウント終了
			preTimeGetTime = p_timeGetTime();

		// 再生に関係する重要なコマンドではない。
		} else {
			// タイマーカウント続行

			// 最後の有効コマンドは「再生」のまま。
			if ( isLastPlayUMsg && iLastBGMLengthSecond > 0 ) {
				double iSubSecond = (p_timeGetTime() - preTimeGetTime);
				iSubSecond = iSubSecond / 1000.0f; // ミリ秒を秒に直す。

				// 最後に再生した曲が終わってから、1秒経過したっぽい。
				if ( iSubSecond > iLastBGMLengthSecond + 1 ) {
					OutputDebugStream("最後の曲が再生してから1秒経った!!");
					IsShouldBeRePlayLastestBGM = TRUE;
				}
			}
		}
	}

	return mirror;
}



MCIERROR WINAPI Play_NotCD_ButFromFile(MCIDEVICEID& IDDevice, UINT& uMsg, DWORD& fdwCommand, DWORD& dwParam ) {

	//------------------------------------------------------------------------------------------------------------
	// ここからは、CD再生要求があった際に、CDからではなく、外部のファイルから直接再生する用途!!
	//------------------------------------------------------------------------------------------------------------
	// 再生時にパラメータを受け取る。CDオープン用として要求されてたデバイスID
	if ( uMsg == MCI_PLAY && mapMCIDEVICEID[IDDevice] ) {

		if ( open2.wDeviceID ) {
			p_mciSendCommandA( open2.wDeviceID, MCI_STOP, 0, 0 );
			p_mciSendCommandA( open2.wDeviceID, MCI_CLOSE, 0, 0 );
		}

		// プレイの時のパラメータ
		MCI_PLAY_PARMS *mpp = (MCI_PLAY_PARMS *)dwParam;
		if ( mpp ) {
			int iTargetTrackNo = (int)mpp->dwFrom;

			static char szFilePath[MAX_PATH] = "";
			wsprintf( szFilePath, "bdat\\%02d.dat", iTargetTrackNo-1 );

			// さらに
			struct stat  st;
			// ファイルが存在すれば
			if (stat(szFilePath, &st) == 0 ) {

				OutputDebugStream("★MCI-OPEN-CD★");

				//mp3再生の場合
				open2.lpstrDeviceType = "MPEGVideo";
				open2.lpstrElementName = szFilePath;
				int result = p_mciSendCommandA( 0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, ( DWORD_PTR )&open2 );

				//エラーなら０以外が返る
				if( result ) {
					//エラー取得
					// p_mciGetErrorString( result, buf, sizeof( buf ) );
					// return;
				}
		        
				// play2.dwCallback = ( DWORD )hwnd;
				result = p_mciSendCommandA( open2.wDeviceID, MCI_PLAY, MCI_NOTIFY, ( DWORD_PTR )&play2 );
				//エラーなら０以外が返る
				if( result ) {
					//エラー取得
					// p_mciGetErrorString( result, buf, sizeof( buf ) );
					// return;
				}
			} else {
				OutputDebugString("cmd['environment_bgm_mode'] = 3\nはDVD用のオプションとなります。\nファイル構成を満たしていません。");
			}

		}
	}

	if ( uMsg == MCI_STATUS && mapMCIDEVICEID[IDDevice] ) {
		if ( open2.wDeviceID ) {
			// 外部ファイル（01.datなど)の方で鳴らしている状況を得る
			MCI_STATUS_PARMS mciStatus;
			
			mciStatus.dwItem = MCI_STATUS_MODE;
			p_mciSendCommandA(open2.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatus);

			// その状況をCDのステータスとして偽造
			MCI_STATUS_PARMS *orgParam = (MCI_STATUS_PARMS *)dwParam;
			orgParam->dwReturn = mciStatus.dwReturn;
		}

	}

	if ( uMsg == MCI_STOP && mapMCIDEVICEID[IDDevice] ) {
		p_mciSendCommandA( open2.wDeviceID, MCI_STOP, 0, 0 );
		p_mciSendCommandA( open2.wDeviceID, MCI_CLOSE, 0, 0 );
	}

	if ( uMsg == MCI_CLOSE && mapMCIDEVICEID[IDDevice] ) {
		p_mciSendCommandA( open2.wDeviceID, MCI_STOP, 0, 0 );
		p_mciSendCommandA( open2.wDeviceID, MCI_CLOSE, 0, 0 );
	}
	//------------------------------------------------------------------------------------------------------------
	// こまでが、CD再生要求があった際に、CDからではなく、外部のファイルから直接再生する用途!!
	//------------------------------------------------------------------------------------------------------------

	return NULL;
}



// 最後の曲をもう一度再生するべきか？ TSModから参照。
__declspec(dllexport) int BGMmm_GetShouldBeRePlayLastestBGM() {
	// CDモードのVista以降の時だけ、ループしないというエラーがある。
	if ( TSModCommand::Environment::cmdBGMMode == 0 ) {
		if ( getOSVersion() >= 6.0 ) {
			return IsShouldBeRePlayLastestBGM;
		}
	}
	return FALSE;
}


DWORD WINAPI d_timeGetTime(VOID) {
	return p_timeGetTime();
}


#if BGMmmDEBUG

__declspec( naked ) void WINAPI d_mciGetDeviceIDA() { 
	_asm{
		call msg_mciGetDeviceIDA
		jmp p_mciGetDeviceIDA
	}
}
__declspec( naked ) void WINAPI d_mmioAdvance() {
	_asm{
		call msg_mmioAdvance
		jmp p_mmioAdvance
	}
}
__declspec( naked ) void WINAPI d_mmioAscend() {
	_asm{
		call msg_mmioAscend
		jmp p_mmioAscend
	}
}
__declspec( naked ) void WINAPI d_mmioClose() { 
	_asm{ 
		call msg_mmioClose
		jmp p_mmioClose
	}
}
__declspec( naked ) void WINAPI d_mmioDescend() { 
	_asm{
		call msg_mmioDescend
		jmp p_mmioDescend
	}
}
__declspec( naked ) void WINAPI d_mmioGetInfo() {
	_asm{
		call msg_mmioGetInfo
		jmp p_mmioGetInfo
	}
}
__declspec( naked ) void WINAPI d_mmioOpenA() {
	_asm{
		call msg_mmioOpenA
		jmp p_mmioOpenA
	}
}
__declspec( naked ) void WINAPI d_mmioRead() { 
	_asm{
		call msg_mmioRead
		jmp p_mmioRead
	}
}
__declspec( naked ) void WINAPI d_mmioSeek() {
	_asm{
		call msg_mmioSeek
		jmp p_mmioSeek
	}
}
__declspec( naked ) void WINAPI d_mmioSetInfo() { 
	_asm{
		call msg_mmioSetInfo
		jmp p_mmioSetInfo
	}
}
__declspec( naked ) void WINAPI d_mmioWrite() { 
	_asm{
		call msg_mmioWrite
		jmp p_mmioWrite
	}
}


#else

__declspec( naked ) void WINAPI d_mciGetDeviceIDA() { 
	_asm{
		jmp p_mciGetDeviceIDA
	}
}
__declspec( naked ) void WINAPI d_mmioAdvance() {
	_asm{
		jmp p_mmioAdvance
	}
}
__declspec( naked ) void WINAPI d_mmioAscend() {
	_asm{
		jmp p_mmioAscend
	}
}
__declspec( naked ) void WINAPI d_mmioClose() { 
	_asm{ 
		jmp p_mmioClose
	}
}
__declspec( naked ) void WINAPI d_mmioDescend() { 
	_asm{
		jmp p_mmioDescend
	}
}
__declspec( naked ) void WINAPI d_mmioGetInfo() {
	_asm{
		jmp p_mmioGetInfo
	}
}
__declspec( naked ) void WINAPI d_mmioOpenA() {
	_asm{
		jmp p_mmioOpenA
	}
}
__declspec( naked ) void WINAPI d_mmioRead() { 
	_asm{
		jmp p_mmioRead
	}
}
__declspec( naked ) void WINAPI d_mmioSeek() {
	_asm{
		jmp p_mmioSeek
	}
}
__declspec( naked ) void WINAPI d_mmioSetInfo() { 
	_asm{
		jmp p_mmioSetInfo
	}
}
__declspec( naked ) void WINAPI d_mmioWrite() { 
	_asm{
		jmp p_mmioWrite
	}
}

#endif