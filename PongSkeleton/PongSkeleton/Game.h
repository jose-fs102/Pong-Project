#pragma once

#include <Windows.h> // Include the Win32 API
#include <tchar.h> // Include support for UNICODE
#include <ctime>
#include <sstream> // Include wostringstream support
using std::wostringstream;

#include <d2d1.h> // Include the Direct2D API.
#pragma comment(lib, "d2d1.lib") // Connect to the Direct2D Import Library.

#include <dwrite.h> // Include the DirectWrite API.
#pragma comment(lib, "dwrite.lib") // Connect to the DirectWrite Import Library.

#include <mmsystem.h> // Include the Multimedia header file.
#pragma comment(lib, "winmm.lib") // Connect to the Multimedia import library.

// WIC
#include <wincodec.h>
#pragma comment(lib, "windowscodecs.lib")

// XAudio2
#include <xaudio2.h>
#pragma comment(lib, "Xaudio2.lib")
#include "audiodata.h"

#include <thread>
#include <iostream>

#define NUM_PARTICLES 1000

class Game
{
	// Window Handle:
	HWND hWnd;

	// DirectX Helper Functions

	// The SafeRelease Pattern
	template <class T> void SafeRelease(T **ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = NULL;
		}
	}

	// Programmer defined macro to make using the known colors easier.
	#define D2DColor(clr) D2D1::ColorF(D2D1::ColorF::clr)

	HRESULT CreateGraphics(HWND hWnd);
	void DestroyGraphics(void);

	// DirectX Interfaces:
	ID2D1Factory* pD2DFactory;
	ID2D1HwndRenderTarget* pRT;
	ID2D1SolidColorBrush *pBrush;

	D2D1_COLOR_F custom;
	D2D1_POINT_2F ptmouse;

	// DirectWrite Interfaces:
	IDWriteFactory* pDWFactory;
	IDWriteTextFormat* pTF;
	IDWriteTextFormat* newFormat;
	
	// WIC Interfaces
	IWICImagingFactory* pWICFactory;
	ID2D1Bitmap* background;
	ID2D1Bitmap* sprite;
	ID2D1Bitmap* particle;
	ID2D1Bitmap* particle2;
	ID2D1Bitmap* scoreboard;
	ID2D1Bitmap* pongBall;
	ID2D1Bitmap* paddle1;
	ID2D1Bitmap* paddle2;

	// XAudio2 Interface
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasterVoice;
	IXAudio2SourceVoice* pSourceVoice[10];
	IXAudio2SourceVoice* pSourceVoiceMusic;

	AudioData music;
	AudioData collideFX;

	// Game Components:
	D2D1_RECT_F ballpos;
	D2D1_RECT_F ballpos2;
	D2D1_SIZE_F ballvec;
	D2D1_SIZE_F ballvec2;

	// Creates the Right paddle with its speed
	D2D1_RECT_F paddleposRight;
	D2D1_SIZE_F paddlevecRight;

	// Creates the left paddle with its speed
	D2D1_RECT_F paddleposLeft;
	D2D1_SIZE_F paddlevecLeft;

	// Creates another set of left and right paddles
	D2D1_RECT_F paddleposRight2;
	D2D1_SIZE_F paddlevecRight2;
	D2D1_RECT_F paddleposLeft2;
	D2D1_SIZE_F paddlevecLeft2;

	// Image for scoreboard
	D2D1_RECT_F scoreboardRect;

	// Image for player 1 and player 2 scores
	D2D1_RECT_F p1Score;
	D2D1_RECT_F p2Score;

	// Variables used to calculate frames per second:
	DWORD dwFrames;
	DWORD dwCurrentTime;
	DWORD dwLastUpdateTime;
	DWORD dwElapsedTime;
	std::wstring szFPS;

	// Sprite sheet variables
	float fSpriteHeight;
	float fSpriteWidth;
	float fSpacing;
	int nFrame;
	float fPosX;
	float fSpriteFrameWidth;
	DWORD dwTime;

	// Particle variables
	D2D1_POINT_2F particlePos[NUM_PARTICLES];
	D2D1_SIZE_F particleVel[NUM_PARTICLES];
	float particleLife[NUM_PARTICLES];

	D2D1_POINT_2F particlePos2[NUM_PARTICLES];
	D2D1_SIZE_F particleVel2[NUM_PARTICLES];
	float particleLife2[NUM_PARTICLES];

public:

	BOOL bRunning;			// Is the game running?
	HANDLE hGameThread;		// Handle to the GameMain thread.
	BYTE keys[256];			// Current state of the keyboard.

	std::thread worker;

	Game(void);
	virtual ~Game(void);

	void Startup(void);
	void Shutdown(void);
	void Input(void);
	void Simulate(void);
	void Render(void);

	void SetHwnd(HWND _hWnd) {hWnd = _hWnd;}

	HRESULT LoadBitmapFromFile(LPCTSTR strFileName, ID2D1Bitmap** ppBitmap);
};