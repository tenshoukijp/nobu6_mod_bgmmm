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

// ���S������
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
#define MCI_STATUS                      0x0814		�� n6paudio�̒��ŕp�ɂ�MCI_STATUS���`�F�b�N���Ă���B���[�v����⃋�[�v������n6paudio.dll�̒��ł̂ݍs���Ă���B
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


static BOOL isLastPlayUMsg = FALSE;			// �Ō�Ƀv���C���Ă���Ƃ������߂������H;
static int iLastBGMLengthSecond = 0;		// �Ō�ɍĐ����Ă����Ȃ̒���(�b)
static DWORD preTimeGetTime = 0;
static BOOL IsShouldBeRePlayLastestBGM = FALSE; // �Ō��BGM��������x�Đ�����ׂ����H


// CD�ł͂Ȃ��A���ڍĐ����邽�߂̃f�o�C�X
static MCI_OPEN_PARMS open2 = {0};
static MCI_PLAY_PARMS play2 = {0};

extern MCIERROR WINAPI Play_NotCD_ButFromFile(MCIDEVICEID& IDDevice, UINT& uMsg, DWORD& fdwCommand, DWORD& dwParam ); // ���l������������\��������̂ŁA�S�ĎQ�Ƃ�


MCIERROR WINAPI d_mciSendCommandA(MCIDEVICEID IDDevice, UINT uMsg, DWORD fdwCommand, DWORD dwParam ) {

	// ��ɃI���W�i���ɓn���B
	MCIERROR mirror = p_mciSendCommandA(IDDevice, uMsg, fdwCommand, dwParam );

	// �P���Q�Ȃ�INMM�BINMM�Ȃ牽���l���Ȃ��Ă��ǂ��BINMM����肭��邾�낤�B
	if ( TSModCommand::Environment::cmdBGMMode == 1 || TSModCommand::Environment::cmdBGMMode == 2 ) {
		OutputDebugStream("TSModCommand::Environment::cmdBGMMode��1 or 2");
		return mirror;
	}

	//--------------------------------- _INMM �ȊO�̏ꍇ�B���[�v���Ȃ��Ƃ����o�O����������B

	if ( uMsg != MCI_STATUS ) {
		OutputDebugStream("jmp p_mciSendCommandA:");
		OutputDebugStream("�f�o�C�X���ʎq:%d\n", IDDevice);
		OutputDebugStream("�R�}���h���b�Z�[�W%x\n", uMsg);
		OutputDebugStream("�t���O%x", fdwCommand);
		OutputDebugStream("�G���[%d", mirror );
		OutputDebugStream("�p�����^����:%d\n---------------------------------------\n", dwParam);

	}
	/*
	if ( uMsg == MCI_STATUS ) {
		MCI_STATUS_PARMS *mciStatus;
		mciStatus = (MCI_STATUS_PARMS *)dwParam;

		if ( mciStatus->dwItem == MCI_CDA_STATUS_TYPE_TRACK ) {
			OutputDebugString("�g���b�N���ǂ����̔���!!" );
		}
		
		// �����������g���b�N���ǂ����̔�����Ȃ�Ƃ��N���A���Ȃ���΂Ȃ�Ȃ��B
		mciStatus.dwItem  = MCI_CDA_STATUS_TYPE_TRACK;
		mciStatus.dwTrack = 1;
		mciSendCommand(mciDeviceId, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD_PTR)&mciStatus);
		BOOL isREAULT = (mciStatus.dwReturn == MCI_CDA_TRACK_AUDIO)
	}
	*/


	// �I�[�v����
	if ( uMsg == MCI_OPEN ) {

		MCI_OPEN_PARMS *mop = (MCI_OPEN_PARMS *)dwParam;
		// �I�[�v���̎�ނ̃��b�Z�[�W�������Ă�����
		if ( mop ) {
			// �f�o�C�X�^�C�v��CD�̎��A
			if ( (LPSTR)(mop->lpstrDeviceType) == (LPSTR)MCI_DEVTYPE_CD_AUDIO ) {

				mapMCIDEVICEID[(MCIDEVICEID)mop->wDeviceID] = 1; // �I�[�v���̎��ɗ^����ꂽ�f�o�C�XID��ێ�
				OutputDebugStream("��MCI-OPEN-CD��");
				OutputDebugStream("%s", mop->lpstrAlias);
				; // CD���I�[�v�����ꂽ!!�B
			}
		}
	}

	// DVD�^�C�v
	if ( TSModCommand::Environment::cmdBGMMode == 3 ) {
		Play_NotCD_ButFromFile(IDDevice, uMsg, fdwCommand, dwParam );
	}


	if ( TSModCommand::Environment::cmdBGMMode == 0 ) {

		// �g���b�N���𓾂�
		// �Đ����Ƀp�����[�^���󂯎��
		if ( uMsg == MCI_PLAY ) {
			// �v���C�̎��̃p�����[�^
			MCI_PLAY_PARMS *mpp = (MCI_PLAY_PARMS *)dwParam;
			if ( mpp ) {
				OutputDebugStream("�J�n%d - �I��%d\n", (int)mpp->dwFrom, (int)mpp->dwTo ); // ����From�����y�Đ��ԍ����̂��̂ł���B(TSmod.txt��_�p�b�`�̔ԍ��ƈ�v����)

				if ( mpp->dwFrom > 0 && mpp->dwTo > 0 ) {
					// �g���b�N���𓾂�
					MCI_SET_PARMS set;
					MCI_STATUS_PARMS status;

					set.dwTimeFormat = MCI_FORMAT_MSF;
					p_mciSendCommandA(IDDevice, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID)&set);

					status.dwItem = MCI_STATUS_LENGTH;
					status.dwTrack = mpp->dwFrom; // �Đ��g���b�N�ڂ̒����𓾂�ꍇ
					p_mciSendCommandA(IDDevice, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD)(LPVOID)&status);

					/// �Ɠ��ȃt�H�[�}�b�g�Ȃ̂ŁA�b�ɕϊ�
					int nTime = MCI_MSF_MINUTE(status.dwReturn) * 60 + MCI_MSF_SECOND(status.dwReturn);

					if ( (mpp->dwTo - mpp->dwFrom) == 1) { // �g���b�N�������Ƃ��P�w�肵�Ă���B
						iLastBGMLengthSecond = nTime; // ���߂ōĐ������Ȃ̕b���Ƃ��ĕۑ�

						OutputDebugStream("�����b%d", nTime);
					} else {
						iLastBGMLengthSecond = 0;
					}
				}
			}
		}

		// �X�g�b�v��V�[�N�n�̂ǂꂩ�������B�Đ��Ɋւ���d�v�Ȗ��߂Ƃ��Ď󂯓����
		if ( uMsg == MCI_SET || uMsg == MCI_PAUSE || uMsg == MCI_STOP || uMsg == MCI_SEEK ) {
			isLastPlayUMsg = FALSE;
			IsShouldBeRePlayLastestBGM = FALSE;

			// �^�C�}�[�J�E���g�I��
			preTimeGetTime = p_timeGetTime();

		// �Đ��������B�Đ��R�}���h�̍X�V
		} else if ( uMsg == MCI_PLAY && iLastBGMLengthSecond ) {
			IsShouldBeRePlayLastestBGM = FALSE;

			// �Ō�ɍĐ�������^�Ƃ���B
			isLastPlayUMsg = TRUE;

			// �^�C�}�[�J�E���g�I��
			preTimeGetTime = p_timeGetTime();

		// �Đ��Ɋ֌W����d�v�ȃR�}���h�ł͂Ȃ��B
		} else {
			// �^�C�}�[�J�E���g���s

			// �Ō�̗L���R�}���h�́u�Đ��v�̂܂܁B
			if ( isLastPlayUMsg && iLastBGMLengthSecond > 0 ) {
				double iSubSecond = (p_timeGetTime() - preTimeGetTime);
				iSubSecond = iSubSecond / 1000.0f; // �~���b��b�ɒ����B

				// �Ō�ɍĐ������Ȃ��I����Ă���A1�b�o�߂������ۂ��B
				if ( iSubSecond > iLastBGMLengthSecond + 1 ) {
					OutputDebugStream("�Ō�̋Ȃ��Đ����Ă���1�b�o����!!");
					IsShouldBeRePlayLastestBGM = TRUE;
				}
			}
		}
	}

	return mirror;
}



