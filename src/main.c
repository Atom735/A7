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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* Структуры */
    /* Структура точек D3D9 2d Diffuse Texture */
    typedef struct T_VERTEX_2DT
    {
        FLOAT   x, y, z, w;   /* координаты точки */
        DWORD   c;            /* цвет точки */
        FLOAT   u, v;         /* координаты текстуры */
    } T_VERTEX_2DT;
    /* Структуры символов */
    typedef struct T_FTSYM { /* Сортировка сначала по возврастанию nH, потом по возврастанию iU */
        uint32_t    iU;     /* юникод символа */
        uint16_t    tX, tY; /* координаты текстуры */
        uint8_t     tW, tH; /* размеры текстуры */
        int8_t      aX, aY; /* отступы текстуры от пера */
        uint8_t     nA;     /* расстояние до след символа */
        uint8_t     nH;     /* шрифтовая высота символа */
        uint8_t     _0, _1; /* дополнительные данные, для выравнивания */
    } T_FTSYM;
    /* Структура заголовка текстуры */
    typedef struct T_FTTEX {
        uint32_t    magic;  /* магическая константа */
        uint16_t    size;   /* размер текстуры */
        uint16_t    count;  /* количесто объектов */
        uint16_t    pX, pY; /* позиция пера заполнения */
        uint16_t    nH;     /* высота заполняемой линии */
        uint8_t     _0, _1; /* дополнительные данные, для выравнивания */
    } T_FTTEX;
    /* Структура текста */
    typedef struct T_STEXT
    {
        unsigned int                count;      /* Количество рисуемых символов */
        IDirect3DVertexBuffer9     *D3DVB;      /* Буффер вершин */
        IDirect3DIndexBuffer9      *D3DIB;      /* Буффер индексов вершин */
    } T_STEXT;

/* Объявление функций */
    /* выделение память под FT7 */
    T_FTTEX *   FT7_Alloc( unsigned int size );
    /* освобождение памяти из под FT7 */
    void        FT7_Free( T_FTTEX *p );
    /* получает указатель на массив символов FT7 */
    T_FTSYM *   FT7_GetSymbols( T_FTTEX *p );
    /* добавить новый символ FT7 */
    T_FTSYM *   FT7_AddSymbol( T_FTTEX *p, unsigned int iU, unsigned int w, unsigned int h, const void *pbuf, unsigned int pitch );
    /* добавить новый символ FT7 из шрифта */
    T_FTSYM *   FT7_AddSymbolByFace( T_FTTEX *p, FT_Face pF, unsigned int h, unsigned int iU );
    /* найти символ */
    T_FTSYM *   FT7_GetSymbolStd( T_FTTEX *p, unsigned int h, unsigned int iU );
    /* обновляет текстуру */
    void IDirect3DTexture9_FT7Synhro( IDirect3DTexture9 *t, T_FTTEX *p );
    /* создаёт статический текст */
    T_STEXT *   STT_Alloc( T_FTTEX *ftt, FT_Face pF, unsigned int h, FLOAT x, FLOAT y, DWORD c, const char *txt );
    /* освобождает статический текст */
    void        STT_Free( T_STEXT *p );
    /* рисует статический текст */
    void        STT_Draw( T_STEXT *p );

/* Константы */
    /* Константа структуры точек D3D9 2d Diffuse Texture */
    #define D3DFVF_2DT  (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
    /* коды ошибок */
    enum {
        E_OK = 0,
        E_D3D_NULL,
        E_D3DD_NULL,
    };
    /* магическая константа T_FTTEX */
    #define FT7_MAGIC ((uint32_t)0xf706a360)
/* Глобальные перменные */
    /* Интефрейс D3D9 */
    IDirect3D9                 *g_D3D       = NULL;
    /* Интерфейс устройства D3D9 */
    IDirect3DDevice9           *g_D3DD      = NULL;
    /* Буффер вершин */
    IDirect3DVertexBuffer9     *g_D3DVB     = NULL;
    /* Буффер индексов вершин */
    IDirect3DIndexBuffer9      *g_D3DIB     = NULL;
    /* Текстура */
    IDirect3DTexture9          *g_D3DTEX    = NULL;
    /* Порт просмотра */
    D3DVIEWPORT9                g_D3DVP;


    /* Библиотека FreeType */
    FT_Library                  g_ftLib;
    /* Фонт фейс FreeType */
    FT_Face                     g_ftFace;
    /* Данные фонта */
    T_FTTEX                    *g_ft7Tex;
    /* Позиции мыши */
    int                         g_iMouseX, g_iMouseY;
    int                         g_iMouse0   = 1;

    T_STEXT                    *g_ST[4];


