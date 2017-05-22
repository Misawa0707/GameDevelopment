/// <summary>
/// JoyPad	�R���g���[���[�������N���X
/// </summary>
#pragma once
//DriectInput�̃o�[�W�����w��
#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>
#include <vector>

//�����L�[�̃t���O��`  16�i��
#define PAD_DIR_UP		0x01	//��
#define PAD_DIR_LEFT	0x02	//��
#define PAD_DIR_DOWN	0x04	//��
#define PAD_DIR_RIGHT	0x08	//�E
#define PAD_DIR_NONE	0x10	//���������ĂȂ�


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
	//������
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
	//�X�V
	void Update();
private:
	//�W���C�p�b�h�P���̏��
	struct JoyPadSet{

		JoyPadSet() {
			inputDevice = NULL;
		 }

		//�f�o�C�X
		LPDIRECTINPUTDEVICE8 inputDevice;
		//���͏��
		DIJOYSTATE2 joypad;
		//�O��̓��͏��
		DIJOYSTATE2 joypadOld;


	};
	//�C���X�^���X
	LPDIRECTINPUT8 m_pDInput;
	//�W���C�p�b�h�z��
	std::vector<JoyPadSet> m_joyPadSet;

};
