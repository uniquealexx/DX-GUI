#pragma once

namespace GUI
{
    class CGUI
    {
    public:
        CGUI( HWND hwnd );
        ~CGUI( );

        void Render( );
        void OnResize( UINT width, UINT height );

    private:
        void InitializeD3D( HWND hwnd );
        void CleanupD3D( );

        HWND m_hwnd;
        Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTarget;

        float m_clearColor[ 4 ] = { 0.45f, 0.55f, 0.60f, 1.00f };
    };

    extern std::unique_ptr<CGUI> m_pGUI;
}
