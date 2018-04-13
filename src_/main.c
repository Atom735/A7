#include "h_inc.h"
#include "h_vars.h"

#include "h_LOG.h"
#include "h_WndEnumProc.h"
#include "h_WndMsgProc.h"


LRESULT CALLBACK LowLevelMouseProc( INT nCode, WPARAM wParam, LPARAM lParam )
{
    //
    FILE *hF = fopen("F:/F.txt", "w+");
    fprintf( hF, "% 10i, %08x, %08x\n", nCode, wParam, lParam );
    fclose(hF);
    return 0;
}

/* Точка входа */
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, INT iCmd)
{


    #include "h__finds_windows.h"

    HWND hWnd = GetNextWindow( Progman_hWnd, GW_HWNDPREV );
    SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong( hWnd, GWL_EXSTYLE ) | WS_EX_LAYERED );
    HDC hDCS = GetDC( hWnd );
    HDC hDC = CreateCompatibleDC( hDCS );
    HBITMAP hBmp = CreateCompatibleBitmap(hDCS, 800, 600);
    HBITMAP hBmpOld = (HBITMAP)SelectObject(hDC, hBmp);

    DWORD dwThread = GetWindowThreadProcessId( hWnd, NULL );
    HHOOK hHook = SetWindowsHookEx( WH_MOUSE_LL, LowLevelMouseProc, NULL, dwThread );
    HHOOK hHook2 =  SetWindowsHook( WH_MOUSE_LL, LowLevelMouseProc );


    // Call UpdateLayeredWindow
    BLENDFUNCTION blend = {0};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 0xff;
    blend.AlphaFormat = AC_SRC_ALPHA;

    POINT ptPos = {0, 0};
    SIZE sizeWnd = {800, 600};
    POINT ptSrc = {0, 0};

    for (int i = 0; i < 1000; ++i)
    {
        RECT rc = { 0, 0, 800, 600 };
        FillRect( hDC, &rc, (HBRUSH)( COLOR_WINDOW ) );

        LOGI( "MP: %i, %i", mpMouse.x, mpMouse.y );
        rc.left = mpMouse.x;
        rc.top = mpMouse.y;
        FillRect( hDC, &rc, (HBRUSH)( COLOR_WINDOW + 1 ) );

        UpdateLayeredWindow( hWnd, hDCS, &ptPos, &sizeWnd, hDC, &ptSrc, 0, &blend, ULW_ALPHA );
        Sleep( 10 );
    }

    SelectObject(hDC, hBmpOld);
    DeleteObject(hBmp);
    DeleteDC(hDC);
    ReleaseDC(hWnd, hDCS);
    SetWindowLong( hWnd, GWL_EXSTYLE, GetWindowLong( hWnd, GWL_EXSTYLE ) & ( ~WS_EX_LAYERED ) );

    UnhookWindowsHook( WH_MOUSE_LL, LowLevelMouseProc );
    UnhookWindowsHookEx( hHook );
    return 0;
    #include "h__loop.h"

    return 0;
}
