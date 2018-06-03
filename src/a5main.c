#include <Windows.h>
#include <WindowsX.h>
#include <Stdio.h>
#include <Stdarg.h>

#include <D3DX9.h>

CONST LPCWSTR   g_lpWndClassName    = L"Случайное название";
HINSTANCE       g_hInstance         = NULL;
HANDLE          g_hConsoleOut       = NULL;
HANDLE          g_hFileOut          = NULL;
HANDLE          g_hThreadRender     = NULL;
DWORD           g_dwThreadIdRender  = 0;
BOOL            g_bThreadRenderActive   = FALSE;
BOOL                    g_bcsLOGDBG  = FALSE;
CRITICAL_SECTION        g_csLOGDBG;
LPDIRECT3D9             g_pD3D      = NULL;
LPDIRECT3DDEVICE9       g_pD3DDev   = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pD3DVB    = NULL;


extern CONST LPCSTR g_a5TabWM[];

typedef struct CUSTOMVERTEX
{
    FLOAT x, y, z;
    DWORD color;
} CUSTOMVERTEX;
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)


VOID LOGDBG( LPCWSTR fmt, ... )
{
    WCHAR pBuf[1024];
    DWORD dw;
    INT nLen = 0;
    va_list args;
    va_start( args, fmt );
    nLen =  snwprintf( pBuf, 1024, L"[% 8X][% 10d]: ", GetCurrentThreadId(), GetTickCount() );
    nLen+= vsnwprintf( pBuf+nLen, 1024-nLen, fmt, args );
    pBuf[nLen] = L'\n';
    if( g_bcsLOGDBG )
    {
        EnterCriticalSection( &g_csLOGDBG );
        WriteConsoleW( g_hConsoleOut, pBuf, nLen+1, &dw, NULL );
        WriteFile( g_hFileOut, pBuf, (nLen+1)*sizeof(WCHAR), &dw, NULL );
        LeaveCriticalSection( &g_csLOGDBG );
    }
    else
    {
        WriteConsoleW( g_hConsoleOut, pBuf, nLen+1, &dw, NULL );
        WriteFile( g_hFileOut, pBuf, (nLen+1)*sizeof(WCHAR), &dw, NULL );
    }
    va_end( args );
}

