#ifndef _H__
#define _H__

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

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "log.h"
#include "api.h"
#include "ft.h"
#include "txt.h"


    /* Структура точек D3D9 2d Diffuse Texture */
    typedef struct T_VERTEX_2DT
    {
        FLOAT   x, y, z, w; /* координаты точки */
        DWORD   c;          /* цвет точки */
        FLOAT   u, v;       /* координаты текстуры */
    } T_VERTEX_2DT;
    /* Константа структуры точек D3D9 2d Diffuse Texture */
    #define D3DFVF_2DT  (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

    /* Структура точек D3D9 3d Diffuse Texture */
    typedef struct T_VERTEX_3DT
    {
        FLOAT   x, y, z;    /* координаты точки */
        DWORD   c;          /* цвет точки */
        FLOAT   u, v;       /* координаты текстуры */
    } T_VERTEX_3DT;
    /* Константа структуры точек D3D9 3d Diffuse Texture */
    #define D3DFVF_3DT  (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

/* магические константы */
    #define MAGIC_FT7 ((uint32_t)0xff06a3ff)



#endif /* _H__ */
