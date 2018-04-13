#include "_.h"

/* Глобальные перменные */
    /* Буффер индексов вершин кадратов */
    IDirect3DIndexBuffer9      *g_IBQ_D3DIB     = NULL;
    /* Максимальное количество рисуемых квадратов этим буффером */
    UINT                        g_IBQ_count     = 0;

/* пересоздаёт буффер индексов для квадратов */
VOID        IDirect3DDevice9_IBQ_ReCount( IDirect3DDevice9 *D3DD, UINT count )
{
    /* происходит удаление буффера */
    if( count == 0 )
    {
        if( g_IBQ_D3DIB != NULL )
            IDirect3DIndexBuffer9_Release( g_IBQ_D3DIB );
        g_IBQ_D3DIB = NULL;
        g_IBQ_count = 0;
        LOGD("IBQ Free");
    }
    else
    {
        /* пересоздаём буффер */
        if( g_IBQ_D3DIB != NULL )
            IDirect3DIndexBuffer9_Release( g_IBQ_D3DIB );
        IDirect3DDevice9_CreateIndexBuffer( D3DD, count*6*sizeof(UINT16), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &g_IBQ_D3DIB, NULL );
        UINT16 *pI;
        IDirect3DIndexBuffer9_Lock( g_IBQ_D3DIB, 0, count*6*sizeof(UINT16), (VOID**)&pI, 0 );
        for (UINT i = 0; i < count; ++i)
        {
            CONST UINT ii = i*4;
            *pI = ii+0; ++pI;
            *pI = ii+1; ++pI;
            *pI = ii+2; ++pI;
            *pI = ii+2; ++pI;
            *pI = ii+1; ++pI;
            *pI = ii+3; ++pI;
        }
        IDirect3DIndexBuffer9_Unlock( g_IBQ_D3DIB );
        LOGD("IBQ ReCounted from %u to %u", g_IBQ_count, count);
        g_IBQ_count = count;
    }
}
/* прекрипляет буффер индексов для квадратов */
VOID        IDirect3DDevice9_IBQ_SetUp( IDirect3DDevice9 *D3DD, UINT count )
{
    /* проверяем на недостаточность */
    if( count > g_IBQ_count )
        IDirect3DDevice9_IBQ_ReCount( D3DD, count );
    IDirect3DDevice9_SetIndices( D3DD, g_IBQ_D3DIB );
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
/* устанавливает 2Д проектор и вьюпорт */
VOID        IDirect3DDevice9_SetVPP2D( IDirect3DDevice9 *D3DD, D3DVIEWPORT9 *pD3DVP )
{
    IDirect3DDevice9_SetViewport( D3DD, pD3DVP );
    D3DMATRIX m = {
        ._11 = 2.0f/(FLOAT)pD3DVP->Width, ._12 = 0.0f, ._13 = 0.0f, ._14 = 0.0f,
        ._21 = 0.0f, ._22 =-2.0f/(FLOAT)pD3DVP->Height, ._23 = 0.0f, ._24 = 0.0f,
        ._31 = 0.0f, ._32 = 0.0f, ._33 = 1.0f, ._34 = 0.0f,
        ._41 =-1.0f, ._42 = 1.0f, ._43 = 0.0f, ._44 = 1.0f,
        };
    IDirect3DDevice9_SetTransform( D3DD, D3DTS_PROJECTION, &m );
}
