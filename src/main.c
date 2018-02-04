#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN

#define D3D_DEBUG_INFO

#include <Windows.h>
#include <WindowsX.h>
#include <d3d9.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "log.h"


/* Структуры */
    /* Структура точек D3D9 2d Diffuse Texture */
    typedef struct T_VERTEX_2DT
    {
        FLOAT   x, y, z, w;   /* координаты точки */
        DWORD   c;            /* цвет точки */
        FLOAT   u, v;         /* координаты текстуры */
    } T_VERTEX_2DT, *PT_VERTEX_2DT;
/* Константы */
    /* Константа структуры точек D3D9 2d Diffuse Texture */
    #define D3DFVF_2DT  (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
    /* коды ошибок */
    enum {
        E_OK = 0,
        E_D3D_NULL,
        E_D3DD_NULL,
    };
/* Глобальные перменные */
    /* Интефрейс D3D9 */
    IDirect3D9                 *g_D3D       = NULL;
    /* Интерфейс устройства D3D9 */
    IDirect3DDevice9           *g_D3DD      = NULL;
    /* Буффер вершин */
    IDirect3DVertexBuffer9     *g_D3DVB     = NULL;
    /* Буффер индексов вершин */
    IDirect3DIndexBuffer9      *g_D3DIB     = NULL;


    /* Библиотека FreeType */
    FT_Library                  g_ftLib;
    /* Фонт фейс FreeType */
    FT_Face                     g_ftFace;

VOID    D3D_ResInit()
{
    IDirect3DDevice9_CreateVertexBuffer( g_D3DD, 4096*sizeof(T_VERTEX_2DT), D3DUSAGE_WRITEONLY, D3DFVF_2DT, D3DPOOL_DEFAULT, &g_D3DVB, NULL );
    {
        T_VERTEX_2DT *pV;
        IDirect3DVertexBuffer9_Lock( g_D3DVB, 0, 4096*sizeof(T_VERTEX_2DT), (VOID**)&pV, 0 );
        for (UINT i = 0; i < 4096; ++i)
        {
            pV[i].x = i;
            pV[i].y = (sin(i*0.03)+1)*500;
            pV[i].z = (FLOAT)0.5;
            pV[i].w = (FLOAT)1;
            pV[i].c = 0xff000000+(i&255)*0x010101;
        }
        IDirect3DVertexBuffer9_Unlock( g_D3DVB );
    }
    IDirect3DDevice9_CreateIndexBuffer( g_D3DD, 4096*sizeof(UINT16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_D3DIB, NULL );
    {
        UINT16 *pI;
        IDirect3DIndexBuffer9_Lock( g_D3DIB, 0, 4096*sizeof(UINT16), (VOID**)&pI, 0 );
        for (UINT i = 0; i < 4096; ++i)
        {
            pI[i] = i;
        }
        IDirect3DIndexBuffer9_Unlock( g_D3DIB );
    }

}

VOID    D3D_Render()
{
    IDirect3DDevice9_Clear( g_D3DD, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0x10,0x50,0x90), 0.0f, 0);
    IDirect3DDevice9_BeginScene( g_D3DD );
    {
        IDirect3DDevice9_SetStreamSource( g_D3DD, 0, g_D3DVB, 0, sizeof( T_VERTEX_2DT ) );
        IDirect3DDevice9_SetFVF( g_D3DD, D3DFVF_2DT );
        IDirect3DDevice9_SetIndices( g_D3DD, g_D3DIB );
        IDirect3DDevice9_DrawIndexedPrimitive( g_D3DD, D3DPT_LINELIST, 0, 0, 4096, 0, 2048 );
    }
    IDirect3DDevice9_EndScene( g_D3DD );
    IDirect3DDevice9_Present( g_D3DD, NULL, NULL, NULL, NULL );
}


