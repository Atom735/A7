#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN

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
    #define D3DFVF_2DT  (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
    /* коды ошибок */
    enum {
        E_OK = 0,
        E_D3D_NULL,
        E_D3DD_NULL,
    };

/* Глобальные перменные */
    /* Библиотека FreeType */
    FT_Library                  g_ftLib;
    /* Фонт фейс FreeType */
    FT_Face                     g_ftFace;

    /* Интефрейс D3D9 */
    IDirect3D9                 *g_D3D;
    /* Интерфейс устройства D3D9 */
    IDirect3DDevice9           *g_D3DD;
    /* Буффер вершин */
    IDirect3DVertexBuffer9     *g_D3DVB;
    /* Буффер индексов вершин */
    IDirect3DIndexBuffer9      *g_D3DIB;

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
INT     D3D_Init( HWND hWnd, UINT nWidth, UINT nHeight )
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
            LOGI("D3D9 Adapter[%u]: %ux%u:%u (%s)", i+1, dm.Width, dm.Height, dm.RefreshRate, GetName_D3DFMT( dm.Format ) );
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
                        LOGI("D3D9 Adapter[%u]: [%u/%u] %ux%u:%u (%s)", i+1, j+1, nAMC, dm.Width, dm.Height, dm.RefreshRate, GetName_D3DFMT( dm.Format ) ); \
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
    return E_OK;
}
/* Очистка контекста Direct3D9 */
VOID    D3D_Release()
{
    if( g_D3DD != NULL )
        IDirect3DDevice9_Release( g_D3DD );
    if( g_D3D != NULL )
        IDirect3D9_Release( g_D3D );
}

//-----------------------------------------------------------------------------
// Name: InitVB()
// Desc: Creates a vertex buffer and fills it with our vertices. The vertex
//       buffer is basically just a chuck of memory that holds vertices. After
//       creating it, we must Lock()/Unlock() it to fill it. For indices, D3D
//       also uses index buffers. The special thing about vertex and index
//       buffers is that they can be created in device memory, allowing some
//       cards to process them in hardware, resulting in a dramatic
//       performance gain.
//-----------------------------------------------------------------------------
// HRESULT InitVB()
// {

//     if( slot->format == FT_GLYPH_FORMAT_OUTLINE ) {
//         LOGI( "FT_GLYPH_FORMAT_OUTLINE" );
//         LOGI( "n_contours: %hd", slot->outline.n_contours );
//         LOGI( "n_points: %hd", slot->outline.n_points );
//         n_points = slot->outline.n_points;
//         if( FAILED( IDirect3DDevice9_CreateVertexBuffer( g_D3DD, n_points * sizeof( struct CUSTOMVERTEX ),
//                                                   0, D3DFVF_CUSTOMVERTEX,
//                                                   D3DPOOL_DEFAULT, &g_D3DVB, NULL ) ) )
//         {
//             return E_FAIL;
//         }
//         VOID* pVertices;
//         if( FAILED( IDirect3DVertexBuffer9_Lock( g_D3DVB, 0, n_points * sizeof( struct CUSTOMVERTEX ), ( void** )&pVertices, 0 ) ) )
//             return E_FAIL;
//         struct CUSTOMVERTEX *pV = (struct CUSTOMVERTEX *)pVertices;
//         for (int i = 0; i < n_points; ++i)
//         {
//             pV[i].x = (FLOAT)slot->outline.points[i].x;
//             pV[i].y = (FLOAT)slot->outline.points[i].y;
//             pV[i].z = 0.5f;
//             pV[i].rhw = 1.0f;
//             pV[i].color = 0xff7f7f7f;
//         }
//         IDirect3DVertexBuffer9_Unlock( g_D3DVB );

//         return S_OK;

//     }
//     return S_OK;
// }




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
// VOID Render()
// {
//     if( NULL == g_D3DD )
//         return;

//     // Clear the backbuffer to a blue color
//     IDirect3DDevice9_Clear( g_D3DD, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

