#include "a7head.h"
#include "a7txt.h"
#include "a7log.h"

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
    /* Хендл иконки трея ядра */
    HICON           g_a7CoreHIcon               = NULL;
    /* Глобальный буффер в 64Кб для всячины */
    BYTE            g_a7CoreBuf64k[64*1024]     = { 0 };
    /* Совместимый контекст устройства */
    HDC             g_a7CoreHDC                 = NULL;
    /* Битмап иконки трея ядра */
    HBITMAP         g_a7CoreTrayIconHBMP        = NULL;
    /* Битмап маски иконки трея ядра */
    HBITMAP         g_a7CoreTrayMaskHBMP        = NULL;

/*
    Объявление функций
*/
    /* Обработчик сообщений ядра */
    LRESULT CALLBACK    a7rCoreMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


void a7rCoreTrayIconDestroy()
{

}

void a7rCoreTrayIconCreate();
void CALLBACK tmprc( HWND hwnd,   UINT msg, UINT idTimer, DWORD dwTime )
{
    // a7rCoreIconCreate();


}

void a7rCoreIconCreate()
{
    static int i = 0;
    // a7rLogD( L"Начало создания значка" );
    static HDC hDC = NULL;
    if( hDC == NULL )
    {
        hDC = CreateCompatibleDC( NULL );
    }
    // a7rLogDF( L"Получили совместимый контекст устройства\nHDC = [%p]", hDC );
    static int BitCount = 0;
    if( BitCount == 0 )
    {
        BitCount = GetDeviceCaps( hDC, BITSPIXEL );
    }
    // a7rLogDF( L"Количество битов на пиксель\nBPP = [%i]", BitCount );
    static HBITMAP hBMP = NULL;
    if( hBMP == NULL )
    {
        hBMP = CreateBitmap( 16, 16, 2, BitCount / 2, NULL );
    }
    // a7rLogDF( L"Создали пустую битмапу 16 на 16 с двумя плоскостями и количетсвом бит на пиксель %i\nHBITMAP = [%p]", BitCount / 2, hBMP );
    static HBITMAP hBMPM = NULL;
    if( hBMPM == NULL )
    {
        hBMPM = CreateBitmap( 16, 16, 1, 1, NULL );
    }
    // a7rLogDF( L"Создали пустую битмап маску 16 на 16 с одной плоскостью и одним битом на пиксель\nHBITMAP = [%p]", hBMPM );
    SelectObject( hDC, hBMP );
    // a7rLogD( L"Выбрали для контекста устройства первый битмап" );
    static HBRUSH hBrush = NULL;
    if( hBrush == NULL )
    {
        hBrush = CreateSolidBrush( 0xffffff );
    }
    // a7rLogDF( L"Создали белую кисть заполнения\nHBRUSH = [%p]", hBrush );
    static RECT rcBMP = { 0, 0, 16, 16 }; /*LTRB*/

    FillRect( hDC, &rcBMP, hBrush );
    // DeleteBrush( hBrush );

    TextOutW( hDC, 16-(i%134), 0, L"Нахуй иди, да?!", 15 );

    ICONINFO ii;
    ZeroMemory( &ii, sizeof(ICONINFO) );
    ii.hbmMask  = hBMPM;
    ii.hbmColor = hBMP;
    g_a7CoreHIcon = CreateIconIndirect( &ii );

    // a7rLogDF( L"Создали иконку\nHICON = [%p]", g_a7CoreHIcon );

    // DeleteBitmap( hBMPM );
    // DeleteBitmap( hBMP );
    // DeleteDC( hDC );

    // a7rLogD( L"Конец создания значка" );

    NOTIFYICONDATAW data;
    ZeroMemory( &data, sizeof( data ) );
    data.cbSize             = sizeof(data);
    data.hWnd               = g_a7CoreHWnd;
    data.uID                = 7777;
    data.uFlags             = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    data.hIcon              = g_a7CoreHIcon;
    data.uCallbackMessage   = ( WM_USER + 1 );
    data.uVersion           = NOTIFYICON_VERSION;
    StrCpyW( data.szTip, L"Тут кароче описание иконки\nа это новая строка" );

    if( i == 0 )
    {
        Shell_NotifyIcon( NIM_ADD, &data );
    }
    else
    {
        Shell_NotifyIcon( NIM_MODIFY, &data );
    }
    ++i;


    // {
    //     // Yang icon AND bitmask
    //     BYTE ANDmaskIcon[8*8/8];
    //     // Yang icon XOR bitmask
    //     BYTE XORmaskIcon[8*8/8];

    //     for( int y = 0; y < 8; ++y )
    //     {
    //         for( int x = 0; x < 8; ++x )
    //         {
    //             int ii = (y * 8 + x) / 8;
    //             int ij = x % 8;
    //             if( (x+y)%2 == 0 )
    //             {
    //                 ANDmaskIcon[ii] |= 1<<ij;
    //                 XORmaskIcon[ii] |= 1>>ij;
    //             }
    //             else
    //             {
    //                 ANDmaskIcon[ii] &= 0xff^(1<<ij);
    //                 XORmaskIcon[ii] &= 0xff^(1>>ij);
    //             }
    //         }
    //     }

    //     HICON hIco = CreateIcon( g_a7HInstance, 8, 8, 1, 1, ANDmaskIcon, XORmaskIcon );

    //     NOTIFYICONDATAW data;
    //     ZeroMemory( &data, sizeof( data ) );
    //     data.cbSize             = sizeof(data);
    //     data.hWnd               = g_a7CoreHWnd;
    //     data.uID                = 7777;
    //     data.uFlags             = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    //     data.hIcon              = hIco;
    //     data.uCallbackMessage   = ( WM_USER + 1 );
    //     data.uVersion           = NOTIFYICON_VERSION;
    //     StrCpyW( data.szTip, L"Тут кароче описание иконки\nа это новая строка" );

    //     Shell_NotifyIcon( NIM_ADD, &data );
    // }
}

