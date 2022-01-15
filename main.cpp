
#include <stdio.h>>
#include <windows.h>
#include <gdiplus.h>
#include <math.h>

#define RADIAN_IN_ANGLE 0.01745329f // 180 / PI

static LONG    NumberOfPoints = 10;
static DOUBLE Circumference = 0.0;

static LRESULT WINAPI WindowProcedure(IN HWND hWindow, IN UINT nMessage, IN WPARAM wParam, IN LPARAM lParam) {
    if (nMessage == WM_KEYUP) {
        BOOL bUpdateWindow = FALSE;

        if (wParam == VK_UP) {
            NumberOfPoints += 10;
            bUpdateWindow = TRUE;
        } else if (wParam == VK_DOWN) {
            NumberOfPoints -= 10;

            if (NumberOfPoints < 10) {
                NumberOfPoints = 10;
            }

            bUpdateWindow = TRUE;
        }

        if (bUpdateWindow) {
            RECT Rect;

            GetClientRect(hWindow, &Rect);
            InvalidateRect(hWindow, &Rect, TRUE);
            UpdateWindow(hWindow);
        }

        return 0;
    } else if (nMessage == WM_PAINT) {
        FLOAT       fOriginX;
        FLOAT       fOriginY;
        FLOAT       fWidth;
        PAINTSTRUCT PaintStruct;
        RECT        Rect;
        FLOAT       fPointsAngleAppart;
        FLOAT       fAngle = 0.0f;
        HDC         hDc = BeginPaint(hWindow, &PaintStruct);

        GetClientRect(hWindow, &Rect);

        Gdiplus::Pen        CirclePen(Gdiplus::Color::Blue, 1.0f);
        Gdiplus::Pen        PointPen(Gdiplus::Color::Red, 1.0f);
        Gdiplus::Graphics   Gfx(hDc);

        if ((Rect.bottom - Rect.top) > (Rect.right - Rect.left)) {
            fWidth = (FLOAT)Rect.right - Rect.left;
        } else {
            fWidth = (FLOAT)Rect.bottom - Rect.top;
        }

        fWidth /= 2.0f;
        fOriginX = (FLOAT)(Rect.right - Rect.left) / 2.0f - fWidth / 2.0f;
        fOriginY = (FLOAT)(Rect.bottom - Rect.top) / 2.0f - fWidth / 2.0f;

        Gfx.DrawEllipse(&CirclePen, fOriginX, fOriginY, fWidth, fWidth);

        fPointsAngleAppart = 360.0f / (FLOAT)NumberOfPoints;

        Circumference = 0.0;

        for (ULONG_PTR i = 0; i != NumberOfPoints; i++) {
            DOUBLE fStartingX = fOriginX + fWidth / 2;
            DOUBLE fStartingY = fOriginY + fWidth / 2;
            DOUBLE fEndingOfNextX = (fWidth / 2.0f) * sin((fAngle + fPointsAngleAppart) * RADIAN_IN_ANGLE);
            DOUBLE fEndingOfNextY = (fWidth / 2.0f) * cos((fAngle + fPointsAngleAppart) * RADIAN_IN_ANGLE);
            DOUBLE fEndingX = (fWidth / 2.0f) * sin(fAngle * RADIAN_IN_ANGLE);
            DOUBLE fEndingY = (fWidth / 2.0f) * cos(fAngle * RADIAN_IN_ANGLE);

            fEndingX = fStartingX + fEndingX;
            fEndingY = fStartingY + fEndingY;
            fEndingOfNextX = fStartingX + fEndingOfNextX;
            fEndingOfNextY = fStartingY + fEndingOfNextY;

            Gfx.DrawLine(&PointPen, Gdiplus::PointF(fStartingX, fStartingY), Gdiplus::PointF(fEndingX, fEndingY));
            Gfx.DrawLine(&PointPen, Gdiplus::PointF(fEndingX, fEndingY), Gdiplus::PointF(fEndingOfNextX, fEndingOfNextY));

            Circumference += sqrt(pow(fEndingX - fEndingOfNextX, 2) + pow(fEndingY - fEndingOfNextY, 2));

            fAngle += fPointsAngleAppart;
        }

        printf("Circumference: %lf\nPI:%lf (3.1415926535897932384626433832795)\n", Circumference, Circumference / ((DOUBLE)fWidth / 2.0) / 2.0);

        EndPaint(hWindow, &PaintStruct);

        return 0;
    } else if (nMessage == WM_CLOSE) {
        DestroyWindow(hWindow);

        return 0;
    } else if (nMessage == WM_DESTROY) {
        PostQuitMessage(0);

        return 0;
    }

    return DefWindowProcW(hWindow, nMessage, wParam, lParam);
}

INT wmain() {
    #define WINDOW_CLASS L"{30E7977B-7A83-4A27-A26B-12D62CF95D19}"

    HWND                         hWindow;
    MSG                          Message;
    LONG                         dwMessageCode;
    WNDCLASSW                    WindowClassW = { 0 };
    ULONG_PTR                    dwToken = 0;
    HINSTANCE                    hInstance = GetModuleHandleW(NULL);
    Gdiplus::GdiplusStartupInput GdiplusStartupInput;
    
    printf("This is a proof that PI is infinitive long by using the Archimedes method.\nThe more points you have the closest it gets to pi.\nARROW UP - Incerase the number of points\nARROW DOWN - Decrese the number of points\n");

    WindowClassW.style = CS_HREDRAW | CS_VREDRAW;
    WindowClassW.lpfnWndProc = WindowProcedure;
    WindowClassW.hInstance = hInstance;
    WindowClassW.hCursor = LoadCursorW(NULL, IDC_ARROW);
    WindowClassW.lpszClassName = WINDOW_CLASS;
    WindowClassW.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

    Gdiplus::GdiplusStartup(&dwToken, &GdiplusStartupInput, NULL);

    RegisterClassW(&WindowClassW);

    if (!(hWindow = CreateWindowExW(0, WINDOW_CLASS, L"Infinitive PI", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL))) {
        return NO_ERROR;
    }

    UpdateWindow(hWindow);
    ShowWindow(hWindow, SW_SHOW);

    while ((dwMessageCode = GetMessageW(&Message, NULL, 0, 0))) {
        if (dwMessageCode < 0) {
            break;
        }

        TranslateMessage(&Message);
        DispatchMessageW(&Message);
    }

    #undef WINDOW_CLASS

    return NO_ERROR;
}