/* Инициализация ресурсов */
VOID    D3D_ResInit()
{

    FT_Init_FreeType( &g_ftLib );
    FT_New_Face( g_ftLib, "font.ttf", 0, &g_ftFace );
    g_ft7Tex = FT7_Alloc( 2048 );

    g_ST[0] = STT_Alloc( g_ft7Tex, g_ftFace, 24, 0.0, 24.0, 0xff007fff, "###!Hello World!" );
    g_ST[1] = STT_Alloc( g_ft7Tex, g_ftFace, 24, 0.5, 48.0, 0xff007fff, "###!Hello World!" );
    g_ST[2] = STT_Alloc( g_ft7Tex, g_ftFace, 24, 0.0, 72.5, 0xff007fff, "###!Hello World!" );
    g_ST[3] = STT_Alloc( g_ft7Tex, g_ftFace, 24, 0.5, 96.5, 0xff007fff, "###!Hello World!" );


    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_LIGHTING, FALSE );
    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_ALPHABLENDENABLE, TRUE );
    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_BLENDOP, D3DBLENDOP_ADD );

    IDirect3DDevice9_CreateTexture( g_D3DD, g_ft7Tex->size, g_ft7Tex->size, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8, D3DPOOL_DEFAULT, &g_D3DTEX, NULL );

    IDirect3DTexture9_FT7Synhro( g_D3DTEX, g_ft7Tex );


    IDirect3DDevice9_CreateVertexBuffer( g_D3DD, 4096*4*sizeof(T_VERTEX_2DT), D3DUSAGE_WRITEONLY, D3DFVF_2DT, D3DPOOL_DEFAULT, &g_D3DVB, NULL );
    {
        T_VERTEX_2DT *pV;
        IDirect3DVertexBuffer9_Lock( g_D3DVB, 0, 4096*4*sizeof(T_VERTEX_2DT), (VOID**)&pV, 0 );
        for (UINT i = 0; i < 4096; ++i)
        {
            pV[i*4+0].x = (i%64)*32;
            pV[i*4+0].y = (i/64)*32;
            pV[i*4+0].z = (FLOAT)0.5;
            pV[i*4+0].w = (FLOAT)1;
            pV[i*4+0].c = 0xffffffff;

            pV[i*4+1].x = (i%64)*32+32;
            pV[i*4+1].y = (i/64)*32;
            pV[i*4+1].z = (FLOAT)0.5;
            pV[i*4+1].w = (FLOAT)1;
            pV[i*4+1].c = 0xffff0000;

            pV[i*4+2].x = (i%64)*32;
            pV[i*4+2].y = (i/64)*32+32;
            pV[i*4+2].z = (FLOAT)0.5;
            pV[i*4+2].w = (FLOAT)1;
            pV[i*4+2].c = 0xff00ff00;

            pV[i*4+3].x = (i%64)*32+32;
            pV[i*4+3].y = (i/64)*32+32;
            pV[i*4+3].z = (FLOAT)0.5;
            pV[i*4+3].w = (FLOAT)1;
            pV[i*4+3].c = 0xff0000ff;
        }
        IDirect3DVertexBuffer9_Unlock( g_D3DVB );
    }
    IDirect3DDevice9_CreateIndexBuffer( g_D3DD, 4096*6*sizeof(UINT16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_D3DIB, NULL );
    {
        UINT16 *pI;
        IDirect3DIndexBuffer9_Lock( g_D3DIB, 0, 4096*6*sizeof(UINT16), (VOID**)&pI, 0 );
        for (UINT i = 0; i < 4096; ++i)
        {
            pI[i*6+0] = i*4+0;
            pI[i*6+1] = i*4+1;
            pI[i*6+2] = i*4+2;
            pI[i*6+3] = i*4+2;
            pI[i*6+4] = i*4+1;
            pI[i*6+5] = i*4+3;
        }
        IDirect3DIndexBuffer9_Unlock( g_D3DIB );
    }

    LOGI("resources init!");
}
/* Особождение ресурсов */
VOID    D3D_ResFree()
{

    if( g_D3DTEX != NULL )
        IDirect3DTexture9_Release( g_D3DTEX );
    if( g_D3DIB != NULL )
        IDirect3DIndexBuffer9_Release( g_D3DIB );
    if( g_D3DVB != NULL )
        IDirect3DVertexBuffer9_Release( g_D3DVB );

    STT_Free( g_ST[0] );
    STT_Free( g_ST[1] );
    STT_Free( g_ST[2] );
    STT_Free( g_ST[3] );

    FT7_Free( g_ft7Tex );
    FT_Done_Face( g_ftFace );
    FT_Done_FreeType( g_ftLib );
    LOGI("resources free!");
}
/* Функция отрисовки */
VOID    D3D_Render()
{
    IDirect3DDevice9_Clear( g_D3DD, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0x10,0x50,0x90), 0.0f, 0);
    IDirect3DDevice9_BeginScene( g_D3DD );
    IDirect3DDevice9_SetStreamSource( g_D3DD, 0, g_D3DVB, 0, sizeof( T_VERTEX_2DT ) );
    IDirect3DDevice9_SetFVF( g_D3DD, D3DFVF_2DT );
    IDirect3DDevice9_SetIndices( g_D3DD, g_D3DIB );

    IDirect3DDevice9_DrawIndexedPrimitive( g_D3DD, D3DPT_TRIANGLELIST, 0, 0, 4096*4, 0, 4096*2 );
    {
        T_VERTEX_2DT *pV;
        IDirect3DVertexBuffer9_Lock( g_D3DVB, 0, 8*sizeof(T_VERTEX_2DT), (VOID**)&pV, 0 );

        pV[0].x = (8-g_iMouseX)*g_iMouse0*2;
        pV[0].y = (8-g_iMouseY)*g_iMouse0*2;
        pV[0].u = 0.0f;
        pV[0].v = 0.0f;
        pV[0].c = 0x7f000000;

        pV[1].x = pV[0].x+g_ft7Tex->size*g_iMouse0;
        pV[1].y = pV[0].y;
        pV[1].u = 1.0f;
        pV[1].v = pV[0].v;
        pV[1].c = 0x7f000000;

        pV[2].x = pV[0].x;
        pV[2].y = pV[0].y+g_ft7Tex->size*g_iMouse0;
        pV[2].u = pV[0].u;
        pV[2].v = 1.0f;
        pV[2].c = 0x7f000000;

        pV[3].x = pV[1].x;
        pV[3].y = pV[2].y;
        pV[3].u = pV[1].u;
        pV[3].v = pV[2].v;
        pV[3].c = 0x7f000000;

        pV += 4;

        pV[0].x = (8-g_iMouseX)*g_iMouse0*2;
        pV[0].y = (8-g_iMouseY)*g_iMouse0*2;
        pV[0].u = 0.0f;
        pV[0].v = 0.0f;

        pV[1].x = pV[0].x+g_ft7Tex->size*g_iMouse0;
        pV[1].y = pV[0].y;
        pV[1].u = 1.0f;
        pV[1].v = pV[0].v;

        pV[2].x = pV[0].x;
        pV[2].y = pV[0].y+g_ft7Tex->size*g_iMouse0;
        pV[2].u = pV[0].u;
        pV[2].v = 1.0f;

        pV[3].x = pV[1].x;
        pV[3].y = pV[2].y;
        pV[3].u = pV[1].u;
        pV[3].v = pV[2].v;

        IDirect3DVertexBuffer9_Unlock( g_D3DVB );

    }

    IDirect3DDevice9_DrawIndexedPrimitive( g_D3DD, D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2 );

    IDirect3DDevice9_SetTexture( g_D3DD, 0, (IDirect3DBaseTexture9*)g_D3DTEX );
    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );
    IDirect3DDevice9_DrawIndexedPrimitive( g_D3DD, D3DPT_TRIANGLELIST, 0, 0, 8, 6, 2 );

    STT_Draw( g_ST[0] );
    STT_Draw( g_ST[1] );
    STT_Draw( g_ST[2] );
    STT_Draw( g_ST[3] );


    IDirect3DDevice9_SetTexture( g_D3DD, 0, NULL );

    IDirect3DDevice9_EndScene( g_D3DD );
    IDirect3DDevice9_Present( g_D3DD, (RECT*)&g_D3DVP, NULL, NULL, NULL );
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
    D3D_ResFree();
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
        /* Обработка нажатия левой клавиши мыши */
        case WM_LBUTTONDOWN:
            g_iMouse0 = 2;
            return 0;
        /* Обработка нажатия левой клавиши мыши */
        case WM_RBUTTONDOWN:
            g_iMouse0 = 4;
            return 0;
        /* Обработка нажатия левой клавиши мыши */
        case WM_MBUTTONDOWN:
            g_iMouse0 = 8;
            return 0;
        /* Обработка нажатия левой клавиши мыши */
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_LBUTTONUP:
            g_iMouse0 = 1;
            return 0;
        /* Обработка перемещения курсора внутри окна */
        case WM_MOUSEMOVE:
            g_iMouseX = GET_X_LPARAM(lParam);
            g_iMouseY = GET_Y_LPARAM(lParam);
            return 0;
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
        /* Обработка изменениев размера окна */
        case WM_SIZE:
            g_D3DVP.X       = 0;
            g_D3DVP.Y       = 0;
            g_D3DVP.Width   = LOWORD( lParam );
            g_D3DVP.Height  = HIWORD( lParam );
            g_D3DVP.MinZ    = 0.0f;
            g_D3DVP.MaxZ    = 1.0f;
            if( g_D3DVP.Width == 0 ) g_D3DVP.Width = 1;
            if( g_D3DVP.Height == 0 ) g_D3DVP.Height = 1;
            IDirect3DDevice9_SetViewport( g_D3DD, &g_D3DVP );
            D3D_Render();
            return 0;
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
        LOGF("Can't create wiondow");
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


