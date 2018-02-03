#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WindowsX.h>
#include <d3d9.h>
#include "log.h"

#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library  _ftLib;
FT_Face     _ftFace;
FT_GlyphSlot  slot;
int         n_points;

/* Глобальные перменные */
    LPDIRECT3D9         g_pD3D          = NULL; /* Используется для создания интерфейса устройства Direct3D9 */
    LPDIRECT3DDEVICE9   g_pd3dDevice    = NULL; /* Интерфейс устройства Direct3D9 */
    LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // Buffer to hold vertices

    // A structure for our custom vertex type
    struct CUSTOMVERTEX
    {
        FLOAT x, y, z, rhw; // The transformed position for the vertex
        DWORD color;        // The vertex color
    };

    // Our custom FVF, which describes our custom vertex structure
    #define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

/* Инициализация контекста Direct3D9 */
HRESULT InitD3D( HWND hWnd )
{
    // Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice. Most parameters are
    // zeroed out. We set Windowed to TRUE, since we want to do D3D in a
    // window, and then set the SwapEffect to "discard", which is the most
    // efficient method of presenting the back buffer to the display.  And
    // we request a back buffer format that matches the current desktop display
    // format.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    // Create the Direct3D device. Here we are using the default adapter (most
    // systems only have one, unless they have multiple graphics hardware cards
    // installed) and requesting the HAL (which is saying we want the hardware
    // device rather than a software one). Software vertex processing is
    // specified since we know it will work on all cards. On cards that support
    // hardware vertex processing, though, we would see a big performance gain
    // by specifying hardware vertex processing.
    if( FAILED( IDirect3D9_CreateDevice( g_pD3D, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

    // Device state would normally be set here

    return S_OK;
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
HRESULT InitVB()
{

    if( slot->format == FT_GLYPH_FORMAT_OUTLINE ) {
        LOGI( "FT_GLYPH_FORMAT_OUTLINE" );
        LOGI( "n_contours: %hd", slot->outline.n_contours );
        LOGI( "n_points: %hd", slot->outline.n_points );
        n_points = slot->outline.n_points;
        if( FAILED( IDirect3DDevice9_CreateVertexBuffer( g_pd3dDevice, n_points * sizeof( struct CUSTOMVERTEX ),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
        {
            return E_FAIL;
        }
        VOID* pVertices;
        if( FAILED( IDirect3DVertexBuffer9_Lock( g_pVB, 0, n_points * sizeof( struct CUSTOMVERTEX ), ( void** )&pVertices, 0 ) ) )
            return E_FAIL;
        struct CUSTOMVERTEX *pV = (struct CUSTOMVERTEX *)pVertices;
        for (int i = 0; i < n_points; ++i)
        {
            pV[i].x = (FLOAT)slot->outline.points[i].x;
            pV[i].y = (FLOAT)slot->outline.points[i].y;
            pV[i].z = 0.5f;
            pV[i].rhw = 1.0f;
            pV[i].color = 0xff7f7f7f;
        }
        IDirect3DVertexBuffer9_Unlock( g_pVB );

        return S_OK;

    }
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{

    if( g_pVB != NULL )
        IDirect3DVertexBuffer9_Release( g_pVB );

    if( g_pd3dDevice != NULL )
        IDirect3DDevice9_Release( g_pd3dDevice );

    if( g_pD3D != NULL )
        IDirect3D9_Release( g_pD3D );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    if( NULL == g_pd3dDevice )
        return;

    // Clear the backbuffer to a blue color
    IDirect3DDevice9_Clear( g_pd3dDevice, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

    // Begin the scene
    if( SUCCEEDED( IDirect3DDevice9_BeginScene( g_pd3dDevice ) ) )
    {
        // Rendering of scene objects can happen here
        // Draw the triangles in the vertex buffer. This is broken into a few
        // steps. We are passing the vertices down a "stream", so first we need
        // to specify the source of that stream, which is our vertex buffer. Then
        // we need to let D3D know what vertex shader to use. Full, custom vertex
        // shaders are an advanced topic, but in most cases the vertex shader is
        // just the FVF, so that D3D knows what type of vertices we are dealing
        // with. Finally, we call DrawPrimitive() which does the actual rendering
        // of our geometry (in this case, just one triangle).
        IDirect3DDevice9_SetStreamSource( g_pd3dDevice, 0, g_pVB, 0, sizeof( struct CUSTOMVERTEX ) );
        IDirect3DDevice9_SetFVF( g_pd3dDevice, D3DFVF_CUSTOMVERTEX );
        int b = 0;
        for (int i = 0; i < slot->outline.n_contours; ++i)
        {
            IDirect3DDevice9_DrawPrimitive( g_pd3dDevice, D3DPT_LINESTRIP, b, slot->outline.contours[i] -b );
            b = slot->outline.contours[i]+1;
            /* code */
        }

        // End the scene
        IDirect3DDevice9_EndScene( g_pd3dDevice );
    }

    // Present the backbuffer contents to the display
    IDirect3DDevice9_Present( g_pd3dDevice, NULL, NULL, NULL, NULL );
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            Cleanup();
            PostQuitMessage( 0 );
            return 0;

        case WM_PAINT:
            Render();
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

    FT_Init_FreeType( &_ftLib );
    FT_New_Face( _ftLib, "font.ttf", 0, &_ftFace );
    // FT_Set_Char_Size( _ftFace, 0, 12*64, 96, 96 );
    FT_Set_Pixel_Sizes( _ftFace, 0, 48 );
    ;

    FT_Load_Glyph( _ftFace, FT_Get_Char_Index( _ftFace, L'О' ), FT_LOAD_NO_SCALE );
    slot = _ftFace->glyph;  /* a small shortcut */

    if(slot->format == FT_GLYPH_FORMAT_NONE)
        LOG("FT_GLYPH_FORMAT_NONE");
    if(slot->format == FT_GLYPH_FORMAT_COMPOSITE)
        LOG("FT_GLYPH_FORMAT_COMPOSITE");
    if(slot->format == FT_GLYPH_FORMAT_BITMAP)
        LOG("FT_GLYPH_FORMAT_BITMAP");
    if(slot->format == FT_GLYPH_FORMAT_OUTLINE)
        LOG("FT_GLYPH_FORMAT_OUTLINE");
    if(slot->format == FT_GLYPH_FORMAT_PLOTTER)
        LOG("FT_GLYPH_FORMAT_PLOTTER");




    // Initialize Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        // Create the vertex buffer
        if( SUCCEEDED( InitVB() ) )
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
                else
                    Render();
            }
        }
    }


    FT_Done_Face( _ftFace );
    FT_Done_FreeType( _ftLib );

    UnregisterClass( L"D3D Tutorial", wc.hInstance );
    return 0;
}



