#include "Common.hpp"

namespace GUI
{
    std::unique_ptr<CGUI> m_pGUI = nullptr;

    CGUI::CGUI( HWND hwnd ) : m_hwnd( hwnd )
    {
        InitializeD3D( hwnd );

        if ( !m_pRenderTarget )
            throw std::exception( "Render Target not initialized!" );

        m_pRenderer = std::make_unique<CRenderer>(
            m_pDevice,
            m_pContext,
            m_pRenderTarget.Get( )
        );
    }
    CGUI::~CGUI( )
    {
        CleanupD3D( );
    }

    void CGUI::InitializeD3D( HWND hwnd )
    {
        DXGI_SWAP_CHAIN_DESC scd;
        ZeroMemory( &scd, sizeof( scd ) );

        scd.BufferCount = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow = hwnd;
        scd.SampleDesc.Count = 1;
        scd.Windowed = TRUE;
        scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_FEATURE_LEVEL featureLevel;
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            createDeviceFlags,  
            NULL,
            0,
            D3D11_SDK_VERSION,
            &scd,
            m_pSwapChain.GetAddressOf( ),
            m_pDevice.GetAddressOf( ),
            &featureLevel,
            m_pContext.GetAddressOf( ) );

        if ( FAILED( hr ) )
            throw std::exception( "Failed to create D3D device and swap chain" );

        if ( SUCCEEDED( hr ) && m_pContext )
        {
            Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
            hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( void** ) pBackBuffer.GetAddressOf( ) );
            if ( SUCCEEDED( hr ) )
            {
                hr = m_pDevice->CreateRenderTargetView( pBackBuffer.Get( ), NULL, m_pRenderTarget.GetAddressOf( ) );
                if ( SUCCEEDED( hr ) )
                {
                    m_pContext->OMSetRenderTargets( 1, m_pRenderTarget.GetAddressOf( ), NULL );

                    D3D11_VIEWPORT viewport;
                    ZeroMemory( &viewport, sizeof( viewport ) );
                    viewport.Width = WIDTH_SCREEN;  
                    viewport.Height = HEIGHT_SCREEN;
                    viewport.MinDepth = 0.0f;
                    viewport.MaxDepth = 1.0f;
                    viewport.TopLeftX = 0.0f;
                    viewport.TopLeftY = 0.0f;
                    m_pContext->RSSetViewports( 1, &viewport );
                }
            }
        }

        if ( FAILED( hr ) )
            throw std::exception( "Failed to initialize Direct3D" );
    }

    void CGUI::Render( )
    {
        if ( !m_pRenderTarget ) return;

        m_pContext->ClearRenderTargetView( m_pRenderTarget.Get( ), m_clearColor );

        //if ( m_pRenderer )
        //{
        //    float red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
        //    m_pRenderer->DrawRectFilled(200.0f, 300.0f, 200.0f, 150.0f, red); // X, Y, Width, Height

        //    float blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
        //    m_pRenderer->DrawTriangle(
        //        200.0f, 300.0f,   // A
        //        300.0f, 200.0f,   // C
        //        400.0f, 300.0f,   // B
        //        blue
        //    );

        //    float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
        //    m_pRenderer->DrawRectFilled(280.0f, 370.0f, 40.0f, 80.0f, green); 

        //    m_pRenderer->DrawRectFilled(230.0f, 350.0f, 50.0f, 50.0f, blue);  
        //    m_pRenderer->DrawRectFilled(330.0f, 350.0f, 50.0f, 50.0f, blue);  
        //    m_pRenderer->DrawRect(230.0f, 350.0f, 50.0f, 50.0f, red);         
        //    m_pRenderer->DrawRect(330.0f, 350.0f, 50.0f, 50.0f, red);

        //    float white[ 4 ] = { 1.0f, 1.0f, 1.0f, 1.0f };
        //    m_pRenderer->DrawText( L"Hello World", 250.0f, 320.0f, 200.0f, 50.0f, white );
        //}

        if (m_pFramework)
            m_pFramework->Render();

        m_pSwapChain->Present( 1, 0 );
    }

    void CGUI::OnResize( UINT width, UINT height )
    {
        if ( m_pSwapChain && width > 0 && height > 0 )
        {
            m_pContext->OMSetRenderTargets( 0, nullptr, nullptr );
            m_pRenderTarget.Reset( );

            HRESULT hr = m_pSwapChain->ResizeBuffers(
                0,
                width,
                height,
                DXGI_FORMAT_UNKNOWN,
                0
            );

            if ( SUCCEEDED( hr ) )
            {
                Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
                hr = m_pSwapChain->GetBuffer(
                    0,
                    __uuidof( ID3D11Texture2D ),
                    ( void** ) pBackBuffer.GetAddressOf( )
                );

                if ( SUCCEEDED( hr ) )
                {
                    hr = m_pDevice->CreateRenderTargetView(
                        pBackBuffer.Get( ),
                        nullptr,
                        m_pRenderTarget.GetAddressOf( )
                    );

                    if ( SUCCEEDED( hr ) && m_pRenderer )
                    {
                        m_pContext->OMSetRenderTargets(
                            1,
                            m_pRenderTarget.GetAddressOf( ),
                            nullptr
                        );

                        D3D11_VIEWPORT viewport = {
                            0.0f, 0.0f,
                            static_cast<float>(width),
                            static_cast<float>(height),
                            0.0f, 1.0f
                        };
                        m_pContext->RSSetViewports(1, &viewport);

                        m_pRenderer->OnDeviceRestored( m_pRenderTarget.Get( ) );
                    }
                }
            }
        }
    }

    void CGUI::CleanupD3D( )
    {
        if ( m_pSwapChain ) m_pSwapChain->SetFullscreenState( FALSE, NULL );
        m_pRenderTarget.Reset( );
        m_pSwapChain.Reset( );
        m_pContext.Reset( );
        m_pDevice.Reset( );
    }
}