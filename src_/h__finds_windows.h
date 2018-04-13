{
    Desktop_hWnd = GetDesktopWindow();
    LOGI("Desktop_hWnd: %p", Desktop_hWnd);
    EnumChildWindows( Desktop_hWnd, Progman_rWEP, 0 );
    if( Desktop_hWnd == NULL )
    {
        INT c = __COUNTER__ + 0xfefe0000;
        LOGF( "ERROR: 0x%x: Can't find Desktop window", c );
        // exit( c );
    }
    if( Progman_hWnd == NULL )
    {
        INT c = __COUNTER__ + 0xfefe0000;
        LOGF( "ERROR: 0x%x: Can't find Progman window", c );
        // exit( c );
    }
    if( DefView_hWnd == NULL )
    {
        INT c = __COUNTER__ + 0xfefe0000;
        LOGF( "ERROR: 0x%x: Can't find DefView window", c );
        // exit( c );
    }
    if( SysList_hWnd == NULL )
    {
        INT c = __COUNTER__ + 0xfefe0000;
        LOGF( "ERROR: 0x%x: Can't find SysList window", c );
        // exit( c );
    }
    if( SysHead_hWnd == NULL )
    {
        INT c = __COUNTER__ + 0xfefe0000;
        LOGF( "ERROR: 0x%x: Can't find SysHead window", c );
        // exit( c );
    }
}
