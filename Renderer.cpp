#include "Common.hpp"

namespace GUI
{
    std::unique_ptr<CRenderer> m_pRenderer = nullptr;

    CRenderer::CRenderer( Microsoft::WRL::ComPtr<ID3D11Device> device,
                       Microsoft::WRL::ComPtr<ID3D11DeviceContext> context )
        : m_pDevice( device ), m_pContext( context )
    {
        InitializeShaders( );
        InitializeBuffers( );
    }

    CRenderer::~CRenderer( )
    {}

    void CRenderer::DrawRectFilled( float x, float y, float width, float height, const float color[ 4 ] )
    {
        // Создаем временный буфер вершин с нужным цветом и координатами
        Vertex vertices[ 4 ] = {
            {{x,         y},          {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Верхний левый
            {{x + width, y},          {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Верхний правый
            {{x,         y + height}, {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}, // Нижний левый
            {{x + width, y + height}, {color[ 0 ], color[ 1 ], color[ 2 ], color[ 3 ]}}  // Нижний правый
        };

        // Обновляем вершинный буфер новыми данными
        m_pContext->UpdateSubresource( m_pVertexBuffer.Get( ), 0, nullptr, vertices, 0, 0 );

        // Устанавливаем шейдеры и буферы
        m_pContext->VSSetShader( m_pVertexShader.Get( ), nullptr, 0 );
        m_pContext->PSSetShader( m_pPixelShader.Get( ), nullptr, 0 );
        m_pContext->IASetInputLayout( m_pInputLayout.Get( ) );

        // Устанавливаем вершинный буфер
        UINT stride = sizeof( Vertex );
        UINT offset = 0;
        m_pContext->IASetVertexBuffers( 0, 1, m_pVertexBuffer.GetAddressOf( ), &stride, &offset );

        // Устанавливаем примитивную топологию
        m_pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

        // Рисуем квадрат
        m_pContext->Draw( 4, 0 );
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
        // Компиляция вершинного шейдера
        auto vsBlob = CompileShader(
            L"Shaders/VertexShader.hlsl",
            "main",
            "vs_5_0" );

        m_pDevice->CreateVertexShader(
            vsBlob->GetBufferPointer( ),
            vsBlob->GetBufferSize( ),
            nullptr,
            m_pVertexShader.GetAddressOf( ) );

        // Компиляция пиксельного шейдера
        auto psBlob = CompileShader(
            L"Shaders/PixelShader.hlsl",
            "main",
            "ps_5_0" );

        m_pDevice->CreatePixelShader(
            psBlob->GetBufferPointer( ),
            psBlob->GetBufferSize( ),
            nullptr,
            m_pPixelShader.GetAddressOf( ) );

        // Создание входного лейаута
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
        // Вершины единичного квадрата
        std::vector<Vertex> vecVertices = {
            {{0.0f, 0.0f}, {1,1,1,1}},
            {{1.0f, 0.0f}, {1,1,1,1}},
            {{1.0f, 1.0f}, {1,1,1,1}},
            {{0.0f, 1.0f}, {1,1,1,1}}
        };

        // Создание вершинного буфера
        D3D11_BUFFER_DESC Desc = {};
        Desc.ByteWidth = sizeof( Vertex ) * 4;
        Desc.Usage = D3D11_USAGE_DEFAULT;
        Desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA Data = {};
        Data.pSysMem = vecVertices.data( );

        m_pDevice->CreateBuffer( &Desc, &Data, m_pVertexBuffer.GetAddressOf( ) );
    }
}