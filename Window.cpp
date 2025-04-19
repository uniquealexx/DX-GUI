#include "Common.hpp"

namespace Window
{
    std::unique_ptr<CWindow> m_pWindow = nullptr;

    CWindow::CWindow( WinMainArguments* pArgs ) :
        m_hInstance( pArgs->hInstance ),
        m_nCmdShow( pArgs->nCmdShow )
    {
        const wchar_t CLASS_NAME[] = L"SampleWindowClass";

        WNDCLASS wc = {};
        wc.lpfnWndProc = StaticWindowProc;
        wc.hInstance = m_hInstance;
        wc.lpszClassName = CLASS_NAME;
        wc.hCursor = LoadCursor( NULL, IDC_ARROW );
        wc.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );

        if ( !RegisterClass( &wc ) ) {
            MessageBox( NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK );
            return;
        }

        std::thread windowThread( &CWindow::CreateWindow, this );
        windowThread.detach( );
    }

    CWindow::~CWindow( )
    {
        if ( m_hwnd ) DestroyWindow( m_hwnd );
        m_bRunning = false;
    }

    void CWindow::CreateWindow( )
    {
        const wchar_t CLASS_NAME[] = L"SampleWindowClass";

        m_hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            L"Example Window",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            WIDTH_SCREEN, HEIGHT_SCREEN,
            NULL,
            NULL,
            m_hInstance,
            this 
        );

        if ( !m_hwnd ) {
            MessageBox( NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK );
            return;
        }

        ShowWindow( m_hwnd, m_nCmdShow );
        UpdateWindow( m_hwnd );
        m_bRunning = true;
        RunMessageLoop( );
    }

    void CWindow::RunMessageLoop( )
    {
        MSG msg;
        while ( GetMessage( &msg, NULL, 0, 0 ) && m_bRunning )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );

            if ( GUI::m_pGUI )
                GUI::m_pGUI->Render( );
        }
    }

    LRESULT CALLBACK CWindow::StaticWindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        CWindow* pThis = nullptr;

        if ( uMsg == WM_NCCREATE )
        {
            CREATESTRUCT* pCreate = reinterpret_cast< CREATESTRUCT* >( lParam );
            pThis = reinterpret_cast< CWindow* >( pCreate->lpCreateParams );
            SetWindowLongPtr( hwnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( pThis ) );
            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = reinterpret_cast< CWindow* >( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
        }

        if ( pThis )
            return pThis->WindowProc( hwnd, uMsg, wParam, lParam );

        return DefWindowProc( hwnd, uMsg, wParam, lParam );
    }

    LRESULT CWindow::WindowProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        switch ( uMsg )
        {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            m_bRunning = false;
            return 0;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint( hwnd, &ps );
            EndPaint( hwnd, &ps );

            return 0;
        }

        default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
        }
    }
}