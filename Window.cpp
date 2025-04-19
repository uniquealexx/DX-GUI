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
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        const float scaleX = WIDTH_SCREEN / static_cast<float>(clientRect.right);
        const float scaleY = HEIGHT_SCREEN / static_cast<float>(clientRect.bottom);

        switch (uMsg)
        {
        case WM_LBUTTONDOWN:
        {
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            const float mouseX = pt.x * scaleX;
            const float mouseY = pt.y * scaleY;

            if (GUI::Framework::m_pForm && GUI::Framework::m_pForm->IsHovered(mouseX, mouseY))
            {
                s_dragContext.isDragging = true;
                s_dragContext.mouseStartPos = pt;
                s_dragContext.formStartX = GUI::Framework::m_pForm->GetX();
                s_dragContext.formStartY = GUI::Framework::m_pForm->GetY();
                SetCapture(hwnd);
            }
            return 0;
        }

        case WM_MOUSEMOVE:
        {
            if (s_dragContext.isDragging && GUI::Framework::m_pForm)
            {
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

                // Вычисляем дельту в GUI-координатах
                const float deltaX = (pt.x - s_dragContext.mouseStartPos.x) * scaleX;
                const float deltaY = (pt.y - s_dragContext.mouseStartPos.y) * scaleY;

                // Рассчитываем новую позицию с учетом ограничений
                float newX = s_dragContext.formStartX + deltaX;
                float newY = s_dragContext.formStartY + deltaY;

                // Применяем ограничения сразу при перемещении
                const float formWidth = GUI::Framework::m_pForm->GetWidth();
                const float formHeight = GUI::Framework::m_pForm->GetHeight();

                // Вместо жесткого clamp
                const float SNAP_THRESHOLD = 10.0f; // Пиксели для прилипания

                newX = std::clamp(newX, -SNAP_THRESHOLD, WIDTH_SCREEN - formWidth + SNAP_THRESHOLD);
                newY = std::clamp(newY, -SNAP_THRESHOLD, HEIGHT_SCREEN - formHeight + SNAP_THRESHOLD);

                // Применяем прилипание
                if (newX < 0) newX = 0;
                else if (newX > WIDTH_SCREEN - formWidth) newX = WIDTH_SCREEN - formWidth;

                if (newY < 0) newY = 0;
                else if (newY > HEIGHT_SCREEN - formHeight) newY = HEIGHT_SCREEN - formHeight;

                GUI::Framework::m_pForm->SetPosition(newX, newY);

                // Форсируем перерисовку
                InvalidateRect(hwnd, NULL, FALSE);
                UpdateWindow(hwnd);
            }
            return 0;
        }
        case WM_LBUTTONUP:
        {
            if (s_dragContext.isDragging)
            {
                s_dragContext.isDragging = false;
                ReleaseCapture();
            }
            return 0;
        }

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
       
        case WM_SIZE:
        {
            const UINT width = LOWORD(lParam);
            const UINT height = HIWORD(lParam);
            m_guiscaleX = WIDTH_SCREEN / width;
            m_guiscaleY = HEIGHT_SCREEN / height;
            if (GUI::m_pGUI) {
                GUI::m_pGUI->OnResize(width, height);
            }
            break;
        }

        default:
            return DefWindowProc( hwnd, uMsg, wParam, lParam );
        }
    }
}