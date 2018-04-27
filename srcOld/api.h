#ifndef _H_API
#define _H_API

/* Объявления фукнций */
    /* пересоздаёт буффер индексов для квадратов */
    VOID        IDirect3DDevice9_IBQ_ReCount( IDirect3DDevice9 *D3DD, UINT count );
    /* прекрипляет буффер индексов для квадратов */
    VOID        IDirect3DDevice9_IBQ_SetUp( IDirect3DDevice9 *D3DD, UINT count );
    /* Получить название формата D3D9 */
    const char * GetName_D3DFMT( D3DFORMAT fmt );
    /* устанавливает 2Д проектор и вьюпорт */
    VOID        IDirect3DDevice9_SetVPP2D( IDirect3DDevice9 *D3DD, D3DVIEWPORT9 *pD3DVP );


#endif /* _H_API */
