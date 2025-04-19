#pragma once

namespace GUI
{
    struct Vertex
    {
        DirectX::XMFLOAT2 pos;
        DirectX::XMFLOAT4 color;
    };

    class CRenderer
    {
    public:
        CRenderer( Microsoft::WRL::ComPtr<ID3D11Device> device,
                 Microsoft::WRL::ComPtr<ID3D11DeviceContext> context );
        ~CRenderer( );

        void DrawRectFilled( float x, float y, float width, float height, const float color[ 4 ] );

    private:
        void InitializeShaders( );
        void InitializeBuffers( );

        Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
            const std::wstring& filename,
            const std::string& entryPoint,
            const std::string& profile );

        Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
    };

    extern std::unique_ptr<CRenderer> m_pRenderer;
}