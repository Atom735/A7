#ifndef _H_FT
#define _H_FT

/* Структуры */
    /* Структуры символов */
    typedef struct T_FTSYM {
        uint32_t    iU;     /* юникод символа */
        uint16_t    tX, tY; /* координаты текстуры */
        uint8_t     tW, tH; /* размеры текстуры */
        int8_t      aX, aY; /* отступы текстуры от пера */
        uint8_t     nA;     /* расстояние до след символа */
        uint8_t     nH;     /* шрифтовая высота символа */
        uint8_t     iF;     /* номер фонтфейса */
        uint8_t     _0;     /* дополнительные данные, для выравнивания */
    } T_FTSYM;
    /* Структура заголовка текстуры */
    typedef struct T_FTTEX {
        uint32_t    magic;  /* магическая константа */
        uint16_t    size;   /* размер текстуры */
        uint16_t    count;  /* количесто объектов */
        uint16_t    pX, pY; /* позиция пера заполнения */
        uint16_t    nH;     /* высота заполняемой линии */
        uint8_t     sort;   /* информация о сортировке */
        uint8_t     _0;     /* дополнительные данные, для выравнивания */
    } T_FTTEX;

/* Объявления фукнций */
    /* получить данные о объектах FT7 */
    #define         FT7_GetSymbols( ft7 ) (T_FTSYM*)((T_FTTEX*)ft7+1)
    /* инициализация FT7 */
    void            FT7_Init();
    /* освобождение FT7 */
    void            FT7_Release();
    /* выделение память под FT7 */
    T_FTTEX *       FT7_Alloc( unsigned int size );
    /* освобождение памяти из под FT7 */
    void            FT7_Free( T_FTTEX *ft7 );
    /* добавить новый объект FT7 */
    T_FTSYM *       FT7_AddNewData( T_FTTEX *ft7, unsigned int tW, unsigned int tH, const void *pBuf, unsigned int pitch );
    /* загрузка фонтфейса из файла */
    unsigned int    FT7_FaceNewByFile( const char *name, unsigned int facenum );
    /* освобождение фонтфейса */
    void            FT7_FaceFree( unsigned int iF );
    /* добавить новый символ FT7 из фонтфейса */
    T_FTSYM *       FT7_AddSymbolByFace( T_FTTEX *ft7, unsigned int iF, unsigned int nH, unsigned int iU );
    /* найти символ */
    T_FTSYM *       FT7_GetSymbolByFace( T_FTTEX *ft7, unsigned int iF, unsigned int nH, unsigned int iU );
    void            IDirect3DDevice9_FT7_CreateTexture( IDirect3DDevice9 *D3DD, T_FTTEX *ft7, IDirect3DTexture9 **D3DTEX );
    /* обновляет текстуру */
    void            IDirect3DTexture9_FT7_Sync( IDirect3DTexture9 *D3DTEX, T_FTTEX *ft7 );

#endif /* _H_FT */
