#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#define D3D_DEBUG_INFO

#include <Windows.h>
#include <WindowsX.h>

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    LOG_T_VERB = 0,
    LOG_T_DEBUG,
    LOG_T_INFO,
    LOG_T_WARN,
    LOG_T_ERROR,
    LOG_T_FATAL,
};

#define LOG(...) LOG_T(LOG_T_VERB,   __VA_ARGS__)
#define LOGV(...) LOG_T(LOG_T_VERB,  __VA_ARGS__)
#define LOGD(...) LOG_T(LOG_T_DEBUG, __VA_ARGS__)
#define LOGI(...) LOG_T(LOG_T_INFO,  __VA_ARGS__)
#define LOGW(...) LOG_T(LOG_T_WARN,  __VA_ARGS__)
#define LOGE(...) LOG_T(LOG_T_ERROR, __VA_ARGS__)
#define LOGF(...) LOG_T(LOG_T_FATAL, __VA_ARGS__)
void LOG_T( unsigned int type, const char *fmt, ... );
LRESULT WINAPI WP_WndMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK rECP_( HWND hWnd, LPARAM lParam );


HWND WP_hWnd = NULL;


HHOOK hk;
LRESULT  CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MessageBoxA(NULL,"Hook","Hook",MB_OK);
    return CallNextHookEx(hk, nCode, wParam, lParam);
}


/* Точка входа */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, INT iCmd)
{
    {
        HWND hWnd = GetDesktopWindow();
        LPARAM z = 0;
        while( WP_hWnd == NULL )
        {
            EnumChildWindows( hWnd, rECP_, z );
            ++z;
            if( z > 32 ) break;
        }
    }
    LOGI("hWnd: %p", WP_hWnd);
    if( WP_hWnd == NULL )
    {
        INT c = __COUNTER__ + 0xfefe0000;
        LOGF( "ERROR: 0x%x: Can't find desktop window", c );
        exit( c );
    }

    HWND WP_hWndParent = GetParent( WP_hWnd );
    LOGI("hWndParent: %p", WP_hWndParent);
    GetParent( WP_hWndParent );
    LOGI("hWndParent: %p", WP_hWndParent);
    /* Регистрация класса окна */
    WNDCLASSEX wc;
    {
        ZeroMemory( &wc, sizeof( wc ) );
        // UINT  cbSize;
        // UINT  style;
        // WNDPROC  lpfnWndProc;
        // int  cbClsExtra;
        // int  cbWndExtra;
        // HINSTANCE  hInstance;
        // HICON  hIcon;
        // HCURSOR  hCursor;
        // HBRUSH  hbrBackground;
        // LPCTSTR  lpszMenuName;
        // LPCTSTR  lpszClassName;
        // HICON  hIconSm;
        wc.cbSize           = sizeof( wc );
        wc.style            = 0/*CS_CLASSDC*/;
        wc.lpfnWndProc      = WP_WndMsgProc;
        wc.hInstance        = hInst;
        wc.hbrBackground    = (HBRUSH) (COLOR_WINDOW+2);
        wc.lpszClassName    = TEXT("C-Atom735");
    }
    RegisterClassEx( &wc );

    HWND hWnd = CreateWindow( wc.lpszClassName, TEXT("W-Atom735"), WS_OVERLAPPEDWINDOW , CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wc.hInstance, NULL );
    /* Проверяем созданно ли */
    if( hWnd == NULL )
    {
        INT c = __COUNTER__ + 0xfefe0000;
        LOGF( "ERROR: 0x%x: Can't create child desctop window", c );
        UnregisterClass( wc.lpszClassName, wc.hInstance );
        exit( c );
    }
    ShowWindow( hWnd, SW_SHOW );
    UpdateWindow( hWnd );

    hk = SetWindowsHookEx(WH_KEYBOARD,&HookProc, NULL, 0);

    // SetParent( WP_hWnd, hWnd );
    // if(0)
    // for (int i = 0; i < 256; ++i)
    // {
    //     HDC WP_hDC = GetWindowDC( WP_hWndParent );
    //     if( WP_hDC == NULL )
    //     {
    //         INT c = __COUNTER__ + 0xfefe0000;
    //         LOGF( "ERROR: 0x%x: Can't get desktop device context", c );
    //         exit( c );
    //     }
    //     RECT rc = { 10, 10, 100, 100 };
    //     FillRect( WP_hDC, &rc, (HBRUSH) (COLOR_WINDOW+3) );
    //     ReleaseDC( WP_hWndParent, WP_hDC );
    //     InvalidateRect( WP_hWndParent, NULL, FALSE );
    //     PostMessage( WP_hWndParent, WM_PAINT, 0, 0 );
    //     InvalidateRect( WP_hWnd, NULL, FALSE );
    //     PostMessage( WP_hWnd, WM_PAINT, 0, 0 );
    //     Sleep( 10 );
    // }
    // SetParent( WP_hWnd, WP_hWndParent );
    // Sleep( 1000 );
    // DestroyWindow( hWnd );
    // system("pause");
    // return 0;

    MSG msg;
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            // LOGV( "% 10d MSG: to hWnd[%p] % 8d w[%p] l[%p]", msg.time, msg.hwnd, msg.message, msg.wParam, msg.lParam );
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }
    UnregisterClass( wc.lpszClassName, wc.hInstance );
    UnhookWindowsHookEx(hk);
    return msg.wParam;
}