/* Получить название формата D3D9 */
const char * GetName_D3DFMT( D3DFORMAT fmt )
{
    switch( fmt ) {
        case D3DFMT_UNKNOWN: return "UNKNOWN";
        case D3DFMT_R8G8B8: return "R8G8B8";
        case D3DFMT_A8R8G8B8: return "A8R8G8B8";
        case D3DFMT_X8R8G8B8: return "X8R8G8B8";
        case D3DFMT_R5G6B5: return "R5G6B5";
        case D3DFMT_X1R5G5B5: return "X1R5G5B5";
        case D3DFMT_A1R5G5B5: return "A1R5G5B5";
        case D3DFMT_A4R4G4B4: return "A4R4G4B4";
        case D3DFMT_R3G3B2: return "R3G3B2";
        case D3DFMT_A8: return "A8";
        case D3DFMT_A8R3G3B2: return "A8R3G3B2";
        case D3DFMT_X4R4G4B4: return "X4R4G4B4";
        case D3DFMT_A2B10G10R10: return "A2B10G10R10";
        case D3DFMT_A8B8G8R8: return "A8B8G8R8";
        case D3DFMT_X8B8G8R8: return "X8B8G8R8";
        case D3DFMT_G16R16: return "G16R16";
        case D3DFMT_A2R10G10B10: return "A2R10G10B10";
        case D3DFMT_A16B16G16R16: return "A16B16G16R16";
        case D3DFMT_A8P8: return "A8P8";
        case D3DFMT_P8: return "P8";
        case D3DFMT_L8: return "L8";
        case D3DFMT_A8L8: return "A8L8";
        case D3DFMT_A4L4: return "A4L4";
        case D3DFMT_V8U8: return "V8U8";
        case D3DFMT_L6V5U5: return "L6V5U5";
        case D3DFMT_X8L8V8U8: return "X8L8V8U8";
        case D3DFMT_Q8W8V8U8: return "Q8W8V8U8";
        case D3DFMT_V16U16: return "V16U16";
        case D3DFMT_A2W10V10U10: return "A2W10V10U10";
        case D3DFMT_UYVY: return "UYVY";
        case D3DFMT_R8G8_B8G8: return "R8G8_B8G8";
        case D3DFMT_YUY2: return "YUY2";
        case D3DFMT_G8R8_G8B8: return "G8R8_G8B8";
        case D3DFMT_DXT1: return "DXT1";
        case D3DFMT_DXT2: return "DXT2";
        case D3DFMT_DXT3: return "DXT3";
        case D3DFMT_DXT4: return "DXT4";
        case D3DFMT_DXT5: return "DXT5";
        case D3DFMT_D16_LOCKABLE: return "D16_LOCKABLE";
        case D3DFMT_D32: return "D32";
        case D3DFMT_D15S1: return "D15S1";
        case D3DFMT_D24S8: return "D24S8";
        case D3DFMT_D24X8: return "D24X8";
        case D3DFMT_D24X4S4: return "D24X4S4";
        case D3DFMT_D16: return "D16";
        case D3DFMT_L16: return "L16";
        case D3DFMT_D32F_LOCKABLE: return "D32F_LOCKABLE";
        case D3DFMT_D24FS8: return "D24FS8";
        case D3DFMT_VERTEXDATA: return "VERTEXDATA";
        case D3DFMT_INDEX16: return "INDEX16";
        case D3DFMT_INDEX32: return "INDEX32";
        case D3DFMT_Q16W16V16U16: return "Q16W16V16U16";
        case D3DFMT_MULTI2_ARGB8: return "MULTI2_ARGB8";
        case D3DFMT_R16F: return "R16F";
        case D3DFMT_G16R16F: return "G16R16F";
        case D3DFMT_A16B16G16R16F: return "A16B16G16R16F";
        case D3DFMT_R32F: return "R32F";
        case D3DFMT_G32R32F: return "G32R32F";
        case D3DFMT_A32B32G32R32F: return "A32B32G32R32F";
        case D3DFMT_CxV8U8: return "CxV8U8";
        case D3DFMT_FORCE_DWORD: return "FORCE_DWORD";
    };
    return "???";
}

