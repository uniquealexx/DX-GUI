#include "Common.hpp"

namespace GUI
{
    std::unique_ptr<CGUI> m_pGUI = nullptr;

    CGUI::CGUI( HWND hwnd ) : m_hwnd( hwnd )
    {
        InitializeD3D( hwnd );
        m_pRenderer = std::make_unique<CRenderer>( m_pDevice, m_pContext );
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

        D3D_FEATURE_LEVEL featureLevel;
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            0,
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
            // Создаем render target view
            Microsoft::WRL::ComPtr<ID3D11Texture2D> pBackBuffer;
            hr = m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( void** ) pBackBuffer.GetAddressOf( ) );
            if ( SUCCEEDED( hr ) )
            {
                hr = m_pDevice->CreateRenderTargetView( pBackBuffer.Get( ), NULL, m_pRenderTarget.GetAddressOf( ) );
                if ( SUCCEEDED( hr ) )
                {
                    m_pContext->OMSetRenderTargets( 1, m_pRenderTarget.GetAddressOf( ), NULL );

                    // Настройка viewport
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

        if ( m_pRenderer )
        {
            float red[ 4 ] = { 0.0f, 1.0f, 0.0f, 1.0f };
            m_pRenderer->DrawRectFilled( 300.0f, 200.0f, 150.0f, 150.0f, red );
        }

        m_pSwapChain->Present( 1, 0 );
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