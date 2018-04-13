/* Поиск окна SysHead в SysList */
BOOL CALLBACK SysHead_rWEP( HWND hWnd, LPARAM lParam )
{
    char bufClass[256]; char bufTxt[256]; UINT u;
    u = RealGetWindowClassA( hWnd, bufClass, 255 ); bufClass[u] = '\0';
    u = GetWindowTextA( hWnd, bufTxt, 255 ); bufTxt[u] = '\0';
    if( strcmp( "SysHeader32", bufClass ) == 0 )
    {
        if( strcmp( "", bufTxt ) == 0 )
        {
            SysHead_hWnd = hWnd;
            LOGI("SysHead_hWnd searched: %p", SysHead_hWnd);
            return FALSE;
        }

    }
    return TRUE;
}

/* Поиск окна SysList в DefView */
BOOL CALLBACK SysList_rWEP( HWND hWnd, LPARAM lParam )
{
    char bufClass[256]; char bufTxt[256]; UINT u;
    u = RealGetWindowClassA( hWnd, bufClass, 255 ); bufClass[u] = '\0';
    u = GetWindowTextA( hWnd, bufTxt, 255 ); bufTxt[u] = '\0';
    if( strcmp( "SysListView32", bufClass ) == 0 )
    {
        if( strcmp( "FolderView", bufTxt ) == 0 )
        {
            SysList_hWnd = hWnd;
            LOGI("SysList_hWnd searched: %p", SysList_hWnd);
            EnumChildWindows( SysList_hWnd, SysHead_rWEP, 0 );
            if( SysList_hWnd == NULL )
            {
                LOGE( "Can't find SysHead window in SysList window" );
            }
            return FALSE;
        }

    }
    return TRUE;
}


/* Поиск окна DefView в Progman */
BOOL CALLBACK DefView_rWEP( HWND hWnd, LPARAM lParam )
{
    char bufClass[256]; char bufTxt[256]; UINT u;
    u = RealGetWindowClassA( hWnd, bufClass, 255 ); bufClass[u] = '\0';
    u = GetWindowTextA( hWnd, bufTxt, 255 ); bufTxt[u] = '\0';
    if( strcmp( "SHELLDLL_DefView", bufClass ) == 0 )
    {
        if( strcmp( "", bufTxt ) == 0 )
        {
            DefView_hWnd = hWnd;
            LOGI("DefView_hWnd searched: %p", DefView_hWnd);
            EnumChildWindows( DefView_hWnd, SysList_rWEP, 0 );
            if( SysList_hWnd == NULL )
            {
                LOGE( "Can't find SysList window in DefView window" );
            }
            return FALSE;
        }

    }
    return TRUE;
}

/* Поиск окна Progman в Desktop */
BOOL CALLBACK Progman_rWEP( HWND hWnd, LPARAM lParam )
{
    char bufClass[256]; char bufTxt[256]; UINT u;
    u = RealGetWindowClassA( hWnd, bufClass, 255 ); bufClass[u] = '\0';
    u = GetWindowTextA( hWnd, bufTxt, 255 ); bufTxt[u] = '\0';
    if( strcmp( "Progman", bufClass ) == 0 )
    {
        if( strcmp( "Program Manager", bufTxt ) == 0 )
        {
            Progman_hWnd = hWnd;
            LOGI("Progman_hWnd searched: %p", Progman_hWnd);
            EnumChildWindows( Progman_hWnd, DefView_rWEP, 0 );
            if( DefView_hWnd == NULL )
            {
                LOGE( "Can't find DefView window in Progman window" );
            }
            return FALSE;
        }

    }
    return TRUE;
}