/* выделение память под FT7 */
T_FTTEX *   FT7_Alloc( unsigned int size )
{
    /* проверяем число на ноль */
    if( size == 0 )
    {
        LOGW("FT7: size is null");
        return NULL;
    }
    /* проверяем число на размер */
    if( size > 0x8000 )
    {
        LOGE("FT7: size %u is too large", size);
        return NULL;
    }
    /* подсчитываем количество битов в числе */
    {
        unsigned int a = 0;
        for( unsigned int i = 0; i < sizeof( unsigned int ) * 8; ++i )
        {
            if((size>>i)&1) ++a;
        }
        if( a != 1 )
        {
            LOGW("FT7 size %u is not pow of 2", size);
        }
    }
    /* выделяем память под FT7 */
    T_FTTEX *o = ( T_FTTEX* )VirtualAlloc( NULL, size*size, MEM_COMMIT, PAGE_READWRITE );
    LOGD("FT7: %p alloc (%ux%u)[%x]", o, size, size, size*size);
    if( o == NULL )
    {
        LOGF("FT7 size %u not allocated", size);
        return NULL;
    }
    /* заполняем данные структуры FT7 */
    {
        o->magic    = FT7_MAGIC;
        o->size     = size;
        o->count    = 0;
        o->pX       = 0;
        o->pY       = size;
        o->nH       = 0;
    }
    return o;
}
/* освобождение памяти из под FT7 */
void        FT7_Free( T_FTTEX *p )
{
    /* проверяем на пустой указатель */
    if( p == NULL )
    {
        LOGF("FT7: can't free NULL pointer");
        return;
    }
    /* проверяем указатель на валидность FT7 */
    if( p->magic != FT7_MAGIC )
    {
        LOGE("FT7: can't free not ft7 struct %p", p);
        return;
    }
    LOGD("FT7: %p free (%ux%u)[%x]", p, p->size, p->size, p->size);
    /* освобождаем память */
    VirtualFree( p, 0, MEM_RELEASE );
}
/* получает указатель на массив символов FT7 */
T_FTSYM *   FT7_GetSymbols( T_FTTEX *p )
{
    /* проверяем на пустой указатель */
    if( p == NULL )
    {
        LOGF("FT7: can't get symbols by NULL pointer");
        return NULL;
    }
    /* проверяем указатель на валидность FT7 */
    if( p->magic != FT7_MAGIC )
    {
        LOGE("FT7: can't get symbols by not ft7 struct %p", p);
        return NULL;
    }
    /* возвращаем указатель на данные символов */
    return ( T_FTSYM* )( p+1 );
}
/* добавить новый символ FT7 */
T_FTSYM *   FT7_AddSymbol( T_FTTEX *p, unsigned int iU, unsigned int w, unsigned int h, const void *pbuf, unsigned int pitch )
{
    /* проверяем на пустой указатель */
    if( p == NULL )
    {
        LOGF("FT7: can't add new symbol {%u(%x)(\'%c\') %ux%u [%p]} to NULL pointer", iU, iU, iU, w, h, pbuf);
        return NULL;
    }
    /* проверяем указатель на валидность FT7 */
    if( p->magic != FT7_MAGIC )
    {
        LOGE("FT7: can't add symbol {%u(%x)(\'%c\') %ux%u [%p]} to not ft7 struct %p", iU, iU, iU, w, h, pbuf, p);
        return NULL;
    }
    /* получаем указатель на данные символов */
    T_FTSYM * ps = FT7_GetSymbols( p );
    if( ps == NULL )
    {
        LOGE("FT7: can't add symbol {%u(%x)(\'%c\') %ux%u [%p]} to ft7 %p becouse not returned symbol pointer", iU, iU, iU, w, h, pbuf, p);
        return NULL;
    }
    ps += p->count;
    /* пытаемся добавить новый символ */
    if( pbuf != NULL )
    {
        /* если не помещается в данную линию по ширине, переходим на новую линию */
        if( w + p->pX  >= p->size )
        {
            p->pX = 0;
            /* проверяем можно ли перейти на новую линию */
            if( p->pY <= p->nH )
            {
                LOGE("FT7: can't add symbol {%u(%x)(\'%c\') %ux%u [%p]} to ft7 %p becouse can't set pen to new line", iU, iU, iU, w, h, pbuf, p);
                return NULL;
            }
            p->pY -= p->nH;
            p->nH = h;
        }
        /* если не помещается в данную линию по высоте, увеличиваем линию */
        if( p->nH < h )
        {
            p->nH = h;
        }
        /* проверяем не попадём ли мы на системную память */
        if( p->pY  <= ( ( sizeof( T_FTTEX ) + ( sizeof( T_FTSYM ) * ( p->count + 1 ) ) ) / p->size ) + 1 + h )
        {
            LOGE("FT7: can't add symbol {%u(%x)(\'%c\') %ux%u [%p]} to ft7 %p becouse can't fill ft7sys mem", iU, iU, iU, w, h, pbuf, p);
            return NULL;
        }
        if( pitch == 0 )
        {
            pitch = w;
        }
        uint8_t *pD = ( uint8_t* )p + ( ( ( p->pY - h ) * p->size ) + p->pX );
        uint8_t *pS = ( uint8_t* )pbuf;

        /* копируем данные */
        for (unsigned int i = 0; i < h; ++i)
        {
            memcpy( pD + ( i * p->size ), pS + ( i * pitch ), w );
        }
    }
    /* заполняем данные о символе */
    ps->iU = iU;
    ps->tX = p->pX;
    ps->tY = p->pY - h;
    ps->tW = w;
    ps->tH = h;
    /* обновляем данные хедера */
    ++p->count;
    p->pX += w;

    LOGV("FT7: add new symbol {%u(%x)(\'%c\') %ux%u [%p]} to ft7 %p: ", iU, iU, iU, w, h, pbuf, p );
    return ps;
}
/* добавить новый символ FT7 из шрифта */
T_FTSYM *   FT7_AddSymbolByFace( T_FTTEX *p, FT_Face pF, unsigned int h, unsigned int iU )
{
    /* проверяем на пустой указатель */
    if( p == NULL )
    {
        LOGF("FT7: can't add new symbol {%u(%x)(\'%c\') %u} to NULL pointer", iU, iU, iU, h);
        return NULL;
    }
    /* проверяем указатель на валидность FT7 */
    if( p->magic != FT7_MAGIC )
    {
        LOGE("FT7: can't add symbol {%u(%x)(\'%c\') %u} to not ft7 struct %p", iU, iU, iU, h, p);
        return NULL;
    }
    /* проверяем на пустой указатель */
    if( pF == NULL )
    {
        LOGF("FT7: can't add new symbol {%u(%x)(\'%c\') %u} by NULL pointer to face", iU, iU, iU, h);
        return NULL;
    }
    FT_Set_Pixel_Sizes( pF, 0, h );
    FT_Error fterror = FT_Load_Glyph( pF, FT_Get_Char_Index( pF, iU), FT_LOAD_DEFAULT );
    if( fterror != 0 )
    {
        LOGE("FT7: can't add new symbol {%u(%x)(\'%c\') %u}", iU, iU, iU, h);
        LOGE("FT_Error: %u when load glyph", fterror);
        return NULL;
    }
    FT_GlyphSlot pG = pF->glyph;
    fterror = FT_Render_Glyph( pG, FT_RENDER_MODE_NORMAL );
    if( fterror != 0 )
    {
        LOGE("FT7: can't add new symbol {%u(%x)(\'%c\') %u}", iU, iU, iU, h);
        LOGE("FT_Error: %u when render glyph", fterror);
        return NULL;
    }
    T_FTSYM *ps = FT7_AddSymbol( g_ft7Tex, iU, pG->bitmap.width, pG->bitmap.rows, pG->bitmap.buffer, pG->bitmap.pitch );
    if( ps == NULL )
    {
        return NULL;
    }
    /* Дописываем данные о символе */
    ps->aX  = pG->bitmap_left;
    ps->aY  = pG->bitmap_top;
    ps->nA  = pG->advance.x >> 6;
    ps->nH  = h;
    return ps;
}
/* найти символ */
T_FTSYM *   FT7_GetSymbolStd( T_FTTEX *p, unsigned int h, unsigned int iU )
{
    T_FTSYM *ps = FT7_GetSymbols( p );
    if( ps == NULL ) return NULL;
    for (unsigned int i = 0; i < p->count; ++i)
    {
        if( ( ps[i].iU == iU ) && ( ps[i].nH == h ) ) return ps + i;
    }
    return NULL;
}
/* обновляет текстуру */
void IDirect3DTexture9_FT7Synhro( IDirect3DTexture9 *t, T_FTTEX *p )
{
    /* проверяем на пустой указатель */
    if( p == NULL )
    {
        LOGF("FT7: can't synhro NULL pointer");
        return;
    }
    /* проверяем указатель на валидность FT7 */
    if( p->magic != FT7_MAGIC )
    {
        LOGE("FT7: can't synhro not ft7 struct %p", p);
        return;
    }

    D3DLOCKED_RECT LRC;
    IDirect3DTexture9_LockRect( t, 0, &LRC, NULL, 0 );
    memcpy( LRC.pBits, p, p->size*p->size );
    IDirect3DTexture9_UnlockRect( t, 0 );
}
/* создаёт статический текст */
T_STEXT *   STT_Alloc( T_FTTEX *ftt, FT_Face pF, unsigned int h, FLOAT x, FLOAT y, DWORD c, const char *txt )
{
    T_STEXT *p = ( T_STEXT* )malloc( sizeof( T_FTTEX ) );

    p->count = 0;
    unsigned int sz = strlen( txt );
    for (unsigned int i = 0; i < sz; ++i)
    {
        if( isgraph( txt[ i ] ) )
        {
            ++p->count;
        }
    }

    IDirect3DDevice9_CreateVertexBuffer( g_D3DD, p->count*4*sizeof(T_VERTEX_2DT), D3DUSAGE_WRITEONLY, D3DFVF_2DT, D3DPOOL_DEFAULT, &p->D3DVB, NULL );
    {
        T_VERTEX_2DT *pV;
        IDirect3DVertexBuffer9_Lock( p->D3DVB, 0, p->count*4*sizeof(T_VERTEX_2DT), (VOID**)&pV, 0 );
        unsigned int j = 0;
        unsigned int X = 0;
        FLOAT _f = 1.0f / (FLOAT)ftt->size;
        for (unsigned int i = 0; i < sz; ++i)
        {
            T_FTSYM *ps = FT7_GetSymbolStd( ftt, h, txt[i] );
            if( ps == NULL )
            {
                ps = FT7_AddSymbolByFace( ftt, pF, h, txt[i] );
            }
            if( ps == NULL ) continue;
            if( isgraph( txt[i] ) )
            {
                pV[j*4+0].x = x + (FLOAT)( ps->aX + X );
                pV[j*4+0].y = y - (FLOAT)( ps->aY );
                pV[j*4+0].z = (FLOAT)0.5;
                pV[j*4+0].w = (FLOAT)1;
                pV[j*4+0].c = c;
                pV[j*4+0].u = (FLOAT)ps->tX * _f;
                pV[j*4+0].v = (FLOAT)ps->tY * _f;

                pV[j*4+1].x = pV[j*4+0].x + (FLOAT)(ps->tW);
                pV[j*4+1].y = pV[j*4+0].y;
                pV[j*4+1].z = (FLOAT)0.5;
                pV[j*4+1].w = (FLOAT)1;
                pV[j*4+1].c = c;
                pV[j*4+1].u = pV[j*4+0].u + (FLOAT)ps->tW * _f;
                pV[j*4+1].v = pV[j*4+0].v;

                pV[j*4+2].x = pV[j*4+0].x;
                pV[j*4+2].y = pV[j*4+0].y + (FLOAT)(ps->tH);
                pV[j*4+2].z = (FLOAT)0.5;
                pV[j*4+2].w = (FLOAT)1;
                pV[j*4+2].c = c;
                pV[j*4+2].u = pV[j*4+0].u;
                pV[j*4+2].v = pV[j*4+0].v + (FLOAT)ps->tH * _f;

                pV[j*4+3].x = pV[j*4+1].x;
                pV[j*4+3].y = pV[j*4+2].y;
                pV[j*4+3].z = (FLOAT)0.5;
                pV[j*4+3].w = (FLOAT)1;
                pV[j*4+3].c = c;
                pV[j*4+3].u = pV[j*4+1].u;
                pV[j*4+3].v = pV[j*4+2].v;

                ++j;
            }
            X += ps->nA;
        }
        IDirect3DVertexBuffer9_Unlock( p->D3DVB );
        p->count = j;
    }
    IDirect3DDevice9_CreateIndexBuffer( g_D3DD, p->count*6*sizeof(UINT16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &p->D3DIB, NULL );
    {
        UINT16 *pI;
        IDirect3DIndexBuffer9_Lock( p->D3DIB, 0, p->count*6*sizeof(UINT16), (VOID**)&pI, 0 );
        for (UINT i = 0; i < p->count; ++i)
        {
            pI[i*6+0] = i*4+0;
            pI[i*6+1] = i*4+1;
            pI[i*6+2] = i*4+2;
            pI[i*6+3] = i*4+2;
            pI[i*6+4] = i*4+1;
            pI[i*6+5] = i*4+3;
        }
        IDirect3DIndexBuffer9_Unlock( p->D3DIB );
    }
    return p;
}
/* освобождает статический текст */
void        STT_Free( T_STEXT *p )
{
    if( p->D3DIB != NULL )
        IDirect3DIndexBuffer9_Release( p->D3DIB );
    if( p->D3DVB != NULL )
        IDirect3DVertexBuffer9_Release( p->D3DVB );
    free( p );
}
/* рисует статический текст */
void        STT_Draw( T_STEXT *p )
{
    IDirect3DDevice9_SetStreamSource( g_D3DD, 0, p->D3DVB, 0, sizeof( T_VERTEX_2DT ) );
    IDirect3DDevice9_SetFVF( g_D3DD, D3DFVF_2DT );
    IDirect3DDevice9_SetIndices( g_D3DD, p->D3DIB );
    IDirect3DDevice9_DrawIndexedPrimitive( g_D3DD, D3DPT_TRIANGLELIST, 0, 0, p->count*4, 0, p->count*2 );
}
