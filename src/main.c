#include "head.h"
#include "txt_.h"

#include <WindowsX.h>
#include <d3d9.h>
#include <ShellAPI.h>
#include <ShlwAPI.h>
#include <stdio.h>


/*
    Глобальные переменные
*/
    /* Инстанс приложения */
    HINSTANCE       g_a7HInstance               = NULL;
    /* Переменная окна ядра для обработки сообщений */
    HWND            g_a7CoreHWnd                = NULL;
    /* Название класса окна ядра для обработки сообщений */
    CONST LPCWSTR   g_a7CoreWCName              = L"a7WC_Core";
    /* Хендл контекстного меню для иконки трея ядра */
    HMENU           g_a7CoreHMenu               = NULL;

/*
    Объявление функций
*/
    /* Обработчик сообщений ядра */
    LRESULT CALLBACK a7rCoreMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );



/* Точка входа */
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
    g_a7HInstance = hInstance;
    AllocConsole();
    /* Регистрация класса окна ядра для обработки сообщений */
    {
        WNDCLASSEXW wc;
        ZeroMemory( &wc, sizeof( wc ) );
        wc.cbSize           = sizeof( wc );
        wc.style            = 0;
        wc.lpfnWndProc      = a7rCoreMsgProc;
        wc.hInstance        = g_a7HInstance;
        wc.lpszClassName    = g_a7CoreWCName;
        /* Регистрируем класс */
        if( RegisterClassExW( &wc ) == 0 )
        {
            return GetLastError();
        }
    }
    /* Создание окна ядра */
    {
        g_a7CoreHWnd = CreateWindowExW( 0, g_a7CoreWCName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, g_a7HInstance, NULL );
        if( g_a7CoreHWnd == NULL )
        {
            return GetLastError();
        }
    }
    /* Добавляем иконку в трее */
    {
        // Yang icon AND bitmask
        BYTE ANDmaskIcon[16*16/8];
        // Yang icon XOR bitmask
        BYTE XORmaskIcon[16*16/8];

        for( int y = 0; y < 16; ++y )
        {
            for( int x = 0; x < 16; ++x )
            {
                int ii = (y * 16 + x) / 8;
                int ij = x % 8;
                if( (x+y)%2 == 0 )
                {
                    ANDmaskIcon[ii] |= 1<<ij;
                    XORmaskIcon[ii] |= 1>>ij;
                }
                else
                {
                    ANDmaskIcon[ii] &= 0xff^(1<<ij);
                    XORmaskIcon[ii] &= 0xff^(1>>ij);
                }
            }
        }

        HICON hIco = CreateIcon( g_a7HInstance, 16, 16, 1, 1, ANDmaskIcon, XORmaskIcon );

        NOTIFYICONDATAW data;
        ZeroMemory( &data, sizeof( data ) );
        data.cbSize             = sizeof(data);
        data.hWnd               = g_a7CoreHWnd;
        data.uID                = 7777;
        data.uFlags             = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        data.hIcon              = hIco;
        data.uCallbackMessage   = ( WM_USER + 1 );
        data.uVersion           = NOTIFYICON_VERSION;
        StrCpyW( data.szTip, L"Тут кароче описание иконки\nа это новая строка" );

        Shell_NotifyIcon( NIM_ADD, &data );
    }
    /* Входим в цикл обработки сообщений */
    {
        MSG msg;
        ZeroMemory( &msg, sizeof( msg ) );
        while( msg.message != WM_QUIT )
        {
            if( PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE ) )
            {
                TranslateMessage( &msg );
                DispatchMessageW( &msg );
            }
            else
            {
                /* TODO: Idle */
            }
        }
    }

    UnregisterClassW( g_a7CoreWCName, hInstance );

    return 0;
}


