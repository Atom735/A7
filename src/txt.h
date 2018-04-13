#ifndef _H_TXT
#define _H_TXT

/* Структуры */
    /* структура статического текста */
    typedef struct T_TXT {
        unsigned int            count;  /* количество рисуемых символов */
        unsigned int            cmax;   /* максимальное количество рисуемых символов */
        unsigned int            adding; /* происходит ли добавление объектов */
        IDirect3DVertexBuffer9 *D3DVB;  /* Буффер вершин */
        DWORD                   D3DFVF; /* Тип буффера */
        PVOID                   pV;     /* Указатель на буффер вершин */
    } T_TXT;

/* Объявления фукнций */
    /* Выделение места под количество текста */
    T_TXT *         IDirect3DDevice9_TXT_Create( IDirect3DDevice9 *D3DD, unsigned int count, DWORD D3DFVF );
    /* Освобождение */
    void            TXT_Free( T_TXT *txt );
    /* Начать добавление */
    void            TXT_EditBegin( T_TXT *txt );
    /* Закончить добавление */
    void            TXT_EditEnd( T_TXT *txt );
    /* Добавление квадрата */
    unsigned int    TXT_AddRect( T_TXT *txt, FLOAT pX, FLOAT pY, FLOAT pZ, DWORD color, FLOAT fW, FLOAT fH, FLOAT tU0, FLOAT tV0, FLOAT tU1, FLOAT tV2 );
    /* Добавление линии символов */
    unsigned int    TXT_AddFT7Line( T_TXT *txt, T_FTTEX *ft7, FLOAT pX, FLOAT pY, FLOAT pZ, DWORD color, const char *szLine, unsigned int iF, unsigned int nH );

    /* Добавление символа */
    void            TXT_AddFT7Symbol( T_TXT *txt, T_FTSYM *ft7s, FLOAT pX, FLOAT pY, FLOAT pZ, DWORD color, FLOAT tX, FLOAT tY, FLOAT tF );
    /* Рендеринг текста */
    void            IDirect3DDevice9_TXT_Render( IDirect3DDevice9 *D3DD, T_TXT *txt );

#endif