DWORD WINAPI rThreadRender( LPVOID lpParam )
{
    {
        if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        {
            LOGDBG( L"Не смогли получить интерфейс Direct3D9" );
            return E_FAIL;
        }
        LOGDBG( L"Получили интерфейс Direct3D9" );
        LOGDBG( L"g_pD3D = [% 8X]", g_pD3D );

        D3DPRESENT_PARAMETERS d3dpp;
        ZeroMemory( &d3dpp, sizeof( d3dpp ) );
        d3dpp.Windowed = TRUE;
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
        // d3dpp.EnableAutoDepthStencil = TRUE;
        // d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

        if( FAILED( IDirect3D9_CreateDevice( g_pD3D, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (HWND)(lpParam), D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDev ) ) )
        {
            LOGDBG( L"Не смогли получить интерфейс Direct3D9Device" );
            if( g_pD3D != NULL )
            {
                IDirect3D9_Release( g_pD3D );
                LOGDBG( L"Освободили интерфейс Direct3D9" );
                LOGDBG( L"g_pD3D = [% 8X] (NULL)", g_pD3D );
                g_pD3D = NULL;
            }
            return E_FAIL;
        }
        LOGDBG( L"Получили интерфейс Direct3D9Device" );
        LOGDBG( L"g_pD3DDev = [% 8X]", g_pD3DDev );

        IDirect3DDevice9_SetRenderState( g_pD3DDev, D3DRS_CULLMODE, D3DCULL_NONE );
        IDirect3DDevice9_SetRenderState( g_pD3DDev, D3DRS_LIGHTING, FALSE );
        // IDirect3DDevice9_SetRenderState( g_pD3DDev, D3DRS_ZENABLE, TRUE );

    }

    {
        CUSTOMVERTEX g_Vertices[] =
        {
            { -1.0f,-1.0f, 0.0f, 0xffff0000, },
            {  1.0f,-1.0f, 0.0f, 0xff0000ff, },
            {  0.0f, 1.0f, 0.0f, 0xffffffff, },
        };
        if( FAILED( IDirect3DDevice9_CreateVertexBuffer( g_pD3DDev, 3 * sizeof( CUSTOMVERTEX ), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pD3DVB, NULL ) ) )
        {
            LOGDBG( L"Не смогли создать буффер вершин" );
        }
        LPVOID pVertices;
        if( FAILED( IDirect3DVertexBuffer9_Lock( g_pD3DVB, 0, sizeof( g_Vertices ), ( void** )&pVertices, 0 ) ) )
        {
            LOGDBG( L"Не смогли заблокироать буффер вершин" );
        }
        else
        {
            memcpy( pVertices, g_Vertices, sizeof( g_Vertices ) );
            IDirect3DVertexBuffer9_Unlock( g_pD3DVB );
        }
    }

    static DWORD dwTickLast = 0;
    static DWORD dwTickNew = 0;
    static DWORD dwTickDelta = 0;
    dwTickNew = dwTickLast = GetTickCount();
    while( g_bThreadRenderActive != FALSE )
    {
        dwTickNew = GetTickCount();
        if( dwTickNew == dwTickLast )
        {
            Sleep( 1 );
            continue;
        }
        dwTickDelta = dwTickNew - dwTickLast;
        dwTickLast = dwTickNew;

        if( g_pD3DDev == NULL )
        {
            LOGDBG( L"Интерфейс Direct3D9Device пуст, ошибка, остановка рендера" );
            break;
        }
        INT i = ( GetTickCount() >> 1 ) & 0x1ff;
        if( i>0xff ) i = 0x1ff-i;
        i/=5;
        IDirect3DDevice9_Clear( g_pD3DDev, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( i, i, i ), 1.0f, 0 );
        if( SUCCEEDED( IDirect3DDevice9_BeginScene( g_pD3DDev ) ) )
        {
            D3DXMATRIX matWorld;

            UINT iTime = dwTickLast % 1000;
            FLOAT fAngle = iTime * ( 2.0f * D3DX_PI ) / 1000.0f;
            D3DXMatrixRotationY( &matWorld, fAngle );
            IDirect3DDevice9_SetTransform( g_pD3DDev, D3DTS_WORLD, &matWorld );

            D3DXVECTOR3 vEyePt = { 0.0f, 3.0f,-5.0f };
            D3DXVECTOR3 vLookatPt = { 0.0f, 0.0f, 0.0f };
            D3DXVECTOR3 vUpVec = { 0.0f, 1.0f, 0.0f };
            D3DXMATRIX matView;
            D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
            IDirect3DDevice9_SetTransform( g_pD3DDev, D3DTS_VIEW, &matView );

            D3DXMATRIX matProj;
            D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f );
            IDirect3DDevice9_SetTransform( g_pD3DDev, D3DTS_PROJECTION, &matProj );


            IDirect3DDevice9_SetStreamSource( g_pD3DDev, 0, g_pD3DVB, 0, sizeof( CUSTOMVERTEX ) );
            IDirect3DDevice9_SetFVF( g_pD3DDev, D3DFVF_CUSTOMVERTEX );
            IDirect3DDevice9_DrawPrimitive( g_pD3DDev, D3DPT_TRIANGLESTRIP, 0, 1 );

            IDirect3DDevice9_EndScene( g_pD3DDev );
        }
        else
        {
            LOGDBG( L"Интерфейс Direct3D9Device не может начать отрисовку сцены" );
        }
        IDirect3DDevice9_Present( g_pD3DDev, NULL, NULL, NULL, NULL );
    }

    {
        if( g_pD3DVB != NULL )
        {
            IDirect3DVertexBuffer9_Release( g_pD3DVB );
            g_pD3DVB = NULL;
        }
        if( g_pD3DDev != NULL )
        {
            IDirect3DDevice9_Release( g_pD3DDev );
            LOGDBG( L"Освободили интерфейс Direct3D9Device" );
            LOGDBG( L"g_pD3DDev = [% 8X] (NULL)", g_pD3DDev );
            g_pD3DDev = NULL;
        }
        if( g_pD3D != NULL )
        {
            IDirect3D9_Release( g_pD3D );
            LOGDBG( L"Освободили интерфейс Direct3D9" );
            LOGDBG( L"g_pD3D = [% 8X] (NULL)", g_pD3D );
            g_pD3D = NULL;
        }
    }
    return S_OK;
}