//     // Begin the scene
//     if( SUCCEEDED( IDirect3DDevice9_BeginScene( g_D3DD ) ) )
//     {
//         // Rendering of scene objects can happen here
//         // Draw the triangles in the vertex buffer. This is broken into a few
//         // steps. We are passing the vertices down a "stream", so first we need
//         // to specify the source of that stream, which is our vertex buffer. Then
//         // we need to let D3D know what vertex shader to use. Full, custom vertex
//         // shaders are an advanced topic, but in most cases the vertex shader is
//         // just the FVF, so that D3D knows what type of vertices we are dealing
//         // with. Finally, we call DrawPrimitive() which does the actual rendering
//         // of our geometry (in this case, just one triangle).
//         IDirect3DDevice9_SetStreamSource( g_D3DD, 0, g_D3DVB, 0, sizeof( struct CUSTOMVERTEX ) );
//         IDirect3DDevice9_SetFVF( g_D3DD, D3DFVF_CUSTOMVERTEX );
//         int b = 0;
//         for (int i = 0; i < slot->outline.n_contours; ++i)
//         {
//             IDirect3DDevice9_DrawPrimitive( g_D3DD, D3DPT_LINESTRIP, b, slot->outline.contours[i] -b );
//             b = slot->outline.contours[i]+1;
//             /* code */
//         }

//         // End the scene
//         IDirect3DDevice9_EndScene( g_D3DD );
//     }

//     // Present the backbuffer contents to the display
//     IDirect3DDevice9_Present( g_D3DD, NULL, NULL, NULL, NULL );
// }




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            D3D_Release();
            PostQuitMessage( 0 );
            return 0;

        case WM_PAINT:
            // Render();
            ValidateRect( hWnd, NULL );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, INT iCmd)
{
    // Register the window class
    WNDCLASSEX wc =
    {
        sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
        GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
        L"D3D Tutorial", NULL
    };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( L"D3D Tutorial", L"D3D Tutorial 01: CreateDevice",
                              WS_OVERLAPPEDWINDOW, 100, 100, 1024, 768,
                              NULL, NULL, wc.hInstance, NULL );

    // FT_Init_FreeType( &_ftLib );
    // FT_New_Face( _ftLib, "font.ttf", 0, &_ftFace );
    // // FT_Set_Char_Size( _ftFace, 0, 12*64, 96, 96 );
    // FT_Set_Pixel_Sizes( _ftFace, 0, 48 );
    // ;

    // FT_Load_Glyph( _ftFace, FT_Get_Char_Index( _ftFace, L'О' ), FT_LOAD_NO_SCALE );
    // slot = _ftFace->glyph;  /* a small shortcut */

    // if(slot->format == FT_GLYPH_FORMAT_NONE)
    //     LOG("FT_GLYPH_FORMAT_NONE");
    // if(slot->format == FT_GLYPH_FORMAT_COMPOSITE)
    //     LOG("FT_GLYPH_FORMAT_COMPOSITE");
    // if(slot->format == FT_GLYPH_FORMAT_BITMAP)
    //     LOG("FT_GLYPH_FORMAT_BITMAP");
    // if(slot->format == FT_GLYPH_FORMAT_OUTLINE)
    //     LOG("FT_GLYPH_FORMAT_OUTLINE");
    // if(slot->format == FT_GLYPH_FORMAT_PLOTTER)
    //     LOG("FT_GLYPH_FORMAT_PLOTTER");




    // Initialize Direct3D
    if( SUCCEEDED( D3D_Init( hWnd, 0, 0 ) ) )
    {
        // Create the vertex buffer
        // if( SUCCEEDED( InitVB() ) )
        {
            // Show the window
            ShowWindow( hWnd, SW_SHOWDEFAULT );
            UpdateWindow( hWnd );

            // Enter the message loop
            MSG msg;
            ZeroMemory( &msg, sizeof( msg ) );
            while( msg.message != WM_QUIT )
            {
                if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
                {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }
            }
        }
    }


    // FT_Done_Face( _ftFace );
    // FT_Done_FreeType( _ftLib );

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}



