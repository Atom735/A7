{
    WNDCLASSEX wc;
    {
        ZeroMemory( &wc, sizeof( wc ) );
        // UINT  cbSize;
        // UINT  style;
        // WNDPROC  lpfnWndProc;
        // int  cbClsExtra;
        // int  cbWndExtra;
        // HINSTANCE  hInstance;
        // HICON  hIcon;
        // HCURSOR  hCursor;
        // HBRUSH  hbrBackground;
        // LPCTSTR  lpszMenuName;
        // LPCTSTR  lpszClassName;
        // HICON  hIconSm;
        wc.cbSize           = sizeof( wc );
        wc.style            = 0/*CS_CLASSDC*/;
        wc.lpfnWndProc      = This_WndMsgProc;
        wc.hInstance        = hInst;
        wc.hbrBackground    = (HBRUSH) (COLOR_WINDOW);
        wc.lpszClassName    = TEXT("C-Atom735");
    }
    RegisterClassEx( &wc );

    This_hWnd = CreateWindowEx( WS_EX_OVERLAPPEDWINDOW , wc.lpszClassName, TEXT("W-Atom735"),
        WS_SIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0, 800, 600, NULL, NULL, wc.hInstance, NULL );

    /* Проверяем созданно ли */
    if( This_hWnd == NULL )
    {
        INT c = __COUNTER__ + 0xfefe0000;
        LOGF( "ERROR: 0x%x: Can't create this window", c );
        UnregisterClass( wc.lpszClassName, wc.hInstance );
        exit( c );
    }
    ShowWindow( This_hWnd, SW_SHOW );
    UpdateWindow( This_hWnd );


    C_hWnd = Progman_hWnd;

    P_hWnd = SetParent( C_hWnd, This_hWnd );

    SetWindowLong(This_hWnd, GWL_STYLE, GetWindowLong( This_hWnd, GWL_STYLE) | WS_CHILD );

    SetWindowLong( This_hWnd, GWL_EXSTYLE, GetWindowLong( This_hWnd, GWL_EXSTYLE ) | WS_EX_LAYERED );
    This_hDCS       = GetDC( This_hWnd );
    This_hDC        = CreateCompatibleDC( This_hDCS );
    This_hBmp       = CreateCompatibleBitmap( This_hDCS, 800, 600 );
    This_hBmpOld    = (HBITMAP)SelectObject( This_hDC, This_hBmp );
    // SetParent( This_hWnd, Desktop_hWnd );

    // SetWindowPos( DefView_hWnd, NULL, 0, 0, 1280, 1024, SWP_DRAWFRAME | SWP_NOZORDER );

    // InitCommonControls();

    MSG msg;
    ZeroMemory( &msg, sizeof( msg ) );
    while( msg.message != WM_QUIT )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            // LOGV( "% 10d MSG: to hWnd[%p] % 8d w[%p] l[%p]", msg.time, msg.hwnd, msg.message, msg.wParam, msg.lParam );
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }
    UnregisterClass( wc.lpszClassName, wc.hInstance );
    return msg.wParam;
}
