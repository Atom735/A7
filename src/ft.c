#include "_.h"


/* Глобальные перменные */
    #define FT7_MAXFACES 64
    /* Библиотека FreeType */
    FT_Library                  g_ftLib     = NULL;
    /* Фонт фейс FreeType */
    FT_Face                     g_ftFaces[FT7_MAXFACES];

/* инициализация FT7 */
void            FT7_Init()
{
    if( g_ftLib == NULL )
    {
        FT_Init_FreeType( &g_ftLib );
        LOGD("%s: FT_Lib[%p] init", __FUNCTION__, g_ftLib);
        for (unsigned int i = 0; i < FT7_MAXFACES; ++i)
        {
            g_ftFaces[i] = NULL;
        }
    }
    else
    {
        LOGE("%s: FT_Lib[%p] is init", __FUNCTION__, g_ftLib);
    }
}
/* освобождение FT7 */
void            FT7_Release()
{
    if( g_ftLib != NULL )
    {
        for (unsigned int i = 0; i < FT7_MAXFACES; ++i)
        {
            if( g_ftFaces[i] != NULL )
            {
                LOGW("%s: FTFace[(%d/%d), %p] is not freed before done FT_Lib[%p]", __FUNCTION__, i+1, FT7_MAXFACES, g_ftFaces[i], g_ftLib);
                FT7_FaceFree( i+1 );
            }
        }
        LOGD("%s: FT_Lib[%p] done", __FUNCTION__, g_ftLib);
        FT_Done_FreeType( g_ftLib );
    }
    else
    {
        LOGE("%s: FT_Lib[%p] is not init for done", __FUNCTION__, g_ftLib);
    }
}
/* выделение память под FT7 */
T_FTTEX *       FT7_Alloc( unsigned int size )
{
    /* проверяем число на ноль */
    if( size == 0 )
    {
        LOGW("%s: size is null", __FUNCTION__);
        return NULL;
    }
    /* проверяем число на размер */
    if( size > 0x8000 )
    {
        LOGE("%s: size %u is too large", __FUNCTION__, size);
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
            LOGW("%s: size %u is not pow of 2", __FUNCTION__, size);
        }
    }
    /* выделяем память под FT7 */
    T_FTTEX *ft7 = ( T_FTTEX* )VirtualAlloc( NULL, size*size, MEM_COMMIT, PAGE_READWRITE );
    if( ft7 == NULL )
    {
        LOGF("%s: with size %u not allocated", __FUNCTION__, size);
        return NULL;
    }
    LOGD("%s: ft7[%p] alloc (%ux%u)[%x]", __FUNCTION__, ft7, size, size, size*size);
    /* заполняем данные структуры FT7 */
    {
        ft7->magic  = MAGIC_FT7;
        ft7->size   = size;
        ft7->count  = 0;
        ft7->pX     = 0;
        ft7->pY     = size;
        ft7->nH     = 0;
        ft7->sort   = 0;
        ft7->_0     = 0;
    }
    return ft7;
}
/* освобождение памяти из под FT7 */
void            FT7_Free( T_FTTEX *ft7 )
{
    /* проверяем на пустой указатель */
    if( ft7 == NULL )
    {
        LOGF("%s: can't free NULL pointer", __FUNCTION__);
        return;
    }
    /* проверяем указатель на валидность FT7 */
    if( ft7->magic != MAGIC_FT7 )
    {
        LOGE("%s: can't free invalid ft7[%p] struct", __FUNCTION__, ft7);
        return;
    }
    LOGD("%s: ft7[%p] free (%ux%u)[%x]", __FUNCTION__, ft7, ft7->size, ft7->size, ft7->size*ft7->size);
    /* освобождаем память */
    VirtualFree( ft7, 0, MEM_RELEASE );
}
/* добавить новый объект FT7 */
T_FTSYM *       FT7_AddNewData( T_FTTEX *ft7, unsigned int tW, unsigned int tH, const void *pBuf, unsigned int pitch )
{
    /* проверяем на размеры вписываемых данных */
    if( tW > 0xff )
    {
        LOGF("%s: can't add data {%ux%u [%p]} to ft7[%p] becouse data width so big (%u)", __FUNCTION__, tW, tH, pBuf, ft7, tW);
        return NULL;
    }
    if( tH > 0xff )
    {
        LOGF("%s: can't add data {%ux%u [%p]} to ft7[%p] becouse data height so big (%u)", __FUNCTION__, tW, tH, pBuf, ft7, tH);
        return NULL;
    }

    /* проверяем на пустой указатель */
    if( ft7 == NULL )
    {
        LOGF("%s: can't add data {%ux%u [%p]} to NULL pointer", __FUNCTION__, tW, tH, pBuf);
        return NULL;
    }
    /* проверяем указатель на валидность FT7 */
    if( ft7->magic != MAGIC_FT7 )
    {
        LOGE("%s: can't add data {%ux%u [%p]} to invalid ft7[%p] struct", __FUNCTION__, tW, tH, pBuf, ft7);
        return NULL;
    }
    /* получаем указатель на данные символов */
    T_FTSYM * ps = FT7_GetSymbols( ft7 );
    ps += ft7->count;
    /* провервеям есть ли данные растра */
    if( pBuf != NULL )
    {
        /* если не помещается в данную линию по ширине, переходим на новую линию */
        if( tW + ft7->pX  >= ft7->size )
        {
            ft7->pX = 0;
            /* проверяем можно ли перейти на новую линию */
            if( ft7->pY <= ft7->nH )
            {
                LOGE("%s: can't add data {%ux%u [%p]} to ft7[%p] becouse can't set pen to new line", __FUNCTION__, tW, tH, pBuf, ft7);
                return NULL;
            }
            ft7->pY -= ft7->nH;
            ft7->nH = tH;
        }
        /* если не помещается в данную линию по высоте, увеличиваем линию */
        if( ft7->nH < tH )
        {
            ft7->nH = tH;
        }
        /* проверяем не попадём ли мы на системную память */
        if( ft7->pY  <= ( ( sizeof( T_FTTEX ) + ( sizeof( T_FTSYM ) * ( ft7->count + 1 ) ) ) / ft7->size ) + 1 + tH )
        {
            LOGE("%s: can't add data {%ux%u [%p]} to ft7[%p] becouse can't fill ft7sys mem", __FUNCTION__, tW, tH, pBuf, ft7);
            return NULL;
        }
        if( pitch == 0 )
        {
            pitch = tW;
        }
        uint8_t *pD = ( uint8_t* )ft7 + ( ( ( ft7->pY - tH ) * ft7->size ) + ft7->pX );
        uint8_t *pS = ( uint8_t* )pBuf;

        /* копируем данные */
        for (unsigned int i = 0; i < tH; ++i)
        {
            memcpy( pD + ( i * ft7->size ), pS + ( i * pitch ), tW );
        }
    }
    /* заполняем данные о символе */
    {
        ps->iU  = 0;
        if( pBuf == NULL )
        {
            ps->tX  = 0;
            ps->tY  = 0;
            ps->tW  = 0;
            ps->tH  = 0;
        }
        else
        {
            ps->tX  = ft7->pX;
            ps->tY  = ft7->pY - tH;
            ps->tW  = tW;
            ps->tH  = tH;
        }
        ps->aX  = 0;
        ps->aY  = 0;
        ps->nA  = 0;
        ps->nH  = 0;
        ps->iF  = 0;
        ps->_0  = 0;
    }
    /* обновляем данные ft7 */
    ++ft7->count;
    ft7->pX += tW;
    ft7->sort = 0;
    LOGV("%s: added new data {%ux%u [%p]} to ft7[%p]", __FUNCTION__, tW, tH, pBuf, ft7);
    return ps;
}
/* загрузка фонтфейса из файла */
unsigned int    FT7_FaceNewByFile( const char *name, unsigned int facenum )
{
    if( g_ftLib == NULL )
    {
        LOGF("%s: can't load face[\"%s\"(%d)] before init lib", __FUNCTION__, name, facenum);
        return 0;
    }
    for (unsigned int i = 0; i < FT7_MAXFACES; ++i)
    {
        if( g_ftFaces[i] == NULL )
        {
            FT_New_Face( g_ftLib, name, facenum, g_ftFaces+i );
            LOGD("%s: loaded face[\"%s\"(%d)] to FTFace[(%d/%d), %p]", __FUNCTION__, name, facenum, i+1, FT7_MAXFACES, g_ftFaces[i]);
            return i+1;
        }
    }
    LOGE("%s: can't load face[\"%s\"(%d)] becouse can't find free FTFace", __FUNCTION__, name, facenum);
    return 0;
}
/* освобождение фонтфейса */
void            FT7_FaceFree( unsigned int iF )
{
    if( iF > FT7_MAXFACES || iF == 0 )
    {
        LOGE("%s: can't free incorrect FTFace[(%d/%d)]", __FUNCTION__, iF, FT7_MAXFACES);
        return;
    }
    if( g_ftFaces[iF-1] == NULL )
    {
        LOGW("%s: can't free FTFace[(%d/%d), %p] becouse it is freed", __FUNCTION__, iF, FT7_MAXFACES, g_ftFaces[iF-1]);
        return;
    }
    LOGD("%s: freed FTFace[(%d/%d), %p]", __FUNCTION__, iF, FT7_MAXFACES, g_ftFaces[iF-1]);
    FT_Done_Face( g_ftFaces[iF-1] );
    g_ftFaces[iF-1] = NULL;
}
/* добавить новый символ FT7 из фонтфейса */
T_FTSYM *       FT7_AddSymbolByFace( T_FTTEX *ft7, unsigned int iF, unsigned int nH, unsigned int iU )
{
    /* проверяем на пустой указатель */
    if( ft7 == NULL )
    {
        LOGF("%s: can't add symbol['%c' %d (%d)] to NULL pointer", __FUNCTION__, iU, iU, nH);
        return NULL;
    }
    /* проверяем указатель на валидность FT7 */
    if( ft7->magic != MAGIC_FT7 )
    {
        LOGE("%s: can't add symbol['%c' %d (%d)] to invalid ft7[%p] struct", __FUNCTION__, iU, iU, nH, ft7);
        return NULL;
    }
    if( iF > FT7_MAXFACES || iF == 0 )
    {
        LOGE("%s: can't add symbol['%c' %d (%d)] by incorrect FTFace[(%d/%d)]", __FUNCTION__, iU, iU, nH, iF, FT7_MAXFACES);
        return NULL;
    }
    FT_Face pF = g_ftFaces[iF-1];
    if( pF == NULL )
    {
        LOGE("%s: can't add symbol['%c' %d (%d)] by null FTFace[(%d/%d), %p]", __FUNCTION__, iU, iU, nH, iF, FT7_MAXFACES, pF);
        return NULL;
    }
    if( nH > 0xff )
    {
        LOGE("%s: can't add symbol['%c' %d (%d)] to ft7[%p] by FTFace[(%d/%d), %p]\nheight(%d) too large", __FUNCTION__, iU, iU, nH, ft7, iF, FT7_MAXFACES, pF, nH);
        return NULL;
    }

    FT_Set_Pixel_Sizes( pF, 0, nH );
    FT_Error fterror;
    fterror = FT_Load_Glyph( pF, FT_Get_Char_Index( pF, iU), FT_LOAD_DEFAULT );
    if( fterror != 0 )
    {
        LOGE("%s: can't add symbol['%c' %d (%d)] to ft7[%p] by FTFace[(%d/%d), %p]\nFT_Error: %u when load glyph", __FUNCTION__, iU, iU, nH, ft7, iF, FT7_MAXFACES, pF, fterror);
        return NULL;
    }
    FT_GlyphSlot pG = pF->glyph;
    fterror = FT_Render_Glyph( pG, FT_RENDER_MODE_NORMAL );
    if( fterror != 0 )
    {
        LOGE("%s: can't add symbol['%c' %d (%d)] to ft7[%p] by FTFace[(%d/%d), %p]\nFT_Error: %u when render glyph", __FUNCTION__, iU, iU, nH, ft7, iF, FT7_MAXFACES, pF, fterror);
        return NULL;
    }
    T_FTSYM *ps = FT7_AddNewData( ft7, pG->bitmap.width, pG->bitmap.rows, pG->bitmap.buffer, pG->bitmap.pitch );
    if( ps == NULL )
    {
        return NULL;
    }
    /* Дописываем данные о символе */
    ps->iU  = iU;
    if( pG->bitmap.buffer )
    {
        ps->aX  = pG->bitmap_left;
        ps->aY  = pG->bitmap_top;
    }
    ps->nA  = pG->advance.x >> 6;
    ps->nH  = nH;
    ps->iF  = iF;
    return ps;
}
/* найти символ */
T_FTSYM *       FT7_GetSymbolByFace( T_FTTEX *ft7, unsigned int iF, unsigned int nH, unsigned int iU )
{
    /* проверяем на пустой указатель */
    if( ft7 == NULL )
    {
        LOGF("%s: can't get symbol['%c' %d (%d)] by NULL pointer", __FUNCTION__, iU, iU, nH);
        return NULL;
    }
    /* проверяем указатель на валидность FT7 */
    if( ft7->magic != MAGIC_FT7 )
    {
        LOGE("%s: can't get symbol['%c' %d (%d)] by invalid ft7[%p] struct", __FUNCTION__, iU, iU, nH, ft7);
        return NULL;
    }
    if( iF > FT7_MAXFACES || iF == 0 )
    {
        LOGE("%s: can't get symbol['%c' %d (%d)] by incorrect FTFace[(%d/%d)]", __FUNCTION__, iU, iU, nH, iF, FT7_MAXFACES);
        return NULL;
    }
    if( nH > 0xff )
    {
        LOGE("%s: can't get symbol['%c' %d (%d)] in ft7[%p] by FTFace[(%d/%d), %p]\nheight(%d) too large", __FUNCTION__, iU, iU, nH, ft7, iF, FT7_MAXFACES, g_ftFaces[iF-1], nH);
        return NULL;
    }
    /* данные не отсортированы */
    if( ft7->sort == 0 )
    {
        T_FTSYM *ps = FT7_GetSymbols( ft7 );
        for (unsigned int i = 0; i < ft7->count; ++i)
        {
            if( ( ps->iU == iU ) && ( ps->nH == nH ) && ( ps->iF == iF ) )
                return ps;
            ++ps;
        }
    }
    return NULL;
}
void            IDirect3DDevice9_FT7_CreateTexture(  IDirect3DDevice9 *D3DD, T_FTTEX *ft7, IDirect3DTexture9 **D3DTEX )
{
    /* проверяем на пустой указатель */
    if( ft7 == NULL )
    {
        LOGF("%s: can't create texture by NULL ft7", __FUNCTION__);
        return;
    }
    /* проверяем указатель на валидность FT7 */
    if( ft7->magic != MAGIC_FT7 )
    {
        LOGE("%s: can't create texture by ft7[%p] struct", __FUNCTION__, ft7);
        return;
    }
    IDirect3DDevice9_CreateTexture( D3DD, ft7->size, ft7->size, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8, D3DPOOL_DEFAULT, D3DTEX, NULL );
    IDirect3DTexture9_FT7_Sync( *D3DTEX, ft7 );
}
/* обновляет текстуру */
void            IDirect3DTexture9_FT7_Sync( IDirect3DTexture9 *D3DTEX, T_FTTEX *ft7 )
{
    /* проверяем на пустой указатель */
    if( ft7 == NULL )
    {
        LOGF("%s: can't sync NULL ft7", __FUNCTION__);
        return;
    }
    /* проверяем указатель на валидность FT7 */
    if( ft7->magic != MAGIC_FT7 )
    {
        LOGE("%s: can't sync invalid ft7[%p] struct", __FUNCTION__, ft7);
        return;
    }

    D3DLOCKED_RECT LRC;
    IDirect3DTexture9_LockRect( D3DTEX, 0, &LRC, NULL, 0 );
    memcpy( LRC.pBits, ft7, ft7->size*ft7->size );
    IDirect3DTexture9_UnlockRect( D3DTEX, 0 );
}

