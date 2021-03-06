//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include <sstream>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <CommonStates.h>
#include "ADX2Le.h"
#include "Rsources\Music\Basic.h"

extern void ExitGame();

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() :
    m_window(0),
    m_outputWidth(800),
    m_outputHeight(600),
    m_featureLevel(D3D_FEATURE_LEVEL_9_1)
{
}

Game::~Game()
{
	//サウンドライブラリの終了処理
	ADX2Le::Finalize();
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_window = window;
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateDevice();

    CreateResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

	m_spriteBatch = std::make_unique<SpriteBatch>(m_d3dContext.Get());
	m_spriteFont = std::make_unique<SpriteFont>(m_d3dDevice.Get(),
		L"Rsources/myfile.spritefont");

	m_count = 0;
	//rテクスチャ
	/*ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(m_d3dDevice.Get(), L"cat.png",
			resource.GetAddressOf(),
			m_texture.ReleaseAndGetAddressOf()));
	*/
	//ddsを使ったテクスチャ
	ComPtr<ID3D11Resource> resource;
	DX::ThrowIfFailed(
		CreateDDSTextureFromFile(m_d3dDevice.Get(), L"Rsources/cat.dds",
			resource.GetAddressOf(),
			m_texture.ReleaseAndGetAddressOf()));

	//猫のテクスチャ
	ComPtr<ID3D11Texture2D> cat;
	DX::ThrowIfFailed(resource.As(&cat));
	//テクスチャの情報
	CD3D11_TEXTURE2D_DESC catDesc;
	cat->GetDesc(&catDesc);
	//テクスチャの原点を中心にする
	m_origin.x = float(catDesc.Width / 2);
	m_origin.y = float(catDesc.Height / 2);
	//表示座標の調整
	m_screenPos.x = m_outputHeight / 2.f;
	m_screenPos.y = m_outputWidth / 2.f;
	//キーボードのオブジェクト生成
	m_keyboard = std::make_unique<Keyboard>();
	//マウスのオブジェクト生成
	m_mouse = std::make_unique<Mouse>();
	//windowハンドラを通知
	m_mouse->SetWindow(window);

	//ACFファイルの読み込み
	ADX2Le::Initialize("Rsources/Music/ADX2_samples.acf");
	//ACBとAWBを読み込む
	ADX2Le::LoadAcb("Rsources/Music/Basic.acb", "Rsources/Music/Basic.awb");
	//曲を流す
	ADX2Le::Play(CRI_BASIC_MUSIC1);

	//ゲームパットのオブジェクト生成
	m_gamePad = std::make_unique<GamePad>();

	m_pJoyPad = std::make_unique<JoyPad>();
	m_pJoyPad->Initialize(window);

}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//サウンドライブラリの舞フレーム更新
	ADX2Le::Update();

	m_pJoyPad->Update();

	float elapsedTime = float(timer.GetElapsedSeconds());

	// TODO: Add your game logic here.
	elapsedTime;
	//カウンタを進める
	m_count++;
	//文字列に代入
	//m_str = L"asdfghjk";
	std::wstringstream ss;
	//ストリングストリームに出力