/* Обработчик сообщений ядра */
LRESULT CALLBACK a7rCoreMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    DWORD writenChars;
    WriteConsoleW( GetStdHandle( STD_OUTPUT_HANDLE ), a7rTxt_WM( uMsg ), wcslen( a7rTxt_WM( uMsg ) ), &writenChars, NULL );
    WriteConsoleW( GetStdHandle( STD_OUTPUT_HANDLE ), L"\n", 1, &writenChars, NULL );
    switch( uMsg )
    {
        case ( WM_KEYDOWN ):
        {
            return 0;
        }
        case ( WM_USER + 1 ):
        {
            WCHAR str[512];
            if( lParam == WM_RBUTTONDOWN )
            {
                if( g_a7CoreHMenu == NULL )
                {
                    g_a7CoreHMenu = CreatePopupMenu();
                    AppendMenuW( g_a7CoreHMenu, MF_ENABLED | MF_STRING, ( UINT_PTR )0x300, L"Закрыть меня хочешь?\tЗакройся" );
                    AppendMenuW( g_a7CoreHMenu, MF_ENABLED | MF_STRING, ( UINT_PTR )0x301, L"Давай еденичку покажи\t1" );
                    AppendMenuW( g_a7CoreHMenu, MF_ENABLED | MF_STRING, ( UINT_PTR )0x302, L"Давай двоечку покажи\t2" );
                    AppendMenuW( g_a7CoreHMenu, MF_ENABLED | MF_STRING, ( UINT_PTR )0x303, L"Давай троечку покажи\t3" );
                    AppendMenuW( g_a7CoreHMenu, MF_ENABLED | MF_STRING, ( UINT_PTR )0x304, L"Давай четверочку покажи\t4" );
                    AppendMenuW( g_a7CoreHMenu, MF_ENABLED | MF_STRING, ( UINT_PTR )0x304, L"Давай пятерочку покажи\t5" );
                    AppendMenuW( g_a7CoreHMenu, MF_ENABLED | MF_STRING, ( UINT_PTR )0x304, L"Давай шестёрку покажи\t6" );
                    AppendMenuW( g_a7CoreHMenu, MF_ENABLED | MF_STRING, ( UINT_PTR )0x304, L"Давай семёрку покажи\t7" );
                    AppendMenuW( g_a7CoreHMenu, MF_ENABLED | MF_STRING, ( UINT_PTR )0x304, L"Давай восьмёрку покажи\t8" );
                }
                POINT pt;
                GetCursorPos( &pt );
                SetForegroundWindow( g_a7CoreHWnd );
                TrackPopupMenu( g_a7CoreHMenu, TPM_CENTERALIGN | TPM_BOTTOMALIGN , pt.x, pt.y, 0, g_a7CoreHWnd, NULL );
                PostMessage( g_a7CoreHWnd, WM_NULL, 0, 0 );
            }
            swprintf( str, 511, L"W %08x L %08x W %i L %i [%ls]\n", wParam, lParam, wParam, lParam, a7rTxt_WM( lParam ) );
            WriteConsoleW( GetStdHandle( STD_OUTPUT_HANDLE ), str, wcslen( str ), &writenChars, NULL );
            return 0;
        }
        case ( WM_COMMAND ):
        {
            WCHAR str[512];
            int ID = GET_WM_COMMAND_ID( wParam, lParam );
            swprintf( str, 511, L"ID %p HWND %p CMD %i(%p)\n",
                ID,
                GET_WM_COMMAND_HWND( wParam, lParam ),
                GET_WM_COMMAND_CMD( wParam, lParam ),
                GET_WM_COMMAND_CMD( wParam, lParam ) );
            WriteConsoleW( GetStdHandle( STD_OUTPUT_HANDLE ), str, wcslen( str ), &writenChars, NULL );

            switch( ID )
            {
                case ( 0x300 ):
                {
                    PostQuitMessage( 0 );
                    break;
                }
                case ( 0x301 ):
                {
                    // Yang icon AND bitmask
                    BYTE ANDmaskIcon[16*16/8];
                    // Yang icon XOR bitmask
                    BYTE XORmaskIcon[16*16/8];

                    for( int y = 0; y < 16; ++y )
                    {
                        for( int x = 0; x < 2; ++x )
                        {
                            if( (x+y)%2 == 0 )
                            {
                                ANDmaskIcon[y*2+x] = 0x00;
                                XORmaskIcon[y*2+x] = 0x33;
                            }
                            else
                            {
                                ANDmaskIcon[y*2+x] = 0x00;
                                XORmaskIcon[y*2+x] = 0x00;
                            }
                        }
                    }

                    HICON hIco = CreateIcon( g_a7HInstance, 16, 16, 1, 1, ANDmaskIcon, XORmaskIcon );

                    NOTIFYICONDATAW data;
                    ZeroMemory( &data, sizeof( data ) );
                    data.cbSize             = sizeof(data);
                    data.hWnd               = g_a7CoreHWnd;
                    data.uID                = ID | 0x1000;
                    data.uFlags             = NIF_ICON | NIF_MESSAGE | NIF_TIP;
                    data.hIcon              = hIco;
                    data.uCallbackMessage   = ( WM_USER + 1 );
                    data.uVersion           = NOTIFYICON_VERSION;
                    StrCpyW( data.szTip, L"Тут кароче описание иконки\nа это новая строка" );

                    Shell_NotifyIcon( NIM_ADD, &data );
                    break;
                }
                case ( 0x302 ):
                {
                    // Yang icon AND bitmask
                    BYTE ANDmaskIcon[16*16/8];
                    // Yang icon XOR bitmask
                    BYTE XORmaskIcon[16*16/8];

                    for( int y = 0; y < 16; ++y )
                    {
                        for( int x = 0; x < 2; ++x )
                        {
                            if( (x+y)%2 == 0 )
                            {
                                ANDmaskIcon[y*2+x] = 0x00;
                                XORmaskIcon[y*2+x] = 0x0f;
                            }
                            else
                            {
                                ANDmaskIcon[y*2+x] = 0x00;
                                XORmaskIcon[y*2+x] = 0x00;
                            }
                        }
                    }

                    HICON hIco = CreateIcon( g_a7HInstance, 16, 16, 1, 1, ANDmaskIcon, XORmaskIcon );

                    NOTIFYICONDATAW data;
                    ZeroMemory( &data, sizeof( data ) );
                    data.cbSize             = sizeof(data);
                    data.hWnd               = g_a7CoreHWnd;
                    data.uID                = ID | 0x1000;
                    data.uFlags             = NIF_ICON | NIF_MESSAGE | NIF_TIP;
                    data.hIcon              = hIco;
                    data.uCallbackMessage   = ( WM_USER + 1 );
                    data.uVersion           = NOTIFYICON_VERSION;
                    StrCpyW( data.szTip, L"Тут кароче описание иконки\nа это новая строка" );

                    Shell_NotifyIcon( NIM_ADD, &data );
                    break;
                }

                case ( 0x303 ):
                {
                    // Yang icon AND bitmask
                    BYTE ANDmaskIcon[16*16/8];
                    // Yang icon XOR bitmask
                    BYTE XORmaskIcon[16*16/8];

                    for( int y = 0; y < 16; ++y )
                    {
                        for( int x = 0; x < 2; ++x )
                        {
                            ANDmaskIcon[y*2+x] = 0xff;
                            XORmaskIcon[y*2+x] = 0xaa;
                        }
                    }

                    HICON hIco = CreateIcon( g_a7HInstance, 16, 16, 1, 1, ANDmaskIcon, XORmaskIcon );

                    NOTIFYICONDATAW data;
                    ZeroMemory( &data, sizeof( data ) );
                    data.cbSize             = sizeof(data);
                    data.hWnd               = g_a7CoreHWnd;
                    data.uID                = ID | 0x1000;
                    data.uFlags             = NIF_ICON | NIF_MESSAGE | NIF_TIP;
                    data.hIcon              = hIco;
                    data.uCallbackMessage   = ( WM_USER + 1 );
                    data.uVersion           = NOTIFYICON_VERSION;
                    StrCpyW( data.szTip, L"Тут кароче описание иконки\nа это новая строка" );

                    Shell_NotifyIcon( NIM_ADD, &data );
                    break;
                }
            }
        }
    }
    return DefWindowProcW( hWnd, uMsg, wParam, lParam );
}
