#include "Game.h"

// GLOBALS
////////////////////////////////////////
bool left = true;
bool right = false;
bool collideRight = true;
bool collideLeft = false;
bool multPaddlesLeft = false;
bool multPaddlesRight = false;
bool once = true;
bool twice = true;
int player1Score = 0, player2Score = 0;
////////////////////////////////////////

// CONSTRUCTOR // 
Game::Game(void)
{
	bRunning = TRUE;
	hWnd = NULL;
	ZeroMemory(keys, sizeof(keys));

	pD2DFactory = NULL;
	pRT = NULL;
	pBrush = NULL;
	pDWFactory = NULL;
	pTF = NULL;

	// Zero out the frames per second variables:
	dwFrames = 0;
	dwCurrentTime = 0;
	dwLastUpdateTime = 0;
	dwElapsedTime = 0;
	szFPS = _T("");
}

// DESTRUCTOR //
Game::~Game(void)
{

}

// INITIALIZES THE MAIN COMPONENTS //
void Game::Startup(void)
{
	MessageBox(hWnd, L"Press Spacebar to begin\nPlayer 1 Use 'P' & 'L' for up and down movement\nPlayer 2 Use 'Q' & 'A' for up and down movement\n\
								Use 'R' to reset game\nUse 'M' to enter pause menu\
						Use arrow keys 'UP''DOWN' for volume control.", L"Instructions", MB_OK);

	// Initialize DirectX.
	HRESULT hr = CreateGraphics(hWnd);
	if (FAILED(hr))
	{
		return; // Return -1 to abort Window Creation.
	}

	// SET UP BALL AND PADDLES //
	ballpos = D2D1::RectF(375, 280, 425, 330);
	ballvec = D2D1::SizeF(0, 0);

	paddleposRight = D2D1::RectF(pRT->GetSize().width - 30, 100, pRT->GetSize().width - 10, 200);	// Right paddle 1
	paddleposLeft = D2D1::RectF(pRT->GetSize().width - 790, 350, pRT->GetSize().width - 770, 450);	// Left paddle 1
	paddlevecRight = D2D1::SizeF(0, 9);
	paddlevecLeft = D2D1::SizeF(0, 9);

	paddleposRight2 = D2D1::RectF(pRT->GetSize().width - 100, 100, pRT->GetSize().width - 80, 200); // Right paddle 2
	paddleposLeft2 = D2D1::RectF(pRT->GetSize().width - 720, 350, pRT->GetSize().width - 700, 450);	// Left paddle 2
	paddlevecRight2 = D2D1::SizeF(0, 9);
	paddlevecLeft2 = D2D1::SizeF(0, 9);

	/////////////////////////////////////////////////////////////////////////////////////////////////////

	// Scoreboard setup
	scoreboardRect = D2D1::RectF((pRT->GetSize().width / 2) - 100, 0, 500, 75);
	p1Score = D2D1::RectF((pRT->GetSize().width / 2) + 150, 75, 500, 120);
	p2Score = D2D1::RectF((pRT->GetSize().width / 2) - 115, 75, 300, 120);
}

// CONTROLS THE ENDING OF THE PROGRAM //
void Game::Shutdown(void)
{
	//TODO: Unload Bitmaps/Brushes/Pens/Fonts

	//TODO: Destroy Back Buffer

	// Shutdown DirectX.
	DestroyGraphics();
}