MCIERROR WINAPI Play_NotCD_ButFromFile(MCIDEVICEID& IDDevice, UINT& uMsg, DWORD& fdwCommand, DWORD& dwParam ) {

	//------------------------------------------------------------------------------------------------------------
	// ��������́ACD�Đ��v�����������ۂɁACD����ł͂Ȃ��A�O���̃t�@�C�����璼�ڍĐ�����p�r!!
	//------------------------------------------------------------------------------------------------------------
	// �Đ����Ƀp�����[�^���󂯎��BCD�I�[�v���p�Ƃ��ėv������Ă��f�o�C�XID
	if ( uMsg == MCI_PLAY && mapMCIDEVICEID[IDDevice] ) {

		if ( open2.wDeviceID ) {
			p_mciSendCommandA( open2.wDeviceID, MCI_STOP, 0, 0 );
			p_mciSendCommandA( open2.wDeviceID, MCI_CLOSE, 0, 0 );
		}

		// �v���C�̎��̃p�����[�^
		MCI_PLAY_PARMS *mpp = (MCI_PLAY_PARMS *)dwParam;
		if ( mpp ) {
			int iTargetTrackNo = (int)mpp->dwFrom;

			static char szFilePath[MAX_PATH] = "";
			wsprintf( szFilePath, "bdat\\%02d.dat", iTargetTrackNo-1 );

			// �����
			struct stat  st;
			// �t�@�C�������݂����
			if (stat(szFilePath, &st) == 0 ) {

				OutputDebugStream("��MCI-OPEN-CD��");

				//mp3�Đ��̏ꍇ
				open2.lpstrDeviceType = "MPEGVideo";
				open2.lpstrElementName = szFilePath;
				int result = p_mciSendCommandA( 0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, ( DWORD_PTR )&open2 );

				//�G���[�Ȃ�O�ȊO���Ԃ�
				if( result ) {
					//�G���[�擾
					// p_mciGetErrorString( result, buf, sizeof( buf ) );
					// return;
				}
		        
				// play2.dwCallback = ( DWORD )hwnd;
				result = p_mciSendCommandA( open2.wDeviceID, MCI_PLAY, MCI_NOTIFY, ( DWORD_PTR )&play2 );
				//�G���[�Ȃ�O�ȊO���Ԃ�
				if( result ) {
					//�G���[�擾
					// p_mciGetErrorString( result, buf, sizeof( buf ) );
					// return;
				}
			} else {
				OutputDebugString("cmd['environment_bgm_mode'] = 3\n��DVD�p�̃I�v�V�����ƂȂ�܂��B\n�t�@�C���\���𖞂����Ă��܂���B");
			}

		}
	}

	if ( uMsg == MCI_STATUS && mapMCIDEVICEID[IDDevice] ) {
		if ( open2.wDeviceID ) {
			// �O���t�@�C���i01.dat�Ȃ�)�̕��Ŗ炵�Ă���󋵂𓾂�
			MCI_STATUS_PARMS mciStatus;
			
			mciStatus.dwItem = MCI_STATUS_MODE;
			p_mciSendCommandA(open2.wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)&mciStatus);

			// ���̏󋵂�CD�̃X�e�[�^�X�Ƃ��ċU��
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
	// ���܂ł��ACD�Đ��v�����������ۂɁACD����ł͂Ȃ��A�O���̃t�@�C�����璼�ڍĐ�����p�r!!
	//------------------------------------------------------------------------------------------------------------

	return NULL;
}



// �Ō�̋Ȃ�������x�Đ�����ׂ����H TSMod����Q�ƁB
__declspec(dllexport) int BGMmm_GetShouldBeRePlayLastestBGM() {
	// CD���[�h��Vista�ȍ~�̎������A���[�v���Ȃ��Ƃ����G���[������B
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