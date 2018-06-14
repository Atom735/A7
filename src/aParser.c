#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
/*
SET PATH=%PATH%;F:\PROGRAMS\MinGW-W64\i686-7.2.0-win32-sjlj-rt_v5-rev1\mingw32\bin
gcc.exe -Wall -o "parser.exe" "src/aParser.c" -O3
*/

/*
 * define — создание константы или макроса;
 * undef — удаление константы или макроса;
 * include — вставка содержимого указанного файла;
 * if — проверка истинности выражения;
 * ifdef — проверка существования константы или макроса;
 * ifndef — проверка не существования константы или макроса;
 * else — ветка условной компиляции при ложности выражения if;
 * elif — проверка истинности другого выражения; краткая форма записи для комбинации else и if;
 * endif — конец ветки условной компиляции;
 * error — вывод сообщения и остановка компиляции;
 * line — указание имени файла и номера текущей строки для компилятора;
 * warning — вывод сообщения без остановки компиляции;
 * pragma — указание действия, зависящего от реализации, для препроцессора или компилятора;
 */

enum
{
    CPPKW_NULL = 0,
    CPPKW_DEFINE,
    CPPKW_UNDEF,
    CPPKW_INCLUDE,
    CPPKW_IF,
    CPPKW_IFDEF,
    CPPKW_IFNDEF,
    CPPKW_ELSE,
    CPPKW_ELIF,
    CPPKW_ENDIF,
    CPPKW_ERROR,
    CPPKW_WARNING,
    CPPKW_LINE,
    CPPKW_PRAGMA
};

unsigned rCPP_GetKeyWord( char const * str )
{
    switch( *str )
    {
        case 'd': /* define */
        {
            if( str[1] != 'e' ) return CPPKW_NULL;
            if( str[2] != 'f' ) return CPPKW_NULL;
            if( str[3] != 'i' ) return CPPKW_NULL;
            if( str[4] != 'n' ) return CPPKW_NULL;
            if( str[5] != 'e' ) return CPPKW_NULL;
            if( isspace( str[6] ) ) return CPPKW_DEFINE;
            return CPPKW_NULL;
        }
        case 'u': /* undef */
        {
            if( str[1] != 'n' ) return CPPKW_NULL;
            if( str[2] != 'd' ) return CPPKW_NULL;
            if( str[3] != 'e' ) return CPPKW_NULL;
            if( str[4] != 'f' ) return CPPKW_NULL;
            if( isspace( str[5] ) ) return CPPKW_UNDEF;
            return CPPKW_NULL;
        }
        case 'i': /* include, if, ifdef, ifndef */
        {
            if( str[1] == 'n' ) /* include */
            {
                if( str[2] != 'c' ) return CPPKW_NULL;
                if( str[3] != 'l' ) return CPPKW_NULL;
                if( str[4] != 'u' ) return CPPKW_NULL;
                if( str[5] != 'd' ) return CPPKW_NULL;
                if( str[6] != 'e' ) return CPPKW_NULL;
                if( isspace( str[7] ) ) return CPPKW_INCLUDE;
                return CPPKW_NULL;
            }
            if( str[1] == 'f' ) /* if, ifdef, ifndef */
            {
                if( isspace( str[2] ) ) return CPPKW_IF; /* if */
                if( str[2] == 'd' ) /* ifdef */
                {
                    if( str[3] != 'e' ) return CPPKW_NULL;
                    if( str[4] != 'f' ) return CPPKW_NULL;
                    if( isspace( str[5] ) ) return CPPKW_IFDEF;
                }
                if( str[2] == 'n' ) /* ifndef */
                {
                    if( str[3] != 'd' ) return CPPKW_NULL;
                    if( str[4] != 'e' ) return CPPKW_NULL;
                    if( str[5] != 'f' ) return CPPKW_NULL;
                    if( isspace( str[6] ) ) return CPPKW_IFNDEF;
                }
                return CPPKW_NULL;
            }
            return CPPKW_NULL;
        }
        case 'e': /* else, elif, endif, error */
        {
            if( str[1] == 'l' ) /* else, elif */
            {
                if( str[2] == 's' ) /* else */
                {
                    if( str[3] != 'e' ) return CPPKW_NULL;
                    if( isspace( str[4] ) ) return CPPKW_ELSE;
                }
                if( str[2] == 'i' ) /* elif */
                {
                    if( str[3] != 'f' ) return CPPKW_NULL;
                    if( isspace( str[4] ) ) return CPPKW_ELIF;
                }
                return CPPKW_NULL;
            }
            if( str[1] == 'n' ) /* endif */
            {
                if( str[2] != 'd' ) return CPPKW_NULL;
                if( str[3] != 'i' ) return CPPKW_NULL;
                if( str[4] != 'f' ) return CPPKW_NULL;
                if( isspace( str[5] ) ) return CPPKW_ENDIF;
                return CPPKW_NULL;
            }
            if( str[1] == 'r' ) /* error */
            {
                if( str[2] != 'r' ) return CPPKW_NULL;
                if( str[3] != 'o' ) return CPPKW_NULL;
                if( str[4] != 'r' ) return CPPKW_NULL;
                if( isspace( str[5] ) ) return CPPKW_ERROR;
                return CPPKW_NULL;
            }
            return CPPKW_NULL;
        }
        case 'w': /* warning */
        {
            if( str[1] != 'a' ) return CPPKW_NULL;
            if( str[2] != 'r' ) return CPPKW_NULL;
            if( str[3] != 'n' ) return CPPKW_NULL;
            if( str[4] != 'i' ) return CPPKW_NULL;
            if( str[5] != 'n' ) return CPPKW_NULL;
            if( str[6] != 'g' ) return CPPKW_NULL;
            if( isspace( str[7] ) ) return CPPKW_WARNING;
            return CPPKW_NULL;
        }
        case 'l': /* line */
        {
            if( str[1] != 'i' ) return CPPKW_NULL;
            if( str[2] != 'n' ) return CPPKW_NULL;
            if( str[3] != 'e' ) return CPPKW_NULL;
            if( isspace( str[4] ) ) return CPPKW_LINE;
            return CPPKW_NULL;
        }
        case 'p': /* pragma */
        {
            if( str[1] != 'r' ) return CPPKW_NULL;
            if( str[2] != 'a' ) return CPPKW_NULL;
            if( str[3] != 'g' ) return CPPKW_NULL;
            if( str[4] != 'm' ) return CPPKW_NULL;
            if( str[5] != 'a' ) return CPPKW_NULL;
            if( isspace( str[6] ) ) return CPPKW_PRAGMA;
            return CPPKW_NULL;
        }
    }
}

