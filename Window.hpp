#pragma once

#undef CreateWindow

namespace Window
{
    struct WinMainArguments
    {
        HINSTANCE hInstance;
        int       nCmdShow;
    };

    class CWindow
    {
    public:
        explicit CWindow( WinMainArguments* pArgs );
        ~CWindow( );

        bool IsRunning( ) const { return m_bRunning; }
        HWND GetHWND( ) const { return m_hwnd; }

    private:
        static LRESULT CALLBACK StaticWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
        LRESULT WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

        void RunMessageLoop( );
        void CreateWindow( );

        HWND m_hwnd = nullptr;
        HINSTANCE m_hInstance;
        int m_nCmdShow;
        bool m_bRunning = false;
    };

    extern std::unique_ptr<CWindow> m_pWindow;
}