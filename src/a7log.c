#include "a7head.h"
#include "a7log.h"

// #include <ShellAPI.h>
#include <ShlwAPI.h>

/*
    Глобальные переменные
*/
    /* Цвета для уровней сообщения лога */
    WORD            g_a7CoreLogColors[]         = { 0x08, 0x03, 0x0A, 0x0E, 0x0C, 0xCF, };
    /* Буквицы для уровней сообщения лога */
    WCHAR           g_a7CoreLogChars[]          = { L'V', L'D', L'I', L'W', L'E', L'F', };
    /* Хендл вывода сообщений */
    HANDLE          g_a7CoreLogHOut             = NULL;
    /* Имя файла для сообщений лога */
    LPCWSTR         g_a7CoreLogFileName         = L"log.log";

/* Функции логирования строки */
VOID a7rCoreLog( UINT uLevel, LPCTSTR txt )
{
    DWORD writenChars;
    /* Проверка открытой консоли */
    if( g_a7CoreLogHOut == NULL )
    {
        AllocConsole();
        g_a7CoreLogHOut = GetStdHandle( STD_OUTPUT_HANDLE );
    }
    /* Открыаем файл и произодим запись в конец */
    HANDLE hFileOut = CreateFileW( g_a7CoreLogFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    SetFilePointer( hFileOut, 0, NULL, FILE_END );
    /* Настраиваем цвет вывод в консоль */
    SetConsoleTextAttribute( g_a7CoreLogHOut, g_a7CoreLogColors[ uLevel ] );
    /* Получаем время работы */
    DWORD ticks = GetTickCount();
    /* Готовим заголовок лога и записыаем его */
    int n = wnsprintfW( ( LPWSTR )( g_a7CoreBuf64k+16*1024*sizeof(WCHAR) ), 16*1024, L"%lc[% 10d] ", g_a7CoreLogChars[ uLevel ], ticks );
    WriteConsoleW( g_a7CoreLogHOut, g_a7CoreBuf64k+16*1024*sizeof(WCHAR), n, &writenChars, NULL );
    WriteFile( hFileOut, g_a7CoreBuf64k+16*1024*sizeof(WCHAR), n*sizeof(WCHAR), &writenChars, NULL );
    /* получаем количество символов для записи */
    // int end = lstrlenW( txt );
    int begin = 0;
    int newline = 1;
    /* начинаем запись, да так, что бы каждая новая строка была с отступом */
    for( int i = 0; TRUE; ++i )
    {
        /* наткнулись на символ новой строки? */
        if( txt[i] == L'\n' )
        {
            /* мы на новой строке? */
            if( newline == 0 )
            {
                /* теперь будем на новой */
                newline = 1;
                /* если это не первый символ текста */
                // if( begin != 0 )
                {
                    /* записываем отступы */
                    WriteConsoleW( g_a7CoreLogHOut, L"\n              ", 15, &writenChars, NULL );
                    WriteFile( hFileOut, L"\n              ", 15*sizeof(WCHAR), &writenChars, NULL );
                }
                /* записываем саму строку если она есть */
                if( i - begin > 0 )
                {
                    WriteConsoleW( g_a7CoreLogHOut, txt+begin, i-begin, &writenChars, NULL );
                    WriteFile( hFileOut, txt+begin, (i-begin)*sizeof(WCHAR), &writenChars, NULL );
                }
                /* переходим к след символу */
                begin = i+1;
            }
            else
            {
                /* пропускаем пустую строку */
                begin = i+1;
            }
        }
        else if( txt[i] == 0 )
        {
            WriteConsoleW( g_a7CoreLogHOut, txt+begin, i-begin, &writenChars, NULL );
            WriteFile( hFileOut, txt+begin, (i-begin)*sizeof(WCHAR), &writenChars, NULL );
            break;
        }
        else
        {
            /* мы не навой строке */
            newline = 0;
        }
    }
    /* если остались символы, то дописыаем их */
    // if(end > begin) {
    //     WriteConsoleW( g_a7CoreLogHOut, txt+begin, end-begin, &writenChars, NULL );
    //     WriteFile( hFileOut, txt+begin, (end-begin)*sizeof(WCHAR), &writenChars, NULL );
    // }
    /* записываем переход на новую строку */
    WriteConsoleA( g_a7CoreLogHOut, "\n", 1, &writenChars, NULL );
    WriteFile( hFileOut, L"\n", sizeof(WCHAR), &writenChars, NULL );
    /* Восстанавлиаем цвет консоли и закрываем файл */
    SetConsoleTextAttribute( g_a7CoreLogHOut, 0x07 );
    CloseHandle( hFileOut );
    hFileOut = NULL;
}
/* Функции логирования форматированной строки */
VOID a7rCoreLogF( UINT uLevel, LPCWSTR fmt, ... )
{
    va_list args;
    va_start( args, fmt );
    wvnsprintfW( ( LPWSTR )( g_a7CoreBuf64k ), 16*1024, fmt, args );
    va_end( args );
    a7rCoreLog( uLevel, ( LPWSTR )( g_a7CoreBuf64k ) );
}