/* Инициализация контекста Direct3D9 */
UINT    D3D_Init( HWND hWnd, UINT nWidth, UINT nHeight )
{
    /* Пытаемся получить интерфейс g_D3D */
    g_D3D = Direct3DCreate9( D3D_SDK_VERSION );
    /* Проверяем на ошибку */
    if( g_D3D == NULL )
    {
        LOGE("Can't get D3D9 interface");
        return E_D3D_NULL;
    }
    LOGD("D3D9 interface: %p", g_D3D);
    /* Перечесляем адаптеры и типы буфферов */
    {
        /* Получаем количество адаптеров */
        UINT nAC = IDirect3D9_GetAdapterCount( g_D3D );
        LOGI("D3D9 Adapters count: %u", nAC);
        D3DDISPLAYMODE dm;
        for (UINT i = 0; i < nAC; ++i)
        {
            /* Получаем тип отображения адаптера */
            IDirect3D9_GetAdapterDisplayMode( g_D3D, i, &dm );
            LOGI("D3D9 Adapter[%u/%u]: %ux%u:%u (%s)", i+1, nAC, dm.Width, dm.Height, dm.RefreshRate, GetName_D3DFMT( dm.Format ) );
            if(nWidth == (UINT)-1) nWidth = dm.Width;
            if(nHeight == (UINT)-1) nHeight = dm.Height;
            /* Перечисляем возможные типы отображения адаптера */
            UINT nAMC;
            #define __A( fmt ) \
                    /* Получаем количество возможных параметров отображения адаптера */ \
                    nAMC = IDirect3D9_GetAdapterModeCount( g_D3D, i, fmt ); \
                    for (int j = 0; j < nAMC; ++j) \
                    { \
                        IDirect3D9_EnumAdapterModes( g_D3D, i, fmt, j, &dm ); \
                        LOGI("D3D9 Adapter[%u/%u]: [%u/%u] %ux%u:%u (%s)", i+1, nAC, j+1, nAMC, dm.Width, dm.Height, dm.RefreshRate, GetName_D3DFMT( dm.Format ) ); \
                    }
                __A(D3DFMT_A1R5G5B5)
                __A(D3DFMT_A2R10G10B10)
                __A(D3DFMT_A8R8G8B8)
                __A(D3DFMT_R5G6B5)
                __A(D3DFMT_X1R5G5B5)
                __A(D3DFMT_X8R8G8B8)
            #undef __A
        }

    }
    /* Устанавливаем параметры отображения */
    D3DPRESENT_PARAMETERS d3dpp;
    {
        ZeroMemory( &d3dpp, sizeof( d3dpp ) );
        // UINT BackBufferWidth;
        // UINT BackBufferHeight;
        // D3DFORMAT BackBufferFormat;
        // UINT BackBufferCount;
        // D3DMULTISAMPLE_TYPE MultiSampleType;
        // DWORD MultiSampleQuality;
        // D3DSWAPEFFECT SwapEffect;
        // HWND hDeviceWindow;
        // BOOL Windowed;
        // BOOL EnableAutoDepthStencil;
        // D3DFORMAT AutoDepthStencilFormat;
        // DWORD Flags;
        // UINT FullScreen_RefreshRateInHz;
        // UINT PresentationInterval;
        d3dpp.BackBufferWidth   = nWidth;
        d3dpp.BackBufferHeight  = nHeight;
        d3dpp.BackBufferFormat  = D3DFMT_A2R10G10B10;
        d3dpp.SwapEffect        = D3DSWAPEFFECT_DISCARD;
        d3dpp.hDeviceWindow     = hWnd;
        d3dpp.Windowed          = TRUE;
    }
    /* Получаем интерфейс устройства D3D9 */

    if( FAILED( IDirect3D9_CreateDevice( g_D3D, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_D3DD ) ) )
    {
        LOGE("Can't get D3D9 Device interface");
        return E_D3DD_NULL;
    }
    LOGD("D3D9 Device interface: %p", g_D3DD);
    D3D_ResInit();
    return E_OK;
}
/* Очистка контекста Direct3D9 */
VOID    D3D_Release()
{
    if( g_D3DIB != NULL )
        IDirect3DIndexBuffer9_Release( g_D3DIB );
    if( g_D3DVB != NULL )
        IDirect3DVertexBuffer9_Release( g_D3DVB );
    if( g_D3DD != NULL )
        IDirect3DDevice9_Release( g_D3DD );
    if( g_D3D != NULL )
        IDirect3D9_Release( g_D3D );
}
/* Обработчик сообщений окна */
LRESULT WINAPI WndMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
        /* Обработка создания окна */
        case WM_CREATE:
        {
            UINT e = D3D_Init( hWnd, (UINT)-1, (UINT)-1 );
            if( e == E_D3D_NULL )
            {
                return -1;
            }

            if( e == E_D3DD_NULL )
            {
                IDirect3DDevice9_Release( g_D3DD );
                return -1;
            }
            return 0;
        }
        /* Обработка закрытия окна */
        case WM_CLOSE:
            D3D_Release();
            DestroyWindow( hWnd );
            return 0;
        /* Обработка уничтожения окна */
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
        /* Обработка перерисовки окна */
        case WM_PAINT:
            D3D_Render();
            ValidateRect( hWnd, NULL );
            return 0;
    }
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
/* Точка входа */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, INT iCmd)
{
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
        wc.style            = CS_CLASSDC;
        wc.lpfnWndProc      = WndMsgProc;
        wc.hInstance        = hInst;
        wc.lpszClassName    = TEXT("A7Con");
    }
    RegisterClassEx( &wc );
    /* Создаём окно приложения */
    HWND hWnd = CreateWindow( wc.lpszClassName, TEXT("Empty title..."), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wc.hInstance, NULL );
    /* Проверяем созданно ли */
    if( hWnd == NULL )
    {
        UnregisterClass( wc.lpszClassName, wc.hInstance );
        return -1;
    }
    ShowWindow( hWnd, SW_SHOW );
    UpdateWindow( hWnd );

    MSG msg;
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            D3D_Render();
        }
    }

    UnregisterClass( wc.lpszClassName, wc.hInstance );
    return msg.wParam;
}
