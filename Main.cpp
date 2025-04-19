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

    while ( m_pWindow->IsRunning( ) )
    {
        // messages and etc
        Sleep( 10 );
    }

    return 0;
}