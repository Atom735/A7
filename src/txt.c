#include "_.h"

/* Выделение места под количество текста */
T_TXT *         IDirect3DDevice9_TXT_Create( IDirect3DDevice9 *D3DD, unsigned int count, DWORD D3DFVF )
{
    T_TXT *txt    = ( T_TXT* )malloc( sizeof( T_TXT ) );
    txt->count    = 0;
    txt->cmax     = count;
    txt->D3DFVF   = D3DFVF;

    switch( txt->D3DFVF )
    {
        case D3DFVF_3DT:
            IDirect3DDevice9_CreateVertexBuffer( D3DD, count*4*sizeof(T_VERTEX_3DT), D3DUSAGE_WRITEONLY, D3DFVF_3DT, D3DPOOL_DEFAULT, &txt->D3DVB, NULL );
            break;
        case D3DFVF_2DT:
        default:
            IDirect3DDevice9_CreateVertexBuffer( D3DD, count*4*sizeof(T_VERTEX_2DT), D3DUSAGE_WRITEONLY, D3DFVF_2DT, D3DPOOL_DEFAULT, &txt->D3DVB, NULL );
            break;
    }


    return txt;
}
/* Освобождение */
void            TXT_Free( T_TXT *txt )
{
    if( txt->D3DVB != NULL )
        IDirect3DVertexBuffer9_Release( txt->D3DVB );
    free( txt );
}
/* Начать добавление */
void            TXT_BeginAdd( T_TXT *txt )
{
    txt->adding = 1;
    IDirect3DVertexBuffer9_Lock( txt->D3DVB, 0, 0, &txt->pV, 0 );

}
/* Закончить добавление */
void            TXT_EndAdd( T_TXT *txt )
{
    txt->adding = 0;
    IDirect3DVertexBuffer9_Unlock( txt->D3DVB );
}
/* Добавление квадрата */
unsigned int    TXT_AddRect( T_TXT *txt, FLOAT pX, FLOAT pY, FLOAT pZ, DWORD color, FLOAT fW, FLOAT fH, FLOAT tU0, FLOAT tV0, FLOAT tU1, FLOAT tV1 )
{
    if( txt->count >= txt->cmax ) return 0;
    if( fW == 0.0f || fH == 0.0f ) return 0;
    if( txt->adding == 0 )
    {
        TXT_BeginAdd( txt );
        txt->adding = 2;
    }
    switch( txt->D3DFVF )
    {
        case D3DFVF_3DT:
        {
            T_VERTEX_3DT *pV = ( T_VERTEX_3DT* )txt->pV + ( txt->count * 4 );
            pV->x = ( pX ); pV->y = ( pY ); pV->z = pZ; pV->c = color; pV->u = tU0; pV->v = tV0; ++pV;
            pV->x = ( pX + fW ); pV->y = ( pY ); pV->z = pZ; pV->c = color; pV->u = tU1; pV->v = tV0; ++pV;
            pV->x = ( pX ); pV->y = ( pY + fH ); pV->z = pZ; pV->c = color; pV->u = tU0; pV->v = tV1; ++pV;
            pV->x = ( pX + fW ); pV->y = ( pY + fH ); pV->z = pZ; pV->c = color; pV->u = tU1; pV->v = tV1;
            break;
        }
        case D3DFVF_2DT:
        default:
        {
            T_VERTEX_2DT *pV = ( T_VERTEX_2DT* )txt->pV + ( txt->count * 4 );
            pV->x = ( pX ); pV->y = ( pY ); pV->z = pZ; pV->w = 1.0f; pV->c = color; pV->u = tU0; pV->v = tV0; ++pV;
            pV->x = ( pX + fW ); pV->y = ( pY ); pV->z = pZ; pV->w = 1.0f; pV->c = color; pV->u = tU1; pV->v = tV0; ++pV;
            pV->x = ( pX ); pV->y = ( pY + fH ); pV->z = pZ; pV->w = 1.0f; pV->c = color; pV->u = tU0; pV->v = tV1; ++pV;
            pV->x = ( pX + fW ); pV->y = ( pY + fH ); pV->z = pZ; pV->w = 1.0f; pV->c = color; pV->u = tU1; pV->v = tV1;
            break;
        }
    }
    ++txt->count;

    if( txt->adding == 2 )
    {
        TXT_EndAdd( txt );
    }
    return txt->count;
}
/* Добавление линии символов */
unsigned int    TXT_AddFT7Line( T_TXT *txt, T_FTTEX *ft7, FLOAT pX, FLOAT pY, FLOAT pZ, DWORD color, const char *szLine, unsigned int iF, unsigned int nH )
{
    if( txt->adding == 0 )
    {
        TXT_BeginAdd( txt );
        txt->adding = 3;
    }
    FLOAT _F = 1.0f/(FLOAT)ft7->size;
    unsigned int x = 0;
    unsigned int i = 0;
    for (; *szLine; ++szLine)
    {
        T_FTSYM *ft7s = FT7_GetSymbolByFace( ft7, iF, nH, *szLine );
        if( ft7s == NULL )
        {
            ft7s = FT7_AddSymbolByFace( ft7, iF, nH, *szLine );
            if( ft7s == NULL )
                continue;
        }
        if( i == 0 )
        {
            i = TXT_AddRect( txt, pX + 0.5f + (FLOAT)x + (FLOAT)ft7s->aX, pY - 0.5f - (FLOAT)ft7s->aY, pZ, color, (FLOAT)ft7s->tW, (FLOAT)ft7s->tH, ( (FLOAT)ft7s->tX ) * _F, ( (FLOAT)ft7s->tY ) * _F, ( (FLOAT)ft7s->tX + (FLOAT)ft7s->tW ) * _F, ( (FLOAT)ft7s->tY + (FLOAT)ft7s->tH ) * _F );
        }
        else
        {
            TXT_AddRect( txt, pX + 0.5f + (FLOAT)x + (FLOAT)ft7s->aX, pY - 0.5f - (FLOAT)ft7s->aY, pZ, color, (FLOAT)ft7s->tW, (FLOAT)ft7s->tH, ( (FLOAT)ft7s->tX ) * _F, ( (FLOAT)ft7s->tY ) * _F, ( (FLOAT)ft7s->tX + (FLOAT)ft7s->tW ) * _F, ( (FLOAT)ft7s->tY + (FLOAT)ft7s->tH ) * _F );
        }
        x += ft7s->nA;
    }

    if( txt->adding == 3 )
    {
        TXT_EndAdd( txt );
    }
    return i;
}
/* Добавление символов */
void            TXT_AddFT7Symbol( T_TXT *txt, T_FTSYM *ft7s, FLOAT pX, FLOAT pY, FLOAT pZ, DWORD color, FLOAT tX, FLOAT tY, FLOAT tF )
{
    if( txt->count >= txt->cmax ) return;
    if( ft7s->tW == 0 || ft7s->tH == 0 ) return;
    if( txt->adding == 0 )
    {
        TXT_BeginAdd( txt );
        txt->adding = 2;
    }

    switch( txt->D3DFVF )
    {
        case D3DFVF_3DT:
        {
            T_VERTEX_3DT *pV;
            IDirect3DVertexBuffer9_Lock( txt->D3DVB, txt->count*4*sizeof(T_VERTEX_3DT), 4*sizeof(T_VERTEX_3DT), (VOID**)&pV, 0 );
            pV->x = ( pX + (FLOAT)ft7s->aX );
            pV->y = ( pY + (FLOAT)ft7s->aY );
            pV->z = pZ;
            pV->c = color;
            pV->u = ( (FLOAT)ft7s->tX + tX ) * tF;
            pV->v = ( (FLOAT)ft7s->tY + tY ) * tF;
            ++pV;
            pV->x = ( pX + (FLOAT)ft7s->aX + (FLOAT)ft7s->tW );
            pV->y = ( pY + (FLOAT)ft7s->aY );
            pV->z = pZ;
            pV->c = color;
            pV->u = ( (FLOAT)ft7s->tX + tX + (FLOAT)ft7s->tW ) * tF;
            pV->v = ( (FLOAT)ft7s->tY + tY ) * tF;
            ++pV;
            pV->x = ( pX + (FLOAT)ft7s->aX );
            pV->y = ( pY + (FLOAT)ft7s->aY - (FLOAT)ft7s->tH );
            pV->z = pZ;
            pV->c = color;
            pV->u = ( (FLOAT)ft7s->tX + tX ) * tF;
            pV->v = ( (FLOAT)ft7s->tY + tY + (FLOAT)ft7s->tH ) * tF;
            ++pV;
            pV->x = ( pX + (FLOAT)ft7s->aX + (FLOAT)ft7s->tW );
            pV->y = ( pY + (FLOAT)ft7s->aY - (FLOAT)ft7s->tH );
            pV->z = pZ;
            pV->c = color;
            pV->u = ( (FLOAT)ft7s->tX + tX + (FLOAT)ft7s->tW ) * tF;
            pV->v = ( (FLOAT)ft7s->tY + tY + (FLOAT)ft7s->tH ) * tF;
            IDirect3DVertexBuffer9_Unlock( txt->D3DVB );
            break;
        }
        case D3DFVF_2DT:
        default:
        {
            T_VERTEX_2DT *pV;
            IDirect3DVertexBuffer9_Lock( txt->D3DVB, txt->count*4*sizeof(T_VERTEX_2DT), 4*sizeof(T_VERTEX_2DT), (VOID**)&pV, 0 );
            pV->x = pX + (FLOAT)ft7s->aX;
            pV->y = pY - (FLOAT)ft7s->aY;
            pV->z = pZ;
            pV->w = 1.0f;
            pV->c = color;
            pV->u = ( (FLOAT)ft7s->tX + tX ) * tF;
            pV->v = ( (FLOAT)ft7s->tY + tY ) * tF;
            ++pV;
            pV->x = pX + (FLOAT)ft7s->aX + (FLOAT)ft7s->tW;
            pV->y = pY - (FLOAT)ft7s->aY;
            pV->z = pZ;
            pV->w = 1.0f;
            pV->c = color;
            pV->u = ( (FLOAT)ft7s->tX + tX + (FLOAT)ft7s->tW ) * tF;
            pV->v = ( (FLOAT)ft7s->tY + tY ) * tF;
            ++pV;
            pV->x = pX + (FLOAT)ft7s->aX;
            pV->y = pY - (FLOAT)ft7s->aY + (FLOAT)ft7s->tH;
            pV->z = pZ;
            pV->w = 1.0f;
            pV->c = color;
            pV->u = ( (FLOAT)ft7s->tX + tX ) * tF;
            pV->v = ( (FLOAT)ft7s->tY + tY + (FLOAT)ft7s->tH ) * tF;
            ++pV;
            pV->x = pX + (FLOAT)ft7s->aX + (FLOAT)ft7s->tW;
            pV->y = pY - (FLOAT)ft7s->aY + (FLOAT)ft7s->tH;
            pV->z = pZ;
            pV->w = 1.0f;
            pV->c = color;
            pV->u = ( (FLOAT)ft7s->tX + tX + (FLOAT)ft7s->tW ) * tF;
            pV->v = ( (FLOAT)ft7s->tY + tY + (FLOAT)ft7s->tH ) * tF;
            IDirect3DVertexBuffer9_Unlock( txt->D3DVB );
            break;
        }
    }

    if( txt->adding == 2 )
    {
        txt->adding = 0;
        TXT_EndAdd( txt );
    }

    ++txt->count;
}
/* Рендеринг текста */
void            IDirect3DDevice9_TXT_Render( IDirect3DDevice9 *D3DD, T_TXT *txt )
{
    switch( txt->D3DFVF )
    {
        case D3DFVF_3DT:
            IDirect3DDevice9_SetStreamSource( D3DD, 0, txt->D3DVB, 0, sizeof( T_VERTEX_3DT ) );
            IDirect3DDevice9_SetFVF( D3DD, D3DFVF_3DT );
            break;
        case D3DFVF_2DT:
        default:
            IDirect3DDevice9_SetStreamSource( D3DD, 0, txt->D3DVB, 0, sizeof( T_VERTEX_2DT ) );
            IDirect3DDevice9_SetFVF( D3DD, D3DFVF_2DT );
            break;
    }
    IDirect3DDevice9_IBQ_SetUp( D3DD, txt->count );
    IDirect3DDevice9_DrawIndexedPrimitive( D3DD, D3DPT_TRIANGLELIST, 0, 0, txt->count*4, 0, txt->count*2 );
}