//	ss << L"aiueo" << m_count;
	//ストリングストリームから文字列の取得
	m_str = ss.str();

	Keyboard::State kb = m_keyboard->GetState();
	//キーボードｖトラッカーの更新
	m_keyboardtracker.Update(kb);

	if (kb.Back)
	{	
		// Backspace key is down
		m_str = L"BackSpace!";
	}
	//押したとき
	if (m_keyboardtracker.pressed.Space)
	{
		m_str = L"Space!";
	}

		// Space was just pressed down
		//離した時
		if (m_keyboardtracker.IsKeyReleased(Keyboard::Keys::A))
		{
			m_str = L"aaaaaaaaaaaa";
		}
	
			// F1 key was just released
		//マウスの状態を取得
		Mouse::State state = m_mouse->GetState();
		m_tracker.Update(state);

		if (m_tracker.rightButton == Mouse::ButtonStateTracker::HELD)
		{
			//m_str = L"HelloWrold";
		}
		// Left button is down
		//マウスの座標
		XMFLOAT2 mousePosInPixels(float(state.x), float(state.y));
		//テクスチャの座標をマウスの位置にする
		m_screenPos = mousePosInPixels;

		//パスの切り替え
		if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::PRESSED)
		{
			m_mouse->SetMode(Mouse::MODE_RELATIVE);
		}
		else if (m_tracker.leftButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
		{
			m_mouse->SetMode(Mouse::MODE_ABSOLUTE);
		}
		//Xboxのコントローラー
		//ゲームパットの状態取得
		/*DirectX::GamePad::State padstate = m_gamePad->GetState(0, GamePad::DEAD_ZONE_CIRCULAR);
		m_gamepadtracker.Update(padstate);*/
		//接続確認
		//if (padstate.IsConnected())
		//{
		//	//Aボタンが押されているか
		//	if (padstate.IsAPressed())
		//	{
		//		//今押されている
		//		
		//	}
		//	if (padstate.IsBPressed())
		//	{
		//		//今押されている
		//		m_str = L"sssssss";

		//	}
		//	if (padstate.IsDPadDownPressed())
		//	{
		//		//今押されている
		//	}
		//	//左スティック左右(0~1)
		//	float posx = padstate.thumbSticks.leftX;
		//	//左スティック上下(0~1)
		//	float posy = padstate.thumbSticks.leftY;
		//	//右トリガーがどれだけ押されたか(0~1)
		//	float throttle = padstate.triggers.right;

		//	if (padstate.IsViewPressed())
		//	{
		//		

		//		// This is an alias for the Xbox 360 'Back' button
		//		// which is called 'View' on the Xbox One controller
		//	}
		//	//デバイスの能力
		//	//GamePad::Capabilities caps = gamePad->GetCapabilities(0);
		//
		//		
		//	if (m_gamepadtracker.back ==GamePad::ButtonStateTracker::PRESSED)
		//	{
		//		
		//	}
		//	else if (m_gamepadtracker.back == GamePad::ButtonStateTracker::PRESSED)
		//	{
		//		m_gamepadtracker.Reset();
		//		
		//	}
		//}
		
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    // TODO: Add your rendering code here.
	CommonStates states(m_d3dDevice.Get());
	//ddsを使わないでBeing()の中で処理することで見栄えが良くなる
	m_spriteBatch->Begin(SpriteSortMode_Deferred,states.NonPremultiplied());

	//テクスチャの切り取り
	RECT rect;
	//各場所の範囲指定
	rect.left   = 30;
	rect.right  = 30;
	rect.top    = 30;
	rect.bottom = 30;

	//スプライトの描画
	m_spriteBatch->Draw(m_texture.Get(),
		m_screenPos,			   //表示位置
		nullptr,				   //画像の切り取りなど(nullptrをrectに変えると指定した大きさになる)
		Colors::White,			   //色
		XMConvertToRadians(0.0),  //回転角
		m_origin
		);
	//XMConvertToRadians(90,0))ラジアンにする処理の例
	//文字の描画
	m_spriteFont->DrawString(m_spriteBatch.get(),
		L"Hello, world!",  //出す文字
		XMFLOAT2(100, 100),//表示位置
		Colors::RoyalBlue, //色
		m_count/10.0f	   //回す

		);
	m_spriteFont->DrawString(m_spriteBatch.get(),
		m_str.c_str(),	  //ストリングストリームからの描画
		XMFLOAT2(250, 400)//表示位置
	);
	
	m_spriteBatch->End();

    Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views.
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::CornflowerBlue);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

    // Set the viewport.
    CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(m_outputWidth), static_cast<float>(m_outputHeight));
    m_d3dContext->RSSetViewports(1, &viewport);
}

// Presents the back buffer contents to the screen.
void Game::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was reset we must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        OnDeviceLost();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    m_outputWidth = std::max(width, 1);
    m_outputHeight = std::max(height, 1);

    CreateResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}