#define LOG_BUF_SIZE 4096
void LOG_T( unsigned int type, const char *fmt, ... )
{

    static WORD awColors[] = { 0x08, 0x03, 0x0A, 0x0E, 0x0C, 0xCF, };
    static HANDLE hOut = NULL;
    static char achTypes[] = { 'V', 'D', 'I', 'W', 'E', 'F'};
    static char buf[ LOG_BUF_SIZE ];
    static DWORD writenChars = 0;
    if( hOut == NULL ) {
        hOut = GetStdHandle( STD_OUTPUT_HANDLE );
    }
    HANDLE hOutF = CreateFileA( "log.log", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    assert( type <= LOG_T_FATAL );
    assert( hOut != NULL );
    assert( hOutF != NULL );
    SetFilePointer( hOutF, 0, NULL, FILE_END );

    SetConsoleTextAttribute( hOut, awColors[ type ] );

    unsigned int ticks = GetTickCount();
    unsigned int n = snprintf ( buf, LOG_BUF_SIZE, "%c[% 10d] ", achTypes[type], ticks );
    WriteConsoleA( hOut, buf, n, &writenChars, NULL );
    WriteFile( hOutF, buf, n, &writenChars, NULL );

    va_list args;
    va_start (args, fmt);
    unsigned int end = _vsnprintf (buf, LOG_BUF_SIZE, fmt, args);
    va_end (args);
    unsigned int begin = 0;
    unsigned int newline = 1;
    for(unsigned int i = 0; i < end; ++i) {
        if(buf[i] == '\n') {
            if(newline == 0) {
                newline = 1;
                if(begin > 0) {
                    WriteConsoleA( hOut, "\n              ", 15, &writenChars, NULL );
                    WriteFile( hOutF, "\n              ", 15, &writenChars, NULL );
                }
                WriteConsoleA( hOut, buf+begin, i-begin, &writenChars, NULL );
                WriteFile( hOutF, buf+begin, i-begin, &writenChars, NULL );
                begin = i+1;
                continue;
            } else {
                begin = i+1;
            }
        } else {
            newline = 0;
        }
    }
    if(end > begin) {
        WriteConsoleA( hOut, buf+begin, end-begin, &writenChars, NULL );
        WriteFile( hOutF, buf+begin, end-begin, &writenChars, NULL );
    }
    WriteConsoleA( hOut, "\n", 1, &writenChars, NULL );
    WriteFile( hOutF, "\n", 1, &writenChars, NULL );
    SetConsoleTextAttribute(hOut, 0x07);
    CloseHandle( hOutF );
}



BOOL CALLBACK rECP_( HWND hWnd, LPARAM lParam )
{
    static char rECP_bufClass[256];
    static char rECP_bufTxt[256];
    static UINT rECP_u;
    rECP_u = RealGetWindowClassA( hWnd, rECP_bufClass, 255 );
    rECP_bufClass[rECP_u] = 0;
    rECP_u = GetWindowTextA( hWnd, rECP_bufTxt, 255 );
    rECP_bufTxt[rECP_u] = 0;
    if( strcmp( "SysListView32", rECP_bufClass ) == 0 )
    {
        if( strcmp( "FolderView", rECP_bufTxt ) == 0 )
        {
            WP_hWnd = hWnd;
            return FALSE;
        }

    }
    if( lParam > 0 )
    {
        EnumChildWindows( hWnd, rECP_, lParam-1 );
    }
    return TRUE;
}

int g_iMouseX = 0;
int g_iMouseY = 0;
/* Обработчик сообщений окна */
LRESULT WINAPI WP_WndMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    // LOGV( "MSG: to hWnd[%p] % 8d w[%p] l[%p]", hWnd, uMsg, wParam, lParam );
    switch( uMsg )
    {
        /* Обработка перемещения курсора внутри окна */
        case WM_MOUSEMOVE:
        {
            g_iMouseX = GET_X_LPARAM(lParam);
            g_iMouseY = GET_Y_LPARAM(lParam);
            return 0;
        }
        /* Обработка закрытия окна */
        case WM_CLOSE:
            DestroyWindow( hWnd );
            return 0;
        /* Обработка уничтожения окна */
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
        /* Обработка перерисовки окна */
        case WM_PAINT:
            InvalidateRect( WP_hWnd, NULL, FALSE );
            ValidateRect( WP_hWnd, NULL );
            return 0;
    }
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
