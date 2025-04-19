#include "Common.hpp"

using namespace Window;
using namespace GUI;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow )
{
    WinMainArguments args = {
        hInstance,
        nCmdShow
    };

    m_pWindow = std::make_unique<CWindow>( &args );

    // create thread
    Sleep( 200 );

    if ( !m_pWindow )
        return 1;

    m_pGUI = std::make_unique<CGUI>( m_pWindow->GetHWND( ) );

    if ( !m_pGUI )
        return 1;

    m_pFramework = std::make_unique<GUI::CFramework>();

    if (!m_pFramework)
        return 1;

    m_pFramework->Initialize();

    while (m_pWindow->IsRunning())
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Sleep(10); 
    }

    return 0;
}