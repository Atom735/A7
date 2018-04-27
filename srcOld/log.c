#ifndef UNICODE
#define UNICODE
#endif
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "log.h"

#define LOG_BUF_SIZE 4096

void LOG_T( unsigned int type, const char *fmt, ... ) {

    static WORD awColors[] = { 0x08, 0x03, 0x0A, 0x0E, 0x0C, 0xCF, };
    static HANDLE hOut = NULL;
    static char achTypes[] = { 'V', 'D', 'I', 'W', 'E', 'F'};
    static char buf[ LOG_BUF_SIZE ];
    static DWORD writenChars = 0;
    if( hOut == NULL ) {
        hOut = GetStdHandle( STD_OUTPUT_HANDLE );
    }
    HANDLE hOutF = CreateFileA( "log.log", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
    assert( type <= LOG_T_FATAL );
    assert( hOut != NULL );
    assert( hOutF != NULL );
    SetFilePointer( hOutF, 0, NULL, FILE_END );

    SetConsoleTextAttribute( hOut, awColors[ type ] );

    unsigned int ticks = GetTickCount();
    unsigned int n = snprintf ( buf, LOG_BUF_SIZE, "%c[% 10d] ", achTypes[type], ticks );
    WriteConsoleA( hOut, buf, n, &writenChars, NULL );
    WriteFile( hOutF, buf, n, &writenChars, NULL );

    va_list args;
    va_start (args, fmt);
    unsigned int end = _vsnprintf (buf, LOG_BUF_SIZE, fmt, args);
    va_end (args);
    unsigned int begin = 0;
    unsigned int newline = 1;
    for(unsigned int i = 0; i < end; ++i) {
        if(buf[i] == '\n') {
            if(newline == 0) {
                newline = 1;
                if(begin > 0) {
                    WriteConsoleA( hOut, "\n              ", 15, &writenChars, NULL );
                    WriteFile( hOutF, "\n              ", 15, &writenChars, NULL );
                }
                WriteConsoleA( hOut, buf+begin, i-begin, &writenChars, NULL );
                WriteFile( hOutF, buf+begin, i-begin, &writenChars, NULL );
                begin = i+1;
                continue;
            } else {
                begin = i+1;
            }
        } else {
            newline = 0;
        }
    }
    if(end > begin) {
        WriteConsoleA( hOut, buf+begin, end-begin, &writenChars, NULL );
        WriteFile( hOutF, buf+begin, end-begin, &writenChars, NULL );
    }
    WriteConsoleA( hOut, "\n", 1, &writenChars, NULL );
    WriteFile( hOutF, "\n", 1, &writenChars, NULL );
    SetConsoleTextAttribute(hOut, 0x07);
    CloseHandle( hOutF );
}
