#ifndef _A7D_A7LOG_H
#define _A7D_A7LOG_H 0x20180427
#include "a7head.h"

/*
    Зависимости
*/
    /* Заимстуем глобальный буффер в 64Кб для всячины */
    extern BYTE            g_a7CoreBuf64k[64*1024];

/*
    Различные константы
*/
    /* Уровень сообщения лога */
    enum {
        A7ELOGLEVEL_VERB = 0,
        A7ELOGLEVEL_DEBUG,
        A7ELOGLEVEL_INFO,
        A7ELOGLEVEL_WARN,
        A7ELOGLEVEL_ERROR,
        A7ELOGLEVEL_FATAL,
    };
/*
    Объявление функций
*/
    /* Функции логирования строки */
    VOID                a7rCoreLog( UINT uLevel, LPCWSTR txt );
    /* Функции логирования форматированной строки */
    VOID                a7rCoreLogF( UINT uLevel, LPCWSTR fmt, ... );
    /* Скоращения функций логирования */
    #define             a7rLogV(txt)    a7rCoreLog( A7ELOGLEVEL_VERB, txt )
    #define             a7rLogD(txt)    a7rCoreLog( A7ELOGLEVEL_DEBUG, txt )
    #define             a7rLogI(txt)    a7rCoreLog( A7ELOGLEVEL_INFO, txt )
    #define             a7rLogW(txt)    a7rCoreLog( A7ELOGLEVEL_WARN, txt )
    #define             a7rLogE(txt)    a7rCoreLog( A7ELOGLEVEL_ERROR, txt )
    #define             a7rLogF(txt)    a7rCoreLog( A7ELOGLEVEL_FATAL, txt )
    #define             a7rLogVF(...)   a7rCoreLogF( A7ELOGLEVEL_VERB, __VA_ARGS__ )
    #define             a7rLogDF(...)   a7rCoreLogF( A7ELOGLEVEL_DEBUG, __VA_ARGS__ )
    #define             a7rLogIF(...)   a7rCoreLogF( A7ELOGLEVEL_INFO, __VA_ARGS__ )
    #define             a7rLogWF(...)   a7rCoreLogF( A7ELOGLEVEL_WARN, __VA_ARGS__ )
    #define             a7rLogEF(...)   a7rCoreLogF( A7ELOGLEVEL_ERROR, __VA_ARGS__ )
    #define             a7rLogFF(...)   a7rCoreLogF( A7ELOGLEVEL_FATAL, __VA_ARGS__ )

#endif /* _A7D_LOG_H */
