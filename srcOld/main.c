#include "_.h"


/* Структуры */


/* Константы */
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
    /* Текстура */
    IDirect3DTexture9          *g_D3DTEX    = NULL;
    /* Порт просмотра */
    D3DVIEWPORT9                g_D3DVP;

    /* Данные фонта */
    T_FTTEX                    *g_ft7Tex;
    /* Позиции мыши */
    int                         g_iMouseX, g_iMouseY;
    int                         g_iMouse0   = 1;

    T_TXT                      *g_txt3D;
    T_TXT                      *g_txt;

void testStrings()
{
    FLOAT X = 16.0f;
    FLOAT Y = 16.0f;
    int Sz = 18;
    FLOAT H = Sz+Y;
    g_txt->count = 0;
    g_txt3D->count = 0;

    FLOAT __f = 0.5f/(FLOAT)g_ft7Tex->size;

    TXT_AddRect( g_txt, X, H-Sz, 0.0f, 0xaf2f2f2f, 512.0f, Sz*2, __f, __f, __f, __f );
    TXT_AddFT7Line( g_txt  , g_ft7Tex, X, H, 0.5f, 0x7fff7f00, "2D !\"#$%&'()*+,-./0123456789:;<=>?", 1, Sz ); H += Sz;
    TXT_AddFT7Line( g_txt  , g_ft7Tex, X, H, 0.5f, 0x7fff7f00, "Normal 2D line", 1, Sz ); H += Sz;
    H += Y;
    TXT_AddRect( g_txt3D, X, H-Sz, 0.0f, 0xaf2f2f2f, 512.0f, Sz*2, __f, __f, __f, __f );
    TXT_AddFT7Line( g_txt3D, g_ft7Tex, X, H, 0.5f, 0x7f007fff, "3D !\"#$%&'()*+,-./0123456789:;<=>?", 1, Sz ); H += Sz;
    TXT_AddFT7Line( g_txt3D, g_ft7Tex, X, H, 0.5f, 0x7f007fff, "Normal 3D line", 1, Sz ); H += Sz;

}

/* Инициализация ресурсов */
VOID    D3D_ResInit()
{
    FT7_Init();
    FT7_FaceNewByFile( "font.ttf", 0 );
    g_ft7Tex    = FT7_Alloc( 1024 );
    g_txt       = IDirect3DDevice9_TXT_Create( g_D3DD, 4096, D3DFVF_2DT );
    g_txt3D     = IDirect3DDevice9_TXT_Create( g_D3DD, 4096, D3DFVF_3DT );
    testStrings();

    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_LIGHTING, FALSE );
    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_ALPHABLENDENABLE, TRUE );
    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
    IDirect3DDevice9_SetRenderState( g_D3DD, D3DRS_BLENDOP, D3DBLENDOP_ADD );

    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    IDirect3DDevice9_SetTextureStageState( g_D3DD, 0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE );

    IDirect3DDevice9_SetSamplerState( g_D3DD, 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE );
    IDirect3DDevice9_SetSamplerState( g_D3DD, 0, D3DSAMP_MINFILTER, D3DTEXF_NONE );
    IDirect3DDevice9_SetSamplerState( g_D3DD, 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

    IDirect3DDevice9_FT7_CreateTexture( g_D3DD, g_ft7Tex, &g_D3DTEX );

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

    TXT_Free( g_txt );
    TXT_Free( g_txt3D );
    IDirect3DDevice9_IBQ_ReCount( g_D3DD, 0 );

    FT7_Free( g_ft7Tex );
    FT7_FaceFree( 1 );
    FT7_Release();
    LOGI("resources free!");
}
/* Функция отрисовки */
VOID    D3D_Render()
{
    IDirect3DDevice9_Clear( g_D3DD, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0,0x10,0x50,0x90), 0.0f, 0);
    IDirect3DDevice9_BeginScene( g_D3DD );
    IDirect3DDevice9_SetStreamSource( g_D3DD, 0, g_D3DVB, 0, sizeof( T_VERTEX_2DT ) );
    IDirect3DDevice9_SetFVF( g_D3DD, D3DFVF_2DT );



    IDirect3DDevice9_SetTexture( g_D3DD, 0, (IDirect3DBaseTexture9*)g_D3DTEX );
    IDirect3DDevice9_TXT_Render( g_D3DD, g_txt );
    IDirect3DDevice9_TXT_Render( g_D3DD, g_txt3D );

    IDirect3DDevice9_EndScene( g_D3DD );
    IDirect3DDevice9_Present( g_D3DD, (RECT*)&g_D3DVP, NULL, NULL, NULL );
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
        /* Обработка нажатий клавиш */
        case WM_KEYDOWN: {
            if( wParam >= '0' && wParam <= '8' )
            {
                IDirect3DDevice9_SetSamplerState( g_D3DD, 0, D3DSAMP_MAGFILTER, D3DTEXF_NONE+wParam-'0' );
                IDirect3DDevice9_SetSamplerState( g_D3DD, 0, D3DSAMP_MINFILTER, D3DTEXF_NONE+wParam-'0' );
                IDirect3DDevice9_SetSamplerState( g_D3DD, 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE+wParam-'0' );
            }
            return 0;
        }

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
        {
            g_iMouseX = GET_X_LPARAM(lParam);
            g_iMouseY = GET_Y_LPARAM(lParam);

            testStrings();

            return 0;
        }
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
            IDirect3DDevice9_SetVPP2D( g_D3DD, &g_D3DVP );
            testStrings();
            D3D_Render();
            LOGI("WM_SIZE: %ux%u", g_D3DVP.Width, g_D3DVP.Height);

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
