#pragma once


// TSMod.txtの特殊設定変更コマンド指定
namespace TSModCommand {

	namespace Environment {

		// ＢＧＭのモード
		extern int cmdBGMMode;
	};

	// コマンドをセットする
	void setCommandSwitch(char *szCurCmdTokenBuf, int byteCurCmdLv);

};


