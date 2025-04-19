#include "Common.hpp"

namespace GUI
{
    std::unique_ptr<CRenderer> m_pRenderer = nullptr;

    CRenderer::CRenderer( Microsoft::WRL::ComPtr<ID3D11Device> device,
                         Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
                         ID3D11RenderTargetView* pRenderTarget )
        : m_pDevice( device ), m_pContext( context ), m_pRenderTarget( pRenderTarget )
    {
        InitializeShaders( );
        InitializeBuffers( );
        InitializeTextRendering( );
    }

    CRenderer::~CRenderer( )
    {}

    void CRenderer::DrawRectFilled( float x, float y, float width, float height, const float color[ 4 ] )
    {
        Vertex vertices[ 4 ] = {
            {{x,         y},          {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Верхний левый
            {{x + width, y},          {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Верхний правый
            {{x,         y + height}, {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Нижний левый
            {{x + width, y + height}, {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}  // Нижний правый
        };

        m_pContext->UpdateSubresource( m_pVertexBuffer.Get( ), 0, nullptr, vertices, 0, 0 );

        m_pContext->VSSetShader( m_pVertexShader.Get( ), nullptr, 0 );
        m_pContext->PSSetShader( m_pPixelShader.Get( ), nullptr, 0 );
        m_pContext->IASetInputLayout( m_pInputLayout.Get( ) );

        UINT stride = sizeof( Vertex );
        UINT offset = 0;
        m_pContext->IASetVertexBuffers( 0, 1, m_pVertexBuffer.GetAddressOf( ), &stride, &offset );

        m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

        m_pContext->Draw( 4, 0 );
    }

    void CRenderer::DrawRect( float x, float y, float width, float height, const float color[ 4 ] )
    {
        Vertex vertices[ 5 ] = {
            {{x,         y},          {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Верхний левый
            {{x + width, y},          {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Верхний правый
            {{x + width, y + height}, {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Нижний правый
            {{x,         y + height}, {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Нижний левый
            {{x,         y},          {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}  // Замыкаем контур
        };

        m_pContext->UpdateSubresource( m_pVertexBuffer.Get( ), 0, nullptr, vertices, 0, 0 );

        m_pContext->VSSetShader( m_pVertexShader.Get( ), nullptr, 0 );
        m_pContext->PSSetShader( m_pPixelShader.Get( ), nullptr, 0 );
        m_pContext->IASetInputLayout( m_pInputLayout.Get( ) );

        UINT stride = sizeof( Vertex );
        UINT offset = 0;
        m_pContext->IASetVertexBuffers( 0, 1, m_pVertexBuffer.GetAddressOf( ), &stride, &offset );

        m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP );

        m_pContext->Draw( 5, 0 );
    }

    void CRenderer::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const float color[4])  
    {  
       Vertex vertices[3] = {  
           {{x1, y1}, {color[0], color[1], color[2], color[3]}}, // Вершина 1  
           {{x2, y2}, {color[0], color[1], color[2], color[3]}}, // Вершина 2  
           {{x3, y3}, {color[0], color[1], color[2], color[3]}}  // Вершина 3  
       };  

       m_pContext->UpdateSubresource(m_pVertexBuffer.Get(), 0, nullptr, vertices, 0, 0);  

       m_pContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);  
       m_pContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);  
       m_pContext->IASetInputLayout(m_pInputLayout.Get());  

       UINT stride = sizeof(Vertex);  
       UINT offset = 0;  
       m_pContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);  

       m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  

       m_pContext->Draw(3, 0);  
    }

    void CRenderer::DrawText( const std::wstring& text, float x, float y, float width, float height, const float color[ 4 ] )
    {
        if ( !m_pD2DRenderTarget || !m_pTextFormat ) return;

        m_pD2DRenderTarget->BeginDraw( );

        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBrush;
        D2D1_COLOR_F d2dColor = D2D1::ColorF( color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ] );
        m_pD2DRenderTarget->CreateSolidColorBrush( d2dColor, &pBrush );

        D2D1_RECT_F layoutRect = D2D1::RectF( x, y, x + width, y + height );

        m_pD2DRenderTarget->DrawTextW(
            text.c_str( ),
            static_cast< UINT32 >( text.size( ) ),
            m_pTextFormat.Get( ),
            layoutRect,
            pBrush.Get( )
        );

        HRESULT hr = m_pD2DRenderTarget->EndDraw( );
        if ( hr == D2DERR_RECREATE_TARGET )
            InitializeTextRendering( );
    }

    void CRenderer::OnDeviceRestored( ID3D11RenderTargetView* pRenderTarget )
    {
        m_pRenderTarget = pRenderTarget;
        InitializeTextRendering( );
    }

    Microsoft::WRL::ComPtr<ID3DBlob> CRenderer::CompileShader(
           const std::wstring& filename,
           const std::string& entryPoint,
           const std::string& profile )
    {
        Microsoft::WRL::ComPtr<ID3DBlob> pShaderBlob;
        Microsoft::WRL::ComPtr<ID3DBlob> pErrorBlob;

        UINT uFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        uFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        uFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

        HRESULT hRes = D3DCompileFromFile(
            filename.c_str( ),
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint.c_str( ),
            profile.c_str( ),
            uFlags,
            0,
            pShaderBlob.GetAddressOf( ),
            pErrorBlob.GetAddressOf( ) );

        if ( FAILED( hRes ) )
        {
            if ( pErrorBlob )
            {
                OutputDebugStringA( ( char* ) pErrorBlob->GetBufferPointer( ) );
                throw std::exception( ( char* ) pErrorBlob->GetBufferPointer( ) );
            }
            throw std::exception( "Failed to compile shader" );
        }

        return pShaderBlob;
    }

    void CRenderer::InitializeShaders( )
    {
        auto vsBlob = CompileShader(
            L"Shaders/VertexShader.hlsl",
            "main",
            "vs_5_0" );

        m_pDevice->CreateVertexShader(
            vsBlob->GetBufferPointer( ),
            vsBlob->GetBufferSize( ),
            nullptr,
            m_pVertexShader.GetAddressOf( ) );

        auto psBlob = CompileShader(
            L"Shaders/PixelShader.hlsl",
            "main",
            "ps_5_0" );

        m_pDevice->CreatePixelShader(
            psBlob->GetBufferPointer( ),
            psBlob->GetBufferSize( ),
            nullptr,
            m_pPixelShader.GetAddressOf( ) );

        D3D11_INPUT_ELEMENT_DESC Layout[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,
             D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8,
             D3D11_INPUT_PER_VERTEX_DATA, 0}
        };

        m_pDevice->CreateInputLayout( Layout, 2, vsBlob->GetBufferPointer( ),
                                  vsBlob->GetBufferSize( ), m_pInputLayout.GetAddressOf( ) );
    }

    void CRenderer::InitializeBuffers( )
    {
        D3D11_BUFFER_DESC Desc = {};
        Desc.ByteWidth = sizeof( Vertex ) * 8; 
        Desc.Usage = D3D11_USAGE_DEFAULT;
        Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  
        std::vector<Vertex> vecVertices( 8 );

        D3D11_SUBRESOURCE_DATA Data = {};
        Data.pSysMem = vecVertices.data( );

        m_pDevice->CreateBuffer( &Desc, &Data, m_pVertexBuffer.GetAddressOf( ) );
    }

    void CRenderer::InitializeTextRendering( )
    {
        HRESULT hr;

        D2D1_FACTORY_OPTIONS options = {};
#ifdef _DEBUG
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

        hr = D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            options,
            m_pD2DFactory.GetAddressOf( )
        );
        if ( FAILED( hr ) )
            throw std::exception( ( "D2D1CreateFactory failed: 0x" + std::to_string( hr ) ).c_str( ) );

        hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof( IDWriteFactory ),
            reinterpret_cast< IUnknown** >( m_pDWriteFactory.GetAddressOf( ) )
        );
        if ( FAILED( hr ) )
            throw std::exception( ( "DWriteCreateFactory failed: 0x" + std::to_string( hr ) ).c_str( ) );

        Microsoft::WRL::ComPtr<ID3D11Resource> pResource;
        m_pRenderTarget->GetResource( pResource.GetAddressOf( ) );

        Microsoft::WRL::ComPtr<IDXGISurface> pDxgiSurface;
        hr = pResource.As( &pDxgiSurface );
        if ( FAILED( hr ) )
            throw std::exception( ( "QueryInterface(IDXGISurface) failed: 0x" + std::to_string( hr ) ).c_str( ) );

        DXGI_SURFACE_DESC surfaceDesc;
        pDxgiSurface->GetDesc( &surfaceDesc );

#ifdef _DEBUG
        char debugInfo[ 256 ];
        const char* formatName = "Unknown format";
        switch ( surfaceDesc.Format ) {
        case DXGI_FORMAT_R8G8B8A8_UNORM: formatName = "DXGI_FORMAT_R8G8B8A8_UNORM"; break;
        case DXGI_FORMAT_B8G8R8A8_UNORM: formatName = "DXGI_FORMAT_B8G8R8A8_UNORM"; break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT: formatName = "DXGI_FORMAT_R16G16B16A16_FLOAT"; break;
        }
        sprintf_s( debugInfo, "Surface format: %s (%d), Size: %ux%u",
            formatName, surfaceDesc.Format, surfaceDesc.Width, surfaceDesc.Height );
        OutputDebugStringA( debugInfo );
#endif

        D2D1_RENDER_TARGET_PROPERTIES d2dProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat( surfaceDesc.Format, D2D1_ALPHA_MODE_IGNORE ),
            0, 0
        );

        hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
            pDxgiSurface.Get( ),
            &d2dProps,
            m_pD2DRenderTarget.GetAddressOf( )
        );

        if ( FAILED( hr ) && surfaceDesc.Format != DXGI_FORMAT_B8G8R8A8_UNORM ) {
            d2dProps = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat( DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE ),
                0, 0
            );

            hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
                pDxgiSurface.Get( ),
                &d2dProps,
                m_pD2DRenderTarget.GetAddressOf( )
            );
        }

        if ( FAILED( hr ) ) {
            char buffer[ 256 ];
            sprintf_s( buffer, "CreateDxgiSurfaceRenderTarget failed: 0x%08X. Убедитесь, что D3D устройство создано с флагом D3D11_CREATE_DEVICE_BGRA_SUPPORT", hr );
            throw std::exception( buffer );
        }

        hr = m_pDWriteFactory->CreateTextFormat(
            L"Arial",
            nullptr,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            24.0f,
            L"en-us",
            m_pTextFormat.GetAddressOf( )
        );
        if ( FAILED( hr ) )
            throw std::exception( ( "CreateTextFormat failed: 0x" + std::to_string( hr ) ).c_str( ) );
    }}