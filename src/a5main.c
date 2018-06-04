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
BOOL            g_bMouseDownLeft    = FALSE;
INT             g_iMouseDownX       = 0;
INT             g_iMouseDownY       = 0;
INT             g_iMouseX           = 0;
INT             g_iMouseY           = 0;
INT             g_iMouseDirectX     = 0;
INT             g_iMouseDirectY     = 0;

INT             g_nDD               = 1;
INT             g_nDDOld            = 1;

D3DFILLMODE     g_eD3DFillMode      = D3DFILL_POINT;

extern CONST LPCSTR g_a5TabWM[];

typedef struct VERTEX_PN
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
} VERTEX_PN;
#define D3DFVF_VERTEX_PN    (D3DFVF_XYZ|D3DFVF_NORMAL)

typedef struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The 3D position for the vertex
    D3DXVECTOR3 normal;   // The surface normal for the vertex
} CUSTOMVERTEX;
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)


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

VOID rRestartRender()
{
    if( g_pD3DVB != NULL )
    {
        IDirect3DVertexBuffer9_Release( g_pD3DVB );
        g_pD3DVB = NULL;
    }
    if( FAILED( IDirect3DDevice9_CreateVertexBuffer( g_pD3DDev, g_nDDOld * g_nDDOld * 20 * 3 * sizeof(VERTEX_PN), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pD3DVB, NULL ) ) )
    {
        LOGDBG( L"Не смогли создать буффер вершин" );
    }
    VERTEX_PN* pV;
    if( FAILED( IDirect3DVertexBuffer9_Lock( g_pD3DVB, 0, 0, (VOID**)&pV, 0 ) ) )
    {
        LOGDBG( L"Не смогли заблокироать буффер вершин" );
    }
    else
    {
        D3DXVECTOR3* pB = malloc( sizeof( D3DXVECTOR3 ) * ( ( g_nDDOld + 1 ) * ( g_nDDOld + 2 ) / 2 ) );
        if( pB == NULL )
        {
            LOGDBG( L"Не смогли создать доп буффер вершин" );
        }
        else
        {
            FLOAT kfP0x, kfP0y, kfP0z, kfP1x, kfP1y, kfP1z, kfP2x, kfP2y, kfP2z;
            DWORD dwN;

            VOID _Func()
            {
                dwN = 0;
                for( DWORD j = 0; j <= g_nDDOld; ++j )
                {
                    for( DWORD k = 0; k <= j; ++k )
                    {
                        CONST FLOAT f0 = (FLOAT)(g_nDDOld-j);
                        CONST FLOAT f1 = (FLOAT)(j-k);
                        CONST FLOAT f2 = (FLOAT)(k);
                        CONST FLOAT FFx = ( ( kfP0x * f0 ) + ( kfP1x * f1 ) + ( kfP2x * f2 ) );
                        CONST FLOAT FFy = ( ( kfP0y * f0 ) + ( kfP1y * f1 ) + ( kfP2y * f2 ) );
                        CONST FLOAT FFz = ( ( kfP0z * f0 ) + ( kfP1z * f1 ) + ( kfP2z * f2 ) );
                        CONST FLOAT FFL = (sqrtf(5.0f) * 0.5f) / sqrtf( (FFx*FFx) + (FFy*FFy) + (FFz*FFz) );
                        pB[dwN].x = FFx * FFL;
                        pB[dwN].y = FFy * FFL;
                        pB[dwN].z = FFz * FFL;
                        if( j > 0 )
                        {
                            if( k > 0 )
                            {
                                pV[0].p = pB[dwN-1-j];
                                pV[1].p = pB[dwN-1];
                                pV[2].p = pB[dwN];

                                CONST FLOAT Ax = pV[1].p.x - pV[0].p.x;
                                CONST FLOAT Ay = pV[1].p.y - pV[0].p.y;
                                CONST FLOAT Az = pV[1].p.z - pV[0].p.z;

                                CONST FLOAT Bx = pV[2].p.x - pV[0].p.x;
                                CONST FLOAT By = pV[2].p.y - pV[0].p.y;
                                CONST FLOAT Bz = pV[2].p.z - pV[0].p.z;

                                pV[0].n.x = ( Ay * Bz ) - ( Az * By );
                                pV[0].n.y = ( Az * Bx ) - ( Ax * Bz );
                                pV[0].n.z = ( Ax * By ) - ( Ay * Bx );

                                D3DXVec3Normalize( &(pV[0].n), &(pV[0].n) );

                                pV[2].n = pV[1].n = pV[0].n;
                                pV+=3;
                            }
                            if( k > 1 )
                            {
                                pV[0].p = pB[dwN-1-j];
                                pV[1].p = pB[dwN-2-j];
                                pV[2].p = pB[dwN-1];

                                CONST FLOAT Ax = pV[1].p.x - pV[0].p.x;
                                CONST FLOAT Ay = pV[1].p.y - pV[0].p.y;
                                CONST FLOAT Az = pV[1].p.z - pV[0].p.z;

                                CONST FLOAT Bx = pV[2].p.x - pV[0].p.x;
                                CONST FLOAT By = pV[2].p.y - pV[0].p.y;
                                CONST FLOAT Bz = pV[2].p.z - pV[0].p.z;

                                pV[0].n.x = ( Ay * Bz ) - ( Az * By );
                                pV[0].n.y = ( Az * Bx ) - ( Ax * Bz );
                                pV[0].n.z = ( Ax * By ) - ( Ay * Bx );

                                D3DXVec3Normalize( &(pV[0].n), &(pV[0].n) );

                                pV[2].n = pV[1].n = pV[0].n;
                                pV+=3;
                            }
                        }
                        ++dwN;
                    }
                }
            }

            for( DWORD i = 0; i<5; ++i )
            {
                kfP0x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );
                kfP0y = 0.5f;
                kfP0z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );
                kfP1x = cosf( ( D3DX_PI * 0.4f * i ) );
                kfP1y = 0.5f;
                kfP1z = sinf( ( D3DX_PI * 0.4f * i ) );
                kfP2x = 0.0f;
                kfP2y = sqrtf(5.0f) * 0.5f;
                kfP2z = 0.0f;
                _Func();
            }
            for( DWORD i = 0; i<5; ++i )
            {
                kfP0x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );
                kfP0y = 0.5f;
                kfP0z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );
                kfP1x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );
                kfP1y =-0.5f;
                kfP1z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );
                kfP2x = cosf( ( D3DX_PI * 0.4f * i ) );
                kfP2y = 0.5f;
                kfP2z = sinf( ( D3DX_PI * 0.4f * i ) );
                _Func();
            }
            for( DWORD i = 0; i<5; ++i )
            {
                kfP0x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );
                kfP0y =-0.5f;
                kfP0z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );
                kfP1x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );
                kfP1y = 0.5f;
                kfP1z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );
                kfP2x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.6f ) );
                kfP2y =-0.5f;
                kfP2z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.6f ) );
                _Func();
            }
            for( DWORD i = 0; i<5; ++i )
            {
                kfP0x = 0.0f;
                kfP0y =-sqrtf(5.0f) * 0.5f;
                kfP0z = 0.0f;
                kfP1x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );
                kfP1y =-0.5f;
                kfP1z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );
                kfP2x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.6f ) );
                kfP2y =-0.5f;
                kfP2z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.6f ) );
                _Func();
            }
            free( pB );
            pB = NULL;
        }
        IDirect3DVertexBuffer9_Unlock( g_pD3DVB );
    }
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
        d3dpp.EnableAutoDepthStencil = TRUE;
        d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

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

        // IDirect3DDevice9_SetRenderState( g_pD3DDev, D3DRS_CULLMODE, D3DCULL_NONE );
        // IDirect3DDevice9_SetRenderState( g_pD3DDev, D3DRS_LIGHTING, FALSE );
        IDirect3DDevice9_SetRenderState( g_pD3DDev, D3DRS_ZENABLE, TRUE );

    }

    {
        if( FAILED( IDirect3DDevice9_CreateVertexBuffer( g_pD3DDev, 20 * 3 * sizeof(VERTEX_PN), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pD3DVB, NULL ) ) )
        {
            LOGDBG( L"Не смогли создать буффер вершин" );
        }
        VERTEX_PN* pV;
        if( FAILED( IDirect3DVertexBuffer9_Lock( g_pD3DVB, 0, 0, (VOID**)&pV, 0 ) ) )
        {
            LOGDBG( L"Не смогли заблокироать буффер вершин" );
        }
        else
        {
            // 1. Список вершин:
            // x(1)=sqrt(5)/2, y(1)=0, z(1)=0
            // x(12)=-sqrt(5)/2, y(12)=0, z(12)=0
            // x(j+2)=cos(j*72*pi/360),
            // y(j+2)=sin(j*72*pi/360),
            // z(j+2)=0.5, где j=0..4
            // x(j+7)=cos((36+j*72)*pi/360),
            // y(j+7)=sin((36+j*72)*pi/360),
            // z(j+7)=-0.5, где j=0..4
            // 2. Список граней:
            // (1,2,3),(1,3,4),(1,4,5),(1,5,6),(1,6,2),(11,12,7)
            // (12,8,7),(12,9,8),(12,10,9),(12,11,10),(7,2,11),(3,7,8)
            // (4,8,9),(5,9,10),(6,10,11),(7,3,2),(8,4,3),(9,5,4)
            // (10,6,5),(11,2,6)

            for( DWORD i = 0; i<5; ++i )
            {
                pV[0].p.x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );
                pV[0].p.y = 0.5f;
                pV[0].p.z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );

                pV[1].p.x = cosf( ( D3DX_PI * 0.4f * i ) );
                pV[1].p.y = 0.5f;
                pV[1].p.z = sinf( ( D3DX_PI * 0.4f * i ) );

                pV[2].p.x = 0.0f;
                pV[2].p.y = sqrtf(5.0f) * 0.5f;
                pV[2].p.z = 0.0f;


                CONST FLOAT Ax = pV[1].p.x - pV[0].p.x;
                CONST FLOAT Ay = pV[1].p.y - pV[0].p.y;
                CONST FLOAT Az = pV[1].p.z - pV[0].p.z;

                CONST FLOAT Bx = pV[2].p.x - pV[0].p.x;
                CONST FLOAT By = pV[2].p.y - pV[0].p.y;
                CONST FLOAT Bz = pV[2].p.z - pV[0].p.z;

                pV[0].n.x = ( Ay * Bz ) - ( Az * By );
                pV[0].n.y = ( Az * Bx ) - ( Ax * Bz );
                pV[0].n.z = ( Ax * By ) - ( Ay * Bx );

                D3DXVec3Normalize( &(pV[0].n), &(pV[0].n) );

                pV[2].n = pV[1].n = pV[0].n;
                pV+=3;
            }

            for( DWORD i = 0; i<5; ++i )
            {
                pV[0].p.x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );
                pV[0].p.y = 0.5f;
                pV[0].p.z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );

                pV[1].p.x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );
                pV[1].p.y =-0.5f;
                pV[1].p.z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );

                pV[2].p.x = cosf( ( D3DX_PI * 0.4f * i ) );
                pV[2].p.y = 0.5f;
                pV[2].p.z = sinf( ( D3DX_PI * 0.4f * i ) );


                CONST FLOAT Ax = pV[1].p.x - pV[0].p.x;
                CONST FLOAT Ay = pV[1].p.y - pV[0].p.y;
                CONST FLOAT Az = pV[1].p.z - pV[0].p.z;

                CONST FLOAT Bx = pV[2].p.x - pV[0].p.x;
                CONST FLOAT By = pV[2].p.y - pV[0].p.y;
                CONST FLOAT Bz = pV[2].p.z - pV[0].p.z;

                pV[0].n.x = ( Ay * Bz ) - ( Az * By );
                pV[0].n.y = ( Az * Bx ) - ( Ax * Bz );
                pV[0].n.z = ( Ax * By ) - ( Ay * Bx );

                D3DXVec3Normalize( &(pV[0].n), &(pV[0].n) );

                pV[2].n = pV[1].n = pV[0].n;
                pV+=3;
            }

            for( DWORD i = 0; i<5; ++i )
            {
                pV[0].p.x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );
                pV[0].p.y =-0.5f;
                pV[0].p.z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );

                pV[1].p.x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );
                pV[1].p.y = 0.5f;
                pV[1].p.z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.4f ) );

                pV[2].p.x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.6f ) );
                pV[2].p.y =-0.5f;
                pV[2].p.z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.6f ) );

                CONST FLOAT Ax = pV[1].p.x - pV[0].p.x;
                CONST FLOAT Ay = pV[1].p.y - pV[0].p.y;
                CONST FLOAT Az = pV[1].p.z - pV[0].p.z;

                CONST FLOAT Bx = pV[2].p.x - pV[0].p.x;
                CONST FLOAT By = pV[2].p.y - pV[0].p.y;
                CONST FLOAT Bz = pV[2].p.z - pV[0].p.z;

                pV[0].n.x = ( Ay * Bz ) - ( Az * By );
                pV[0].n.y = ( Az * Bx ) - ( Ax * Bz );
                pV[0].n.z = ( Ax * By ) - ( Ay * Bx );

                D3DXVec3Normalize( &(pV[0].n), &(pV[0].n) );

                pV[2].n = pV[1].n = pV[0].n;
                pV+=3;
            }

            for( DWORD i = 0; i<5; ++i )
            {
                pV[0].p.x = 0.0f;
                pV[0].p.y =-sqrtf(5.0f) * 0.5f;
                pV[0].p.z = 0.0f;

                pV[1].p.x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );
                pV[1].p.y =-0.5f;
                pV[1].p.z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.2f ) );

                pV[2].p.x = cosf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.6f ) );
                pV[2].p.y =-0.5f;
                pV[2].p.z = sinf( ( D3DX_PI * 0.4f * i ) + ( D3DX_PI * 0.6f ) );

                CONST FLOAT Ax = pV[1].p.x - pV[0].p.x;
                CONST FLOAT Ay = pV[1].p.y - pV[0].p.y;
                CONST FLOAT Az = pV[1].p.z - pV[0].p.z;

                CONST FLOAT Bx = pV[2].p.x - pV[0].p.x;
                CONST FLOAT By = pV[2].p.y - pV[0].p.y;
                CONST FLOAT Bz = pV[2].p.z - pV[0].p.z;

                pV[0].n.x = ( Ay * Bz ) - ( Az * By );
                pV[0].n.y = ( Az * Bx ) - ( Ax * Bz );
                pV[0].n.z = ( Ax * By ) - ( Ay * Bx );

                D3DXVec3Normalize( &(pV[0].n), &(pV[0].n) );

                pV[2].n = pV[1].n = pV[0].n;
                pV+=3;
            }


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
        i/=2;
        if( g_nDD != g_nDDOld )
        {
            g_nDDOld = g_nDD;
            rRestartRender();
        }
        IDirect3DDevice9_Clear( g_pD3DDev, 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB( 0, 0, i+0x3f ), 1.0f, 0 );
        if( SUCCEEDED( IDirect3DDevice9_BeginScene( g_pD3DDev ) ) )
        {
            IDirect3DDevice9_SetRenderState( g_pD3DDev, D3DRS_FILLMODE, g_eD3DFillMode );

            {
                D3DMATERIAL9 mtrl;
                ZeroMemory( &mtrl, sizeof( D3DMATERIAL9 ) );
                mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
                mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
                mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
                mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
                IDirect3DDevice9_SetMaterial( g_pD3DDev, &mtrl );

                D3DXVECTOR3 vecDir;
                D3DLIGHT9 light;
                ZeroMemory( &light, sizeof( D3DLIGHT9 ) );
                light.Type = D3DLIGHT_DIRECTIONAL;
                light.Diffuse.r = 1.0f;
                light.Diffuse.g = 1.0f;
                light.Diffuse.b = 1.0f;
                vecDir.x = cosf( dwTickNew / 3500.0f );
                vecDir.y = 1.0f;
                vecDir.z = sinf( dwTickNew / 3500.0f );
                D3DXVec3Normalize( ( D3DXVECTOR3* )&light.Direction, &vecDir );
                light.Range = 1000.0f;
                IDirect3DDevice9_SetLight( g_pD3DDev, 0, &light );
                IDirect3DDevice9_LightEnable( g_pD3DDev, 0, TRUE );
                IDirect3DDevice9_SetRenderState( g_pD3DDev, D3DRS_LIGHTING, TRUE );

                IDirect3DDevice9_SetRenderState( g_pD3DDev, D3DRS_AMBIENT, 0x00202020 );
            }

            {
                D3DXMATRIX matWorld;

                UINT iTime = dwTickNew % 100000;
                FLOAT fAngle = iTime * ( 2.0f * D3DX_PI ) / 100000.0f;
                D3DXMatrixRotationY( &matWorld, fAngle );
                IDirect3DDevice9_SetTransform( g_pD3DDev, D3DTS_WORLD, &matWorld );

                static FLOAT fX = 0.0f;
                static FLOAT fY = D3DX_PI * 0.1f;
                fX += ((FLOAT)g_iMouseDirectX * (FLOAT)dwTickDelta) * 0.00001f;
                fY -= ((FLOAT)g_iMouseDirectY * (FLOAT)dwTickDelta) * 0.00001f;
                if(fY > D3DX_PI * 0.45f) fY = D3DX_PI * 0.45f;
                if(fY <-D3DX_PI * 0.45f) fY =-D3DX_PI * 0.45f;
                while(fX > D3DX_PI * 2.0f) fX -= D3DX_PI * 2.0f;
                while(fX < 0.0f) fX += D3DX_PI * 2.0f;
                D3DXVECTOR3 vEyePt;
                vEyePt.x = cosf(fY) * sinf(fX) * 5.0f;
                vEyePt.y = sinf(fY) * 5.0f;
                vEyePt.z =-cosf(fY) * cosf(fX) * 5.0f;
                D3DXVECTOR3 vLookatPt = { 0.0f, 0.0f, 0.0f };
                D3DXVECTOR3 vUpVec = { 0.0f, 1.0f, 0.0f };
                D3DXMATRIX matView;
                D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
                IDirect3DDevice9_SetTransform( g_pD3DDev, D3DTS_VIEW, &matView );

                D3DXMATRIX matProj;
                D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI / 4, 1.0f, 1.0f, 100.0f );
                IDirect3DDevice9_SetTransform( g_pD3DDev, D3DTS_PROJECTION, &matProj );
            }


            IDirect3DDevice9_SetStreamSource( g_pD3DDev, 0, g_pD3DVB, 0, sizeof( VERTEX_PN ) );
            IDirect3DDevice9_SetFVF( g_pD3DDev, D3DFVF_VERTEX_PN );
            IDirect3DDevice9_DrawPrimitive( g_pD3DDev, D3DPT_TRIANGLELIST, 0, g_nDDOld * g_nDDOld * 20 );

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
        case WM_MOUSEMOVE:
        {
            if( g_bMouseDownLeft )
            {
                g_iMouseX = GET_X_LPARAM( lParam );
                g_iMouseY = GET_Y_LPARAM( lParam );
                g_iMouseDirectX = g_iMouseX - g_iMouseDownX;
                g_iMouseDirectY = g_iMouseY - g_iMouseDownY;
            }
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            g_iMouseDownX = g_iMouseX = GET_X_LPARAM( lParam );
            g_iMouseDownY = g_iMouseY = GET_Y_LPARAM( lParam );
            g_iMouseDirectX = 0;
            g_iMouseDirectY = 0;
            g_bMouseDownLeft = TRUE;
            return 0;
        }
        case WM_LBUTTONUP:
        {
            g_iMouseX = GET_X_LPARAM( lParam );
            g_iMouseY = GET_Y_LPARAM( lParam );
            // g_iMouseDirectX = 0;
            // g_iMouseDirectY = 0;
            g_bMouseDownLeft = FALSE;
            return 0;
        }
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
                case VK_F1:
                {
                    g_eD3DFillMode = D3DFILL_POINT;
                    return 0;
                }
                case VK_F2:
                {
                    g_eD3DFillMode = D3DFILL_WIREFRAME;
                    return 0;
                }
                case VK_F3:
                {
                    g_eD3DFillMode = D3DFILL_SOLID;
                    return 0;
                }
                case 'A':
                {
                    ++g_nDD;
                    return 0;
                }
                case 'Z':
                {
                    if( g_nDD > 2) --g_nDD;
                    if( g_nDD < 2) g_nDD = 2;
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