// CONTROLS INPUT FROM KEYBOARD //
void Game::Input(void)
{
	// "ESC" key is used to quit the game
	/////////////////////////////////////////////////////////////////////////////////////////////
	if (keys[VK_ESCAPE] & 0x80)
	{
		int msgCatch = MessageBox(hWnd, L"Are you sure you want to quit?", L"Quit?", MB_YESNO);

		if (msgCatch == 6)
			PostMessage(hWnd, WM_CLOSE, 0, 0);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////

	// 'P' key is used to pause the game
	/////////////////////////////////////////////////////////////////////////////////////////////
	if (keys['M'] & 0x80)
	{
		int msgCatch = MessageBox(hWnd, L"Game is now paused\n Resume?", L"Pause", MB_YESNO);

		if (msgCatch == 7)
		{
			msgCatch = MessageBox(hWnd, L"Are you sure you want to quit?", L"Quit?", MB_YESNO);

			if (msgCatch == 6)
				PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////

	// Gameplay key checks

	// Resets the ball and paddles to center of screen
	if (keys['R'] & 0x80)
	{
		ballpos = D2D1::RectF(375, 280, 425, 330);
		ballvec = D2D1::SizeF(0, 0);

		paddleposRight = D2D1::RectF(pRT->GetSize().width - 30, 100, pRT->GetSize().width - 10, 200);
		paddleposLeft = D2D1::RectF(pRT->GetSize().width - 790, 350, pRT->GetSize().width - 770, 450);
		right = true;
	}

	// If 'Space' is pressed, Then the game Begins
	if (keys[VK_SPACE] & 0x80)
	{
		if (left)
		{
			ballvec = D2D1::SizeF(12, -4);
			left = false;
			right = false;
		}
		else if (right)
		{
			ballvec = D2D1::SizeF(-12, 3);
			left = false;
			right = false;
		}
	}
	//////////////////////////////////////////////////////////////

	// Right Paddle "UP" movement
	if (keys['P'] & 0x80)
	{
		if (ballvec.height != 0 && ballvec.width != 0 && paddleposRight.top >= 0)
		{
			paddleposRight.top -= paddlevecRight.height;
			paddleposRight.bottom -= paddlevecRight.height;
		}
	}

	// Controls the second Right Paddle "UP" movement
	if (multPaddlesRight)
	{
		if (keys['I'] & 0x80)
		{
			if (paddleposRight2.top >= 0)
			{
				paddleposRight2.top -= paddlevecRight2.height;
				paddleposRight2.bottom -= paddlevecRight2.height;
			}
		}
	}
	//////////////////////////////////////////////////////////////

	// Right Paddle "DOWN" movement
	if (keys['L'] & 0x80)
	{
		if (ballvec.height != 0 && ballvec.width != 0 && paddleposRight.bottom <= pRT->GetSize().height)
		{
			paddleposRight.top += paddlevecRight.height;
			paddleposRight.bottom += paddlevecRight.height;
		}
	}

	// Controls the second Right Paddle "DOWN" movement
	if (multPaddlesRight)
	{
		if (keys['K'] & 0x80)
		{
			if (paddleposRight2.bottom <= pRT->GetSize().height)
			{
				paddleposRight2.top += paddlevecRight2.height;
				paddleposRight2.bottom += paddlevecRight2.height;
			}
		}
	}
	//////////////////////////////////////////////////////////////

	// Left Paddle "DOWN" movement
	if (keys['A'] & 0x80)
	{
		if (ballvec.height != 0 && ballvec.width != 0 && paddleposLeft.bottom <= pRT->GetSize().height)
		{
			paddleposLeft.top += paddlevecLeft.height;
			paddleposLeft.bottom += paddlevecLeft.height;
		}
	}

	// Controls the second Left Paddle "DOWN" movement
	if (multPaddlesLeft)
	{
		if (keys['D'] & 0x80)
		{
			if (paddleposLeft2.bottom <= pRT->GetSize().height)
			{
				paddleposLeft2.top += paddlevecLeft2.height;
				paddleposLeft2.bottom += paddlevecLeft2.height;
			}
		}
	}
	//////////////////////////////////////////////////////////////

	// Left Paddle "UP" movement
	if (keys['Q'] & 0x80)
	{
		if (ballvec.height != 0 && ballvec.width != 0 && paddleposLeft.top >= 0)
		{
			paddleposLeft.top -= paddlevecLeft.height;
			paddleposLeft.bottom -= paddlevecLeft.height;
		}
	}

	// Controls the second Left Paddle "UP" Movement
	if (multPaddlesLeft)
	{
		if (keys['E'] & 0x80)
		{
			if (paddleposLeft2.top >= 0)
			{
				paddleposLeft2.top -= paddlevecLeft2.height;
				paddleposLeft2.bottom -= paddlevecLeft2.height;
			}
		}
	}

	// Increase volume
	if (keys[VK_UP] & 0x80)
	{
		float vol;
		pMasterVoice->GetVolume(&vol);
		vol += 0.1f;
		if (vol > 2.0f) vol = 2.0f;
		pMasterVoice->SetVolume(vol);
	}

	// Decrease volume
	if (keys[VK_DOWN] & 0x80)
	{
		float vol;
		pMasterVoice->GetVolume(&vol);
		vol -= 0.1f;
		if (vol < 0.0f) vol = 0.0f;
		pMasterVoice->SetVolume(vol);
	}
}

// USED TO SIMULATE THE GAMEPLAY //
void Game::Simulate(void)
{
	ballpos.left += ballvec.width;
	ballpos.top += ballvec.height;
	ballpos.right += ballvec.width;
	ballpos.bottom += ballvec.height;

	// If the ball touches the bottom of the window, it bounces the ball back up
	if (ballpos.bottom >= pRT->GetSize().height)
	{
		ballvec.height = -ballvec.height;
		PlaySound(L"Collide.wav", NULL, SND_FILENAME | SND_ASYNC);
	}

	// If the ball touches the top of the window, it bounces the ball back up
	if (ballpos.bottom <= 50)
	{
		ballvec.height = -ballvec.height;
		PlaySound(L"Collide.wav", NULL, SND_FILENAME | SND_ASYNC);
	}

	// Rectangle to Rectangle Intersection:
	RECT rIntersect;
	RECT rBall = { (LONG)ballpos.left, (LONG)ballpos.top, (LONG)ballpos.right, (LONG)ballpos.bottom };
	RECT rPaddleRight = { (LONG)paddleposRight.left, (LONG)paddleposRight.top, (LONG)paddleposRight.right, (LONG)paddleposRight.bottom };
	RECT rPaddleLeft = { (LONG)paddleposLeft.left, (LONG)paddleposLeft.top, (LONG)paddleposLeft.right, (LONG)paddleposLeft.bottom };
	RECT rPaddleRight2 = { (LONG)paddleposRight2.left, (LONG)paddleposRight2.top, (LONG)paddleposRight2.right, (LONG)paddleposRight2.bottom };
	RECT rPaddleLeft2 = { (LONG)paddleposLeft2.left, (LONG)paddleposLeft2.top, (LONG)paddleposLeft2.right, (LONG)paddleposLeft2.bottom };

	// Checks for collision with the right paddle 1
	if (IntersectRect(&rIntersect, &rBall, &rPaddleRight))
	{
		ballvec.width = -ballvec.width;
		PlaySound(L"Collide.wav", NULL, SND_FILENAME | SND_ASYNC);
		collideRight = true;
		collideLeft = false;
	}

	// Checks for collision with the left paddle 1
	if (IntersectRect(&rIntersect, &rBall, &rPaddleLeft))
	{
		ballvec.width = -ballvec.width;
		PlaySound(L"Collide.wav", NULL, SND_FILENAME | SND_ASYNC);
		collideLeft = true;
		collideRight = false;
	}
	if (multPaddlesRight)
	{
		// Checks for collision with the right paddle 2
		if (IntersectRect(&rIntersect, &rBall, &rPaddleRight2))
		{
			ballvec.width = -ballvec.width;
			PlaySound(L"Collide.wav", NULL, SND_FILENAME | SND_ASYNC);
			collideRight = true;
			collideLeft = false;
		}
	}

	if (multPaddlesLeft)
	{
		// Checks for collision with the left paddle 2
		if (IntersectRect(&rIntersect, &rBall, &rPaddleLeft2))
		{
			ballvec.width = -ballvec.width;
			PlaySound(L"Collide.wav", NULL, SND_FILENAME | SND_ASYNC);
			collideLeft = true;
			collideRight = false;
		}
	}

	// If the ball moves off the screen to the right, it resets the game
	if (ballpos.right > pRT->GetSize().width + 100)
	{
		ballpos = D2D1::RectF(375, 280, 425, 330);
		ballvec = D2D1::SizeF(0, 0);

		paddleposRight = D2D1::RectF(pRT->GetSize().width - 30, 100, pRT->GetSize().width - 10, 200);
		paddleposLeft = D2D1::RectF(pRT->GetSize().width - 790, 350, pRT->GetSize().width - 770, 450);
		right = true;
		player2Score++;
		if (player2Score == 5)
			multPaddlesRight = true;

		if (player2Score == 15)
		{
			int yes = MessageBox(hWnd, L"Player One is the winner!\nPlay Again", L"Winner", MB_YESNO);

			if (yes == 6)
			{
				ballpos = D2D1::RectF(375, 280, 425, 330);
				ballvec = D2D1::SizeF(0, 0);

				paddleposRight = D2D1::RectF(pRT->GetSize().width - 30, 100, pRT->GetSize().width - 10, 200);
				paddleposLeft = D2D1::RectF(pRT->GetSize().width - 790, 350, pRT->GetSize().width - 770, 450);
				player2Score = 0;
			}
			else if (yes == 7)
				PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}

	// If the ball moves off the screen to the left, it resets the game
	if (ballpos.left < -100)
	{
		ballpos = D2D1::RectF(375, 280, 425, 330);
		ballvec = D2D1::SizeF(0, 0);

		paddleposRight = D2D1::RectF(pRT->GetSize().width - 30, 100, pRT->GetSize().width - 10, 200);
		paddleposLeft = D2D1::RectF(pRT->GetSize().width - 790, 350, pRT->GetSize().width - 770, 450);
		left = true;
		player1Score++;
		if (player1Score == 5)
			multPaddlesLeft = true;

		if (player1Score == 15)
		{
			int yes = MessageBox(hWnd, L"Player One is the winner!\nPlay Again", L"Winner", MB_YESNO);

			if (yes == 6)
			{
				ballpos = D2D1::RectF(375, 280, 425, 330);
				ballvec = D2D1::SizeF(0, 0);

				paddleposRight = D2D1::RectF(pRT->GetSize().width - 30, 100, pRT->GetSize().width - 10, 200);
				paddleposLeft = D2D1::RectF(pRT->GetSize().width - 790, 350, pRT->GetSize().width - 770, 450);
				player1Score = 0;
			}
			else if (yes == 7)
				PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}
}

// RENDERS. SHOULD ONLY BE DRAWING TO THE SCREEN //
void Game::Render(void)
{
	std::wostringstream oss1;
	std::wostringstream oss2;

	oss1 << player1Score;
	oss2 << player2Score;

	std::wstring score1 = oss1.str();
	std::wstring score2 = oss2.str();

	pRT->BeginDraw();

	// TODO: Clear Back Buffer
	pRT->Clear(D2DColor(CornflowerBlue));

	// Background
	pRT->DrawBitmap(background, D2D1::RectF(0, 0, pRT->GetSize().width, pRT->GetSize().height));

	// Pong Ball
	pRT->DrawBitmap(pongBall, ballpos);

	// Draws Right Paddle
	pRT->DrawBitmap(paddle1, paddleposRight);

	// Sets the color and fills second Right Paddle
	pBrush->SetColor(D2DColor(Red));
	if (multPaddlesRight)
	{
		pRT->DrawBitmap(paddle1, paddleposRight2);
		if (once)
			MessageBox(hWnd, L"Player 1's second paddle has been enabled", L"Paddle unlocked", MB_OK);
		once = false;
	}

	// Draws Left Paddle
	pRT->DrawBitmap(paddle2, paddleposLeft);

	// Sets the color and fills second Left Paddle
	pBrush->SetColor(D2DColor(Blue));
	if (multPaddlesLeft)
	{
		pRT->DrawBitmap(paddle2, paddleposLeft2);
		if (twice)
			MessageBox(hWnd, L"Player 2's second paddle has been enabled", L"Paddle unlocked", MB_OK);
		twice = false;
	}

	// Draws the scoreboard bitmap
	pRT->DrawBitmap(scoreboard, scoreboardRect);

	// Draws 2 boxes for the scores
	pBrush->SetColor(D2DColor(Yellow));

	// Sets up point counter
	pRT->DrawText(score1.c_str(), score1.length(), newFormat, p1Score, pBrush);
	pRT->DrawText(score2.c_str(), score2.length(), newFormat, p2Score, pBrush);

	if (collideRight)
	{
		if (ballvec.height != 0 && ballvec.width != 0)
		{
			// Render Particles
			for (int i = 0; i < NUM_PARTICLES; i++)
			{
				// FIRE PARTICLE
				// Render the particle (at its position)
				pRT->DrawBitmap(particle2, D2D1::RectF(
					particlePos2[i].x - 25.0f, particlePos2[i].y - 25.0f,
					particlePos2[i].x + 25.0f, particlePos2[i].y + 25.0f),
					particleLife2[i] / 100.0f);

				// Update its position (based on velocity)
				particlePos2[i].x += particleVel2[i].width;
				particlePos2[i].y += particleVel2[i].height;

				// Decrement its life
				particleLife2[i]--;

				// Check if it is dead
				if (particleLife2[i] <= 0)
				{
					// Reset its position and velocity and life
					particlePos2[i] = D2D1::Point2F(ballpos.left, ballpos.top);
					particleVel2[i] = D2D1::SizeF(float(rand() % 9) - 4.0f,
						float(rand() % 3) - 3.0f);
					particleLife2[i] = float(rand() % 50) + 50.0f;
				}
			}
		}
	}

	else if (collideLeft)
	{
		if (ballvec.height != 0 && ballvec.width != 0)
		{
			// Render Particles
			for (int i = 0; i < NUM_PARTICLES; i++)
			{
				// FIRE PARTICLE
				// Render the particle (at its position)
				pRT->DrawBitmap(particle, D2D1::RectF(
					particlePos[i].x - 25.0f, particlePos[i].y - 25.0f,
					particlePos[i].x + 25.0f, particlePos[i].y + 25.0f),
					particleLife[i] / 100.0f);

				// Update its position (based on velocity)
				particlePos[i].x += particleVel[i].width;
				particlePos[i].y += particleVel[i].height;

				// Decrement its life
				particleLife[i]--;

				// Check if it is dead
				if (particleLife[i] <= 0)
				{
					// Reset its position and velocity and life
					particlePos[i] = D2D1::Point2F(ballpos.left, ballpos.top);
					particleVel[i] = D2D1::SizeF(float(rand() % 9) - 4.0f,
						float(rand() % 3) - 3.0f);
					particleLife[i] = float(rand() % 50) + 50.0f;
				}
			}
		}
	}

	//TODO: Present Back Buffer
	HRESULT hr = pRT->EndDraw();
	if (hr == D2DERR_RECREATE_TARGET)
	{
		//TODO: Destroy Direct2D.
		DestroyGraphics();
		//TODO: Recreate Direct2D.
		CreateGraphics(hWnd);
	}
}

// CREATES YOUR GRAPHICS //
HRESULT Game::CreateGraphics(HWND hWnd)
{
	// Initialize the Direct2D Factory.
	HRESULT hr;
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the Direct2D Factory."),
			_T("Direct2D Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Get the dimensions of the client.
	RECT rc;
	GetClientRect(hWnd, &rc);

	// Initialize a Direct2D Size Structure.
	D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

	// Create the Direct2D Render Target.
	hr = pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(hWnd, size), &pRT);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the Direct2D Render Target."),
			_T("Direct2D Error"), MB_OK | MB_ICONERROR);
		return hr;
	}
	pRT->SetDpi(96.0f, 96.0f);


	// Create the Direct2D Solid Color Brush.
	hr = pRT->CreateSolidColorBrush(D2D1::ColorF(0x0), &pBrush);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the Direct2D Solid Color Brush."),
			_T("Direct2D Error"), MB_OK | MB_ICONERROR);
		return hr;
	}


	// Initialize the DirectWrite Factory.
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
		(IUnknown**)&pDWFactory);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the DirectWrite Factory."),
			_T("DirectWrite Error"), MB_OK | MB_ICONERROR);
		return hr;
	}


	// Create the DirectWrite Text Format for "Veranda"
	hr = pDWFactory->CreateTextFormat(_T("Veranda"), NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		24, _T(""), &pTF);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the DirectWrite Text Format."),
			_T("DirectWrite Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create a new DirectWrite Text Format for "Times New Roman"
	hr = pDWFactory->CreateTextFormat(_T("Times New Roman"), NULL,
		DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		24, _T(""), &newFormat);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the DirectWrite Text Format."),
			_T("DirectWrite Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWICFactory));
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create the WIC Interface."),
			_T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Loads the background image
	hr = LoadBitmapFromFile(L"Metropolis.jpg", &background);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to load Background."),
			_T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Loads the particle effects
	hr = LoadBitmapFromFile(L"Akuma_103.png", &particle);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Faild to load Particle Effects."), _T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Loads the particle effects
	hr = LoadBitmapFromFile(L"Akuma_44.png", &particle2);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Faild to load Particle Effects."), _T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Load the ScoreBoard image	
	hr = LoadBitmapFromFile(L"Score.png", &scoreboard);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Faild to load Scoreboard image."), _T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Load the pong ball image
	hr = LoadBitmapFromFile(L"ball.png", &pongBall);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Faild to load Pong Ball image."), _T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Load the Paddle1 image
	hr = LoadBitmapFromFile(L"Paddle1.png", &paddle1);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Faild to load Paddle1 image."), _T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Load the Paddle2 image
	hr = LoadBitmapFromFile(L"Paddle2.png", &paddle2);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Faild to load Paddle2 image."), _T("WIC Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// XAudio2 Interfaces
	hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create XAudio2 Interface."),
			_T("XAudio2 Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Mastering voice
	hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to Create XAudio2 Master Voice."),
			_T("XAudio2 Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Load WAV Files
	hr = music.LoadWave(L"Audio\\Modify.wav");
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to load Modify.wav."),
			_T("XAudio2 Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create music source voice
	hr = pXAudio2->CreateSourceVoice(&pSourceVoiceMusic, (WAVEFORMATEX*)&music.wfx);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to create music source voice."),
			_T("XAudio2 Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	music.buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	hr = pSourceVoiceMusic->SubmitSourceBuffer(&music.buffer);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to submit music source voice."),
			_T("XAudio2 Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	hr = pSourceVoiceMusic->Start(0, XAUDIO2_COMMIT_NOW);
	if (FAILED(hr))
	{
		MessageBox(HWND_DESKTOP, _T("ERROR: Failed to start music source voice."),
			_T("XAudio2 Error"), MB_OK | MB_ICONERROR);
		return hr;
	}

	return S_OK; // Success!
}

// DESTROYS YOUR GRAPHICS //    aqaqaqaqaq
void Game::DestroyGraphics(void)
{
	// Destroy Music Voice
	if (pSourceVoiceMusic)
	{
		pSourceVoiceMusic->DestroyVoice();
		pSourceVoiceMusic = nullptr;
	}

	// Destory Master Voice
	if (pMasterVoice)
	{
		pMasterVoice->DestroyVoice();
		pMasterVoice = nullptr;
	}

	// Release XAudio2 Interfaces
	SafeRelease(&pXAudio2);

	SafeRelease(&pTF);
	SafeRelease(&pDWFactory);
	SafeRelease(&pBrush);
	SafeRelease(&pRT);
	SafeRelease(&newFormat);

	// Release the Direct2D Factory.
	SafeRelease(&pD2DFactory);
}

// USED TO LOAD BITMAPS IN FROM FILES //
HRESULT Game::LoadBitmapFromFile(LPCTSTR strFileName, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr;

	IWICBitmapDecoder *pDecoder = NULL;
	IWICBitmapFrameDecode *pSource = NULL;
	IWICFormatConverter *pConverter = NULL;

	hr = pWICFactory->CreateDecoderFromFilename(strFileName, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pWICFactory->CreateFormatConverter(&pConverter);
	}
	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(
			pSource,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			NULL,
			0.f,
			WICBitmapPaletteTypeMedianCut
			);
	}
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRT->CreateBitmapFromWicBitmap(
			pConverter,
			NULL,
			ppBitmap
			);
	}

	SafeRelease(&pDecoder);
	SafeRelease(&pSource);
	SafeRelease(&pConverter);

	return hr;
}