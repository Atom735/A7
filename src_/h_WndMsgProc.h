/* Обработчик сообщений окна */
LRESULT WINAPI This_WndMsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{

    static BOOL This_bRelease = TRUE;
    POINT mpMouse;
    GetCursorPos( &mpMouse );
    if( This_bRelease )
    {
        BLENDFUNCTION blend = {0};
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = 0x3f;
        blend.AlphaFormat = AC_SRC_ALPHA;

        POINT ptPos = {0, 0};
        SIZE sizeWnd = {800, 600};
        POINT ptSrc = {0, 0};

        {
            RECT rc = { 0, 0, 800, 600 };
            FillRect( This_hDCS, &rc, (HBRUSH) (COLOR_WINDOW) );
            rc.left = mpMouse.x;
            rc.top = mpMouse.y;
            FillRect( This_hDCS, &rc, (HBRUSH) (COLOR_WINDOW+1) );
        }

        UpdateLayeredWindow( This_hWnd, This_hDCS, &ptPos, &sizeWnd, This_hDC, &ptSrc, 0, &blend, ULW_ALPHA);
        ValidateRect( This_hWnd, NULL );
    }


    if( ( uMsg == WM_CLOSE ) || ( uMsg == WM_DESTROY ) )
    {
        if( This_bRelease )
        {
            SelectObject( This_hDC, This_hBmpOld );
            DeleteObject( This_hBmp );
            DeleteDC( This_hDC );
            ReleaseDC( This_hWnd, This_hDCS );
            SetWindowLong( This_hWnd, GWL_EXSTYLE, GetWindowLong( This_hWnd, GWL_EXSTYLE ) & ( ~WS_EX_LAYERED ) );

            SetParent( C_hWnd, P_hWnd );

            This_bRelease = FALSE;
        }
    }
    // LOGV( "MSG: to hWnd[%p] % 8d w[%p] l[%p]", hWnd, uMsg, wParam, lParam );
    switch( uMsg )
    {
        /* Обработка закрытия окна */
        case WM_CLOSE:
            DestroyWindow( hWnd );
            return 0;
        /* Обработка уничтожения окна */
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