LRESULT CALLBACK rMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( uMsg < WM_USER )
    {
        LOGDBG( L"HWND[% 8X] MSG[% 4X] WP[% 8X] LP[% 8X] %hs", hWnd, uMsg, wParam, lParam, g_a5TabWM[uMsg] );
    }
    else
    {
        LOGDBG( L"HWND[% 8X] MSG[% 4X] WP[% 8X] LP[% 8X] %hs+%i", hWnd, uMsg, wParam, lParam, g_a5TabWM[WM_USER], uMsg-WM_USER );
    }
    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            switch( wParam )
            {
                case VK_ESCAPE:
                {
                    if( MessageBoxW( hWnd, L"Дейстительно выйти?", L"Моя игрушка ^_^", MB_OKCANCEL | MB_ICONQUESTION ) == IDOK)
                    {
                        DestroyWindow( hWnd );
                    }
                    return 0;
                }
            }
            break;
        }
        case WM_CREATE:
        {
            g_bThreadRenderActive = TRUE;
            g_hThreadRender = CreateThread( NULL, 0, rThreadRender, (LPVOID)(hWnd), 0, &g_dwThreadIdRender );

            LOGDBG( L"Создан поток Рендера" );
            LOGDBG( L"g_hThreadRender = [% 8X]", g_hThreadRender );
            LOGDBG( L"g_dwThreadIdRender = [% 8X]", g_dwThreadIdRender );

            if( g_hThreadRender == NULL )
            {
                LOGDBG( L"Не удалось создать поток Рендера" );
                return -1;
            }
            return 0;
        }
        case WM_CLOSE:
        {
            if( MessageBoxW( hWnd, L"Дейстительно выйти?", L"Моя игрушка ^_^", MB_OKCANCEL | MB_ICONQUESTION ) == IDOK)
            {
                DestroyWindow( hWnd );
            }
            DestroyWindow( hWnd );
            return 0;
        }
        case WM_DESTROY:
        {
            if( g_bThreadRenderActive == TRUE )
            {
                g_bThreadRenderActive = FALSE;
                WaitForSingleObject( g_hThreadRender, INFINITE );
                CloseHandle( g_hThreadRender );
            }
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW( hWnd, uMsg, wParam, lParam );
}

INT APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR pCmdLine, INT nShowCmd )
{
    #ifndef NDEBUG
        AllocConsole();
        g_hConsoleOut = GetStdHandle( STD_OUTPUT_HANDLE );
        g_hFileOut = CreateFileW( L"log.log", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
        SetFilePointer( g_hFileOut, 0, NULL, FILE_END );

        if( ( g_bcsLOGDBG = InitializeCriticalSectionAndSpinCount( &g_csLOGDBG, 0x00000400 ) ) == FALSE)
        {
            LOGDBG( L"Не возможно создать критическую секцию для сообщений отладки" );
        }
    #endif
    g_hInstance = hInstance;
    LOGDBG( L"g_hInstance = [%p]", g_hInstance );

    {
        WNDCLASSEXW wc;
        ZeroMemory( &wc, sizeof( wc ) );

        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
        wc.lpfnWndProc   = rMsgProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = g_hInstance;
        wc.hIcon         = LoadIconW(NULL, IDI_WINLOGO);
        wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = g_lpWndClassName;
        wc.hIconSm       = LoadIconW(NULL, IDI_WINLOGO);

        if( RegisterClassExW( &wc ) == 0 )
        {
            LOGDBG( L"Невозможно зарегестрировать класс окна" );
            DWORD er = GetLastError();
            LOGDBG( L"Код ошибки: %08X (%d)", er, er );
            return er;
        }
        else
        {
            LOGDBG( L"Класс окна успешно зарегестрирован" );
            LOGDBG( L"g_lpWndClassName = [%s]", g_lpWndClassName );
        }
    }

    {
        HWND hWnd = CreateWindowExW( 0, g_lpWndClassName, NULL, WS_POPUP, 0, 0, GetSystemMetrics( SM_CXSCREEN ), GetSystemMetrics( SM_CYSCREEN ), HWND_DESKTOP, NULL, g_hInstance, NULL );
        /*CW_USEDEFAULT*/
        if( hWnd == NULL )
        {
            LOGDBG( L"Невозможно создать окно" );
            DWORD er = GetLastError();
            LOGDBG( L"Код ошибки: %08X (%d)", er, er );
            return er;
        }
        else
        {
            LOGDBG( L"Окно создано" );
            LOGDBG( L"HWND = [%p]", hWnd );
            ShowWindow( hWnd, SW_SHOW );
            UpdateWindow( hWnd );
            SetFocus( hWnd );
        }
    }



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

    UnregisterClassW( g_lpWndClassName, g_hInstance );
    LOGDBG( L"Класс окна успешно освобожден" );


    #ifndef NDEBUG
        DeleteCriticalSection( &g_csLOGDBG );
        g_bcsLOGDBG = FALSE;
        LOGDBG( L"Освободили критическую секцию" );
        CloseHandle( g_hFileOut );
        system("pause");
    #endif
    return 0;
}