// These are the resources that depend on the device.
void Game::CreateDevice()
{
    UINT creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    static const D3D_FEATURE_LEVEL featureLevels [] =
    {
        // TODO: Modify for supported Direct3D feature levels (see code below related to 11.1 fallback handling).
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    // Create the DX11 API device object, and get a corresponding context.
    HRESULT hr = D3D11CreateDevice(
        nullptr,                                // specify nullptr to use the default adapter
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        featureLevels,
        _countof(featureLevels),
        D3D11_SDK_VERSION,
        m_d3dDevice.ReleaseAndGetAddressOf(),   // returns the Direct3D device created
        &m_featureLevel,                        // returns feature level of device created
        m_d3dContext.ReleaseAndGetAddressOf()   // returns the device immediate context
        );

    if (hr == E_INVALIDARG)
    {
        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it.
        hr = D3D11CreateDevice(nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            creationFlags,
            &featureLevels[1],
            _countof(featureLevels) - 1,
            D3D11_SDK_VERSION,
            m_d3dDevice.ReleaseAndGetAddressOf(),
            &m_featureLevel,
            m_d3dContext.ReleaseAndGetAddressOf()
            );
    }

    DX::ThrowIfFailed(hr);

#ifndef NDEBUG
    ComPtr<ID3D11Debug> d3dDebug;
    if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
    {
        ComPtr<ID3D11InfoQueue> d3dInfoQueue;
        if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
        {
#ifdef _DEBUG
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
            d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif
            D3D11_MESSAGE_ID hide [] =
            {
                D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
                // TODO: Add more message IDs here as needed.
            };
            D3D11_INFO_QUEUE_FILTER filter = {};
            filter.DenyList.NumIDs = _countof(hide);
            filter.DenyList.pIDList = hide;
            d3dInfoQueue->AddStorageFilterEntries(&filter);
        }
    }
#endif

    // DirectX 11.1 if present
    if (SUCCEEDED(m_d3dDevice.As(&m_d3dDevice1)))
        (void)m_d3dContext.As(&m_d3dContext1);

    // TODO: Initialize device dependent objects here (independent of window size).
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateResources()
{
    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews [] = { nullptr };
    m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_d3dContext->Flush();

    UINT backBufferWidth = static_cast<UINT>(m_outputWidth);
    UINT backBufferHeight = static_cast<UINT>(m_outputHeight);
    DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM;
    DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    UINT backBufferCount = 2;

    // If the swap chain already exists, resize it, otherwise create one.
    if (m_swapChain)
    {
        HRESULT hr = m_swapChain->ResizeBuffers(backBufferCount, backBufferWidth, backBufferHeight, backBufferFormat, 0);

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            OnDeviceLost();

            // Everything is set up now. Do not continue execution of this method. OnDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            DX::ThrowIfFailed(hr);
        }
    }
    else
    {
        // First, retrieve the underlying DXGI Device from the D3D Device.
        ComPtr<IDXGIDevice1> dxgiDevice;
        DX::ThrowIfFailed(m_d3dDevice.As(&dxgiDevice));

        // Identify the physical adapter (GPU or card) this device is running on.
        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf()));

        // And obtain the factory object that created it.
        ComPtr<IDXGIFactory1> dxgiFactory;
        DX::ThrowIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf())));

        ComPtr<IDXGIFactory2> dxgiFactory2;
        if (SUCCEEDED(dxgiFactory.As(&dxgiFactory2)))
        {
            // DirectX 11.1 or later

            // Create a descriptor for the swap chain.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
            swapChainDesc.Width = backBufferWidth;
            swapChainDesc.Height = backBufferHeight;
            swapChainDesc.Format = backBufferFormat;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = backBufferCount;

            DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = { 0 };
            fsSwapChainDesc.Windowed = TRUE;

            // Create a SwapChain from a Win32 window.
            DX::ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(
                m_d3dDevice.Get(),
                m_window,
                &swapChainDesc,
                &fsSwapChainDesc,
                nullptr,
                m_swapChain1.ReleaseAndGetAddressOf()
                ));

            DX::ThrowIfFailed(m_swapChain1.As(&m_swapChain));
        }
        else
        {
            DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
            swapChainDesc.BufferCount = backBufferCount;
            swapChainDesc.BufferDesc.Width = backBufferWidth;
            swapChainDesc.BufferDesc.Height = backBufferHeight;
            swapChainDesc.BufferDesc.Format = backBufferFormat;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.OutputWindow = m_window;
            swapChainDesc.SampleDesc.Count = 1;
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.Windowed = TRUE;

            DX::ThrowIfFailed(dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &swapChainDesc, m_swapChain.ReleaseAndGetAddressOf()));
        }

        // This template does not support exclusive fullscreen mode and prevents DXGI from responding to the ALT+ENTER shortcut.
        DX::ThrowIfFailed(dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER));
    }

    // Obtain the backbuffer for this window which will be the final 3D rendertarget.
    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(m_swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));

    // Create a view interface on the rendertarget to use on bind.
    DX::ThrowIfFailed(m_d3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.ReleaseAndGetAddressOf()));

    // Allocate a 2-D surface as the depth/stencil buffer and
    // create a DepthStencil view on this surface to use on bind.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(depthBufferFormat, backBufferWidth, backBufferHeight, 1, 1, D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(m_d3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, depthStencil.GetAddressOf()));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    DX::ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.

    m_depthStencilView.Reset();
    m_renderTargetView.Reset();
    m_swapChain1.Reset();
    m_swapChain.Reset();
    m_d3dContext1.Reset();
    m_d3dContext.Reset();
    m_d3dDevice1.Reset();
    m_d3dDevice.Reset();

    CreateDevice();

    CreateResources();
}