/* Точка входа */
int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
    a7rLogV( L"Тест системы логирования... Одна строка" );
    a7rLogV( L"Тест системы логирования... В две строки\nВторая строка" );
    a7rLogV( L"Приложение A7 запущено" );
    g_a7HInstance = hInstance;
    a7rLogV( L"Инициализация" );
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
            a7rLogF( L"Невозможно зарегестрировать класс окна ядра A7" );
            return GetLastError();
        }
        else
        {
            a7rLogVF( L"Класс окна ядра A7 успешно зарегестрирован\nCLASSNAME \t= [%ls]", g_a7CoreWCName );
        }
    }
    /* Создание невидимого окна ядра для обработки сообщений */
    {
        g_a7CoreHWnd = CreateWindowExW( 0, g_a7CoreWCName, NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, g_a7HInstance, NULL );
        if( g_a7CoreHWnd == NULL )
        {
            a7rLogF( L"Невозможно создать окно ядра A7" );
            return GetLastError();
        }
        else
        {
            a7rLogVF( L"Окно ядра A7 создано\nHWND \t= [%p]", g_a7CoreHWnd );
        }
    }
    /* Получение совместимого контекста устройства */
    {
        g_a7CoreHDC = CreateCompatibleDC( NULL );
        if( g_a7CoreHDC == NULL )
        {
            a7rLogF( L"Невозможно получить Совместимый конекст устройства" );
            return GetLastError();
        }
        else
        {
            a7rLogVF( L"Совместимый контекст устройства получен\nHDC \t= [%p]", g_a7CoreHDC );
        }
    }
    /* Добавляем иконку в трее */
    {
        g_a7CoreTrayIconHBMP = CreateBitmap( 16, 16, 2, GetDeviceCaps( g_a7CoreHDC, BITSPIXEL ) / 2, NULL );
        g_a7CoreTrayMaskHBMP = CreateBitmap( 16, 16, 1, 1, NULL );
        SelectBitmap( g_a7CoreHDC, g_a7CoreTrayIconHBMP );
        {
            TRIVERTEX tv[] = {
                    {.x= 0,.y= 0,.Red=0xff00,.Green=0xff00,.Blue=0xff00,.Alpha=0 },
                    {.x=15,.y= 0,.Red=0xff00,.Green=0x0000,.Blue=0x0000,.Alpha=0 },
                    {.x=15,.y=15,.Red=0x0000,.Green=0xff00,.Blue=0x0000,.Alpha=0 },
                    {.x= 0,.y=15,.Red=0x0000,.Green=0x0000,.Blue=0xff00,.Alpha=0 },
                };
            GRADIENT_TRIANGLE gt[] = {
                    {0,1,2},
                    {2,3,0},
                };
            GradientFill( g_a7CoreHDC, tv, 4, gt, 2, GRADIENT_FILL_TRIANGLE );
        }
        // SelectBitmap( g_a7CoreHDC, g_a7CoreTrayMaskHBMP );
        // TextOutW( g_a7CoreHDC, 0, 0, L"A7", 2 );
        ICONINFO ii;
        ZeroMemory( &ii, sizeof(ICONINFO) );
        ii.hbmMask      = g_a7CoreTrayMaskHBMP;
        ii.hbmColor     = g_a7CoreTrayIconHBMP;
        g_a7CoreHIcon   = CreateIconIndirect( &ii );
        NOTIFYICONDATAW data;
        ZeroMemory( &data, sizeof( data ) );
        data.cbSize             = sizeof(data);
        data.hWnd               = g_a7CoreHWnd;
        data.uID                = 0xA7C;
        data.uFlags             = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        data.hIcon              = g_a7CoreHIcon;
        data.uCallbackMessage   = ( WM_USER + 1 );
        data.uVersion           = NOTIFYICON_VERSION;
        StrCpyW( data.szTip, L"Тут кароче описание иконки\nа это новая строка" );
        Shell_NotifyIcon( NIM_ADD, &data );
    }
    // a7rCoreIconCreate();

    // SetTimer( NULL, 0x77 , 16, tmprc);

    a7rLogV( L"Входим в цикл" );
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
    a7rLogV( L"Завершение работы" );
    {
        NOTIFYICONDATAW data;
        ZeroMemory( &data, sizeof( data ) );
        data.cbSize             = sizeof(data);
        data.hWnd               = g_a7CoreHWnd;
        data.uID                = 0xA7C;
        data.uFlags             = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        data.hIcon              = g_a7CoreHIcon;
        data.uCallbackMessage   = ( WM_USER + 1 );
        data.uVersion           = NOTIFYICON_VERSION;
        StrCpyW( data.szTip, L"Тут кароче описание иконки\nа это новая строка" );
        Shell_NotifyIcon( NIM_DELETE, &data );
        a7rLogV( L"Удалили иконку" );
    }
    {
        DeleteDC( g_a7CoreHDC );
        g_a7CoreHDC = NULL;
        a7rLogV( L"Отсвободили совместимый контекст устройства" );
    }
    {
        DestroyWindow( g_a7CoreHWnd );
        a7rLogV( L"Уничтожили невидимое окно ядра A7" );
        g_a7CoreHWnd = NULL;
    }
    {
        UnregisterClassW( g_a7CoreWCName, hInstance );
        a7rLogV( L"Отсвободили класс окна ядра A7" );
    }
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
            // a7rCoreIconCreate();
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
            }
        }
    }
    return DefWindowProcW( hWnd, uMsg, wParam, lParam );
}


