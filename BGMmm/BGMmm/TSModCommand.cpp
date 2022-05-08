#include "TSModCommand.h"

#include "string.h"
#include <windows.h>

int TSModCommand::Environment::cmdBGMMode								= (int)0;

// TSMod.txtの:cmd:系のスイッチを反映させる。
void TSModCommand::setCommandSwitch(char *szCurCmdTokenBuf, int byteCurCmdLv) {

	// LV の MIN, MAX オーバーフロー処理
	if (byteCurCmdLv < 0 ) { byteCurCmdLv = 0; }
	if (byteCurCmdLv > 5 ) { byteCurCmdLv = 5; }


	// ＢＧＭモード
	if (strcmp(szCurCmdTokenBuf, "environment_bgm_mode")==0 ) {
		TSModCommand::Environment::cmdBGMMode = byteCurCmdLv;
	}
}
