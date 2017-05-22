/// <summary>
/// JoyPad	コントローラーを扱うクラス
/// </summary>
#pragma once
//DriectInputのバージョン指定
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <vector>

//方向キーのフラグ定義  16進数
#define PAD_DIR_UP		0x01	//上
#define PAD_DIR_LEFT	0x02	//左
#define PAD_DIR_DOWN	0x04	//下
#define PAD_DIR_RIGHT	0x08	//右
#define PAD_DIR_NONE	0x10	//何も押してない


template<typename T>
void SafeRelease(T*& p)
{
	if (p) {
		p->Release();
		p = NULL;
	}
}

class JoyPad
{
public:
	JoyPad();

	~JoyPad();
	//初期化
	bool Initialize(HWND window);
	bool buttonPush(int padId, int buttonId);
	bool buttonTrigger(int padId, int buttonId);
	bool buttonRelease(int padId, int buttonId);
	int buttonPushTime(int padId, int buttonId);
	LONG getStickLX(int padId);
	LONG getStickLY(int padId);
	LONG getStickLZ(int padId);
	LONG getStickLRX(int padId);
	LONG getStickLRY(int padId);
	LONG getStickLRZ(int padId);
	BYTE getDirectionKey(int padId);
	//更新
	void Update();
private:
	//ジョイパッド１個分の情報
	struct JoyPadSet{

		JoyPadSet() {
			inputDevice = NULL;
		 }

		//デバイス
		LPDIRECTINPUTDEVICE8 inputDevice;
		//入力情報
		DIJOYSTATE2 joypad;
		//前回の入力情報
		DIJOYSTATE2 joypadOld;


	};
	//インスタンス
	LPDIRECTINPUT8 m_pDInput;
	//ジョイパッド配列
	std::vector<JoyPadSet> m_joyPadSet;

};