/* ASCII TABLE
 *     .0  .1  .2  .3  .4  .5  .6  .7  .8  .9  .A  .B  .C  .D  .E  .F
 * 0.  NUL SOH STX ETX EOT ENQ ACK BEL BS  HT  LF  VT  FF  CR  SO  SI
 * 1.  DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN EM  SUB ESC FS  GS  RS  US
 * 2.      !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
 * 3.  0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
 * 4.  @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
 * 5.  P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
 * 6.  `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
 * 7.  p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~   DEL
 */

struct sList
{
    unsigned    uTypes;      /* Тип символа */
    char const *pSymbols;    /* Указатель на символы */
    char const *pIfs[12];    /* Указатель на символы условий */
    struct sList *pPrev;    /* Указатель на пред. символ */
    struct sList *pNext;    /* Указатель на след. символ */
};

char const     *pIfs[12];               /* Указатель на символы условий */
int             iIfsDepth       = 0;    /* глубина условий */
struct sList   *ListBegin       = NULL; /* первый символ */
struct sList   *ListEnd         = NULL; /* последний символ */

struct sList *ListNew( struct sList *in, char const *str )
{
    struct sList * o = ( struct sList * )malloc( sizeof( struct sList ) );
    o->uTypes = 0;
    o->pSymbols = str;
    for( int i = 0; i < 12; ++i )
        o->pIfs[i] = pIfs[i];
    o->pNext = NULL;
    if( in != NULL )
    {
        o->pPrev = in;
        in->pNext = o;
    }
    else
    {
        o->pPrev = NULL;
    }
    return o;
}



