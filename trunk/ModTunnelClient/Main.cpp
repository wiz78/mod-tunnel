#include "Daemon.h"

//---------------------------------------------------------------------------

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

static Daemon *DaemonObj = NULL;

//---------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    WNDCLASS	wc;
	HWND		hWnd;
	MSG			msg;

	memset( &wc, 0, sizeof( wc ));

    wc.lpfnWndProc   = (WNDPROC)WndProc; 
    wc.hInstance     = hInstance;
    wc.lpszClassName = "ModTunnelClientWndClass";

    if( !RegisterClass( &wc )) 
		return( FALSE );

    // Create the main window. 
    hWnd = CreateWindow( wc.lpszClassName, "ModTunnelClient", 
                         0, CW_USEDEFAULT, CW_USEDEFAULT, 
                         CW_USEDEFAULT, CW_USEDEFAULT, (HWND) NULL, 
                         (HMENU) NULL, hInstance, (LPVOID) NULL );

    // If the main window cannot be created, terminate
    // the application.  
    if( !hWnd ) 
		return( FALSE );

    // Start the message loop.
    while( GetMessage( &msg, (HWND) NULL, 0, 0 ) == TRUE ) {
        TranslateMessage( &msg );
        DispatchMessage( &msg );
    }

    // Return the exit code to Windows.      
    return( msg.wParam );
}
//---------------------------------------------------------------------------
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch( uMsg ) {

		case WM_CREATE:
            DaemonObj = new Daemon();
            break;

        case WM_DESTROY:
			delete DaemonObj;

            PostQuitMessage( 0 );
            break;

        default:
            return( DefWindowProc( hWnd, uMsg, wParam, lParam ));
    }

    return( 0 );
}
//---------------------------------------------------------------------------
