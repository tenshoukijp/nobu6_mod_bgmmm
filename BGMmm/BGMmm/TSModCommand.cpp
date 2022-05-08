#include "TSModCommand.h"

#include "string.h"
#include <windows.h>

int TSModCommand::Environment::cmdBGMMode								= (int)0;

// TSMod.txt��:cmd:�n�̃X�C�b�`�𔽉f������B
void TSModCommand::setCommandSwitch(char *szCurCmdTokenBuf, int byteCurCmdLv) {

	// LV �� MIN, MAX �I�[�o�[�t���[����
	if (byteCurCmdLv < 0 ) { byteCurCmdLv = 0; }
	if (byteCurCmdLv > 5 ) { byteCurCmdLv = 5; }


	// �a�f�l���[�h
	if (strcmp(szCurCmdTokenBuf, "environment_bgm_mode")==0 ) {
		TSModCommand::Environment::cmdBGMMode = byteCurCmdLv;
	}
}