int main(int argc, char const *argv[])
{
    FILE * pF = fopen( "F:\\PROGRAMS\\MinGW-W64\\i686-7.2.0-win32-sjlj-rt_v5-rev1\\mingw32\\i686-w64-mingw32\\include\\d3d9.h", "r" );
    fseek( pF, 0, SEEK_END );
    int nSize = ftell( pF );
    fseek( pF, 0, SEEK_SET);
    char * pBuf = ( char * )malloc( nSize + 1 );
    fread( pBuf, 1, nSize, pF );
    fclose( pF );
    pF = fopen( "p.txt", "w" );

    for( int i = 0; i < nSize; ++i )
    {
        if( pBuf[i] == '#' )
        {
            while( pBuf[i] != '\n' )
            {
                ++i;
                unsigned u = 0;
                if( ( u = rCPP_GetKeyWord( pBuf + i ) ) != CPPKW_NULL )
                {
                    switch( u )
                    {
                        case CPPKW_DEFINE:
                        {
                            fputs( "\n@DEFINE >>>\n", pF );
                            int j = i;
                            while( pBuf[j] != '\n' ) ++j;
                            pBuf[j] = '\0';
                            fputs( pBuf + i, pF );
                            for( int d = 0; d < 12; ++d )
                            {
                                if( pIfs[ d ] == NULL ) break;

                                fprintf( pF, "\n    @IF[%i] >>>\n", d );
                                fputs( pIfs[ d ], pF );
                            }
                            fputs( "\n@DEFINE <<<\n", pF );
                            break;
                        }
                        case CPPKW_IF:
                        case CPPKW_IFDEF:
                        case CPPKW_IFNDEF:
                        {
                            pIfs[ iIfsDepth ] = pBuf + i;
                            ++iIfsDepth;
                            while( pBuf[i] != '\n' ) ++i;
                            pBuf[i] = '\0';
                            break;
                        }
                        case CPPKW_ELSE:
                        case CPPKW_ELIF:
                        {
                            pIfs[ iIfsDepth ] = pBuf + i;
                            while( pBuf[i] != '\n' ) ++i;
                            pBuf[i] = '\0';
                            break;
                        }
                        case CPPKW_ENDIF:
                        {
                            --iIfsDepth;
                            pIfs[ iIfsDepth ] = NULL;
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    fclose( pF );
    free( pBuf );
    return 0;
}




#if 0

























/*
 * Стандартом С89 предусмотрены следующие ключевые слова:
 *
 * sizeof - для указания операции получения размера объекта;
 * typedef - для описания прототипа объекта;
 *
 * для объявления переменных:
 *     auto, register - для обозначения класса хранения переменных;
 *     extern - для обозначения того, что объект описывается в другом месте;
 *     static - для обозначения того, что объект статический;
 * char, short, int, long, signed, unsigned, float, double, void - для обозначения типа переменных (void для указания на произвольный родовой тип);
 * struct, enum, union - для обозначения специальных типов данных;
 * для обозначения операторов и их элементов:
 *     do, for, while - для обозначения операторов цикла;
 *     if, else - для обозначения условного оператора;
 *     switch, case, default - для обозначения оператора выбора;
 * для обозначения операторов перехода:
 *     break, continue - операторы прерывания исполнения кода;
 *     goto - для обозначения оператора безусловного перехода;
 * return - для обозначения оператора возврата из функции;
 * В стандарте C99[⇨] добавлены следующие ключевые слова:
 * inline - для обозначения того, что функция является встраиваемой;
 * _Bool, _Complex, _Imaginary - для обозначения типа переменной;
 * restrict - для обозначения того, что объявляемый указатель указывает на блок памяти, на который не указывает никакой другой указатель;
 *
 * sizeof
 * typedef
 * auto, register
 * extern
 * static
 * char, short, int, long, signed, unsigned, float, double, void
 * struct, enum, union
 * do, for, while
 * if, else
 * switch, case, default
 * break, continue
 * goto
 * return
 * inline
 * _Bool, _Complex, _Imaginary
 * restrict
 *
 * _Bool
 * _Complex
 * _Imaginary
 * auto
 * break
 * case
 * char
 * continue
 * default
 * do
 * double
 * else
 * enum
 * extern
 * float
 * for
 * goto
 * if
 * inline
 * int
 * long
 * register
 * restrict
 * return
 * short
 * signed
 * sizeof
 * static
 * struct
 * switch
 * typedef
 * union
 * unsigned
 * void
 * while
 */

enum
{
    CKW_NULL,
    CKW__BOOL,
    CKW__COMPLEX,
    CKW__IMAGINARY,
    CKW_AUTO,
    CKW_BREAK,
    CKW_CASE,
    CKW_CHAR,
    CKW_CONTINUE,
    CKW_DEFAULT,
    CKW_DO,
    CKW_DOUBLE,
    CKW_ELSE,
    CKW_ENUM,
    CKW_EXTERN,
    CKW_FLOAT,
    CKW_FOR,
    CKW_GOTO,
    CKW_IF,
    CKW_INLINE,
    CKW_INT,
    CKW_LONG,
    CKW_REGISTER,
    CKW_RESTRICT,
    CKW_RETURN,
    CKW_SHORT,
    CKW_SIGNED,
    CKW_SIZEOF,
    CKW_STATIC,
    CKW_STRUCT,
    CKW_SWITCH,
    CKW_TYPEDEF,
    CKW_UNION,
    CKW_UNSIGNED,
    CKW_VOID,
    CKW_WHILE,
};

unsigned rC_GetKeyWord( char const * str )
{
}


int rC_IsTypedef( char const * str )
{
    if( str[0] != 't' ) return 0;
    if( str[1] != 'y' ) return 0;
    if( str[2] != 'p' ) return 0;
    if( str[3] != 'e' ) return 0;
    if( str[4] != 'd' ) return 0;
    if( str[5] != 'e' ) return 0;
    if( str[6] != 'f' ) return 0;
    return 7;
}
int rC_IsStruct( char const * str )
{
    if( str[0] != 's' ) return 0;
    if( str[1] != 't' ) return 0;
    if( str[2] != 'r' ) return 0;
    if( str[3] != 'u' ) return 0;
    if( str[4] != 'c' ) return 0;
    if( str[5] != 't' ) return 0;
    return 6;
}
int rC_IsCommentStart( char const * str )
{
    if( str[0] != '/' ) return 0;
    if( str[1] != '*' ) return 0;
    return 2;
}
int rC_IsCommentEnd( char const * str )
{
    if( str[0] != '*' ) return 0;
    if( str[1] != '/' ) return 0;
    return 2;
}
int rC_IsCommentStartLine( char const * str )
{
    if( str[0] != '/' ) return 0;
    if( str[1] != '/' ) return 0;
    return 2;
}
int rC_IsIdentifier( char const * str )
{
    int o = 0;
    if( ( ( *str >= 'A' ) && ( *str <= 'Z' ) ) ||
        ( ( *str >= 'a' ) && ( *str <= 'z' ) ) ||
        ( ( *str == '_' ) ) )
    {
        ++o;
    }
    else
    {
        return 0;
    }
    while(  ( ( str[o] >= 'A' ) && ( str[o] <= 'Z' ) ) ||
            ( ( str[o] >= 'a' ) && ( str[o] <= 'z' ) ) ||
            ( ( str[o] >= '0' ) && ( str[o] <= '9' ) ) ||
            ( ( str[o] == '_' ) ) )
    {
        ++o;
    }
    return o;
}
int rC_IsNumber( char const * str, int * num )
{
    if( *str == '0' )
    {
        /* Либо это [0], либо запись в [Oct], либо запись в [Hex] */
        if( ( str[1] == 'x' ) || ( str[1] == 'X' ) )
        {
            /* [Hex] */
            int o = 2;
            int n = 0;
            while( 1 )
            {
                if( ( str[o] >= '0' ) && ( str[o] <= '9' ) )
                {
                    n *= 16;
                    n += str[o] - '0';
                    ++o;
                    continue;
                }
                if( ( str[o] >= 'a' ) && ( str[o] <= 'f' ) )
                {
                    n *= 16;
                    n += str[o] - 'a';
                    ++o;
                    continue;
                }
                if( ( str[o] >= 'A' ) && ( str[o] <= 'F' ) )
                {
                    n *= 16;
                    n += str[o] - 'A';
                    ++o;
                    continue;
                }
                break;
            }
            if( o == 2 )
            {
                return 0;
            }
            if( num != NULL )
            {
                *num = n;
            }
            return o;
        }
        if( ( str[1] >= '0' ) && ( str[1] <= '7' ) )
        {
            /* [Oct] */
            int o = 2;
            int n = str[1] - '0';
            while( ( ( str[o] >= '0' ) && ( str[o] <= '7' ) ) )
            {
                n *= 8;
                n += str[o] - '0';
                ++o;
            }
            if( num != NULL )
            {
                *num = n;
            }
            return o;
        }
        /* [0] */
        if( num != NULL )
        {
            *num = 0;
        }
        return 1;
    }
    if( ( *str > '0' ) && ( *str <= '9' ) )
    {
        /* [Dec] */
        int o = 1;
        int n = *str - '0';
        while( ( ( str[o] >= '0' ) && ( str[o] <= '9' ) ) )
        {
            n *= 10;
            n += str[o] - '0';
            ++o;
        }
        if( num != NULL )
        {
            *num = n;
        }
        return o;
    }
    return 0;
}

int eee_main(int argc, char const *argv[])
{
    FILE * pF = fopen( "F:\\PROGRAMS\\MinGW-W64\\i686-7.2.0-win32-sjlj-rt_v5-rev1\\mingw32\\i686-w64-mingw32\\include\\d3d9.h", "r" );
    fseek( pF, 0, SEEK_END );
    int nSize = ftell( pF );
    fseek( pF, 0, SEEK_SET);
    char * pBuf = ( char * )malloc( nSize + 1 );
    fread( pBuf, 1, nSize, pF );
    fclose( pF );
    pF = fopen( "p.txt", "w" );

    int L = 0;
    char * pLastCommentStdBegin     = NULL;
    char * pLastCommentStdEnd       = NULL;
    char * pLastCommentLineBegin    = NULL;
    char * pLastCommentLineEnd      = NULL;
    char * pLastDefineBegin         = NULL;
    char * pLastDefineEnd           = NULL;
    char * pLastDefineIdentBegin    = NULL;
    char * pLastDefineIdentEnd      = NULL;

    for( int i = 0; i < nSize; ++i )
    {
        if( ( L = rC_IsCommentStart( pBuf + i ) ) > 0 )
        {
            pLastCommentStdBegin = pBuf + i;
            i += L;
            while( ! ( L = rC_IsCommentEnd( pBuf + i ) ) )
            {
                ++i;
                if( i == nSize )
                {
                    fclose( pF );
                    free( pBuf );
                    return 1;
                }
            }
            i += L;
            pLastCommentStdEnd = pBuf + i;
            fputs( "\n@COMMENT_STD >>>\n", pF );
            *pLastCommentStdEnd = '\0';
            fputs( pLastCommentStdBegin, pF );
            fputs( "\n$COMMENT_STD <<<\n", pF );

            continue;
        }
        if( ( L = rC_IsCommentStartLine( pBuf + i ) ) > 0 )
        {
            pLastCommentLineBegin = pBuf + i;
            i += L;
            while( pBuf[i] != '\n' )
            {
                ++i;
                if( i == nSize )
                {
                    break;
                }
            }
            pLastCommentLineEnd = pBuf + i;
            fputs( "\n@COMMENT_LINE >>>\n", pF );
            *pLastCommentLineEnd = '\0';
            fputs( pLastCommentLineBegin, pF );
            fputs( "\n$COMMENT_LINE <<<\n", pF );
            continue;
        }
        if( ( L = rC_IsDefine( pBuf + i ) ) )
        {
            pLastDefineBegin = pBuf + i;
            i += L;
            while( ( L = rC_IsIdentifier( pBuf + i ) ) == 0 )
            {
                ++i;
                if( i == nSize )
                {
                    break;
                }
            }
            pLastDefineIdentBegin = pBuf + i;
            i += L;
            pLastDefineIdentEnd = pBuf + i;
            while( pBuf[i] != '\n' )
            {
                ++i;
                if( i == nSize )
                {
                    break;
                }
            }
            pLastDefineEnd = pBuf + i;
            fputs( "\n@DEFINE >>>\n", pF );
            *pLastDefineEnd = '\0';
            fputs( pLastDefineBegin, pF );
            fputs( "\n@DEFINE_IDENT >>> ", pF );
            *pLastDefineIdentEnd = '\0';
            fputs( pLastDefineIdentBegin, pF );
            fputs( "\n$DEFINE <<<\n", pF );
            continue;
        }
    }

    fclose( pF );
    free( pBuf );
    return 0;
}
#endif
