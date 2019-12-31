#include <sstream>
#include "..\WinGUI.h"

using namespace std;


CImage* imgPtr;
CMainWindow mainWin;
CDrawBox drawBox1;
CDrawBox drawBox2;
CButton btnRedraw;
CButton btnRedrawArea;
CButton btnStick;
CButton btnStickArea;
CButton btnDrawRect;
CButton btnDrawText;
CButton btnDrawDots;
CButton btnResize;
CButton btnCopyArea;

LRESULT MainWin_OnClose(CWindow*, const CWinEvent&);
LRESULT BtnRedraw_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnRedrawArea_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnStick_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnStickArea_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnDrawRect_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnDrawText_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnDrawDots_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnResize_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnCopyArea_OnClick(CWindow*, const CWinEvent&);

string ValToStr(double);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WPARAM retVal;

    if(!WinGUISetup())
        return 0;

    imgPtr = new CImage;
    imgPtr->Load("FrigButton.bmp");

    //Create main window
    mainWin.Create("DrawBox Test", 200, 200, 650, 500, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);

    //Create controls
    drawBox1.Create(&mainWin, 10, 90, 300, 300, WS_VISIBLE | WS_CHILD | WS_BORDER);
    drawBox2.Create(&mainWin, 320, 90, 300, 300, WS_VISIBLE | WS_CHILD | WS_BORDER);
    btnRedraw.Create(&mainWin, "Redraw", 10, 10, 100, 30, WS_VISIBLE | WS_CHILD);
    btnRedrawArea.Create(&mainWin, "Redraw Area", 120, 10, 100, 30, WS_VISIBLE | WS_CHILD);
    btnStick.Create(&mainWin, "Stick", 10, 50, 100, 30, WS_VISIBLE | WS_CHILD);
    btnStickArea.Create(&mainWin, "Stick Area", 120, 50, 100, 30, WS_VISIBLE | WS_CHILD);
    btnDrawRect.Create(&mainWin, "Draw Rect", 230, 10, 100, 30, WS_VISIBLE | WS_CHILD);
    btnDrawText.Create(&mainWin, "Draw Text", 230, 50, 100, 30, WS_VISIBLE | WS_CHILD);
    btnDrawDots.Create(&mainWin, "Draw Dots", 340, 10, 100, 30, WS_VISIBLE | WS_CHILD);
    btnResize.Create(&mainWin, "Resize", 340, 50, 100, 30, WS_VISIBLE | WS_CHILD);
    btnCopyArea.Create(&mainWin, "Copy Area", 450, 10, 100, 30, WS_VISIBLE | WS_CHILD);

    //Set the event handlers
    mainWin.AddEvent(WM_CLOSE, MainWin_OnClose, WINEVENT_MESSAGE);
    btnRedraw.AddEvent(BN_CLICKED, BtnRedraw_OnClick, WINEVENT_COMMAND);
    btnRedrawArea.AddEvent(BN_CLICKED, BtnRedrawArea_OnClick, WINEVENT_COMMAND);
    btnStick.AddEvent(BN_CLICKED, BtnStick_OnClick, WINEVENT_COMMAND);
    btnStickArea.AddEvent(BN_CLICKED, BtnStickArea_OnClick, WINEVENT_COMMAND);
    btnDrawRect.AddEvent(BN_CLICKED, BtnDrawRect_OnClick, WINEVENT_COMMAND);
    btnDrawText.AddEvent(BN_CLICKED, BtnDrawText_OnClick, WINEVENT_COMMAND);
    btnDrawDots.AddEvent(BN_CLICKED, BtnDrawDots_OnClick, WINEVENT_COMMAND);
    btnResize.AddEvent(BN_CLICKED, BtnResize_OnClick, WINEVENT_COMMAND);
    btnCopyArea.AddEvent(BN_CLICKED, BtnCopyArea_OnClick, WINEVENT_COMMAND);

    //Display the main window
    mainWin.Show();

    //Run message loop
    retVal = InputWinEvents();

    WinGUICleanup();

    return retVal;
}

LRESULT MainWin_OnClose(CWindow* winPtr, const CWinEvent& eventObj)
//
{
    if(MessageBox(winPtr->winHandle, "Are you sure you want to exit?", "WinGUI Example", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
    {
        //Destroy main window
        winPtr->Destroy();

        delete imgPtr;

        //Signal the main loop to end
        PostQuitMessage(0);
    }

    return 0;
}

LRESULT BtnRedraw_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    drawBox1.Redraw();
    return 0;
}

LRESULT BtnRedrawArea_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    drawBox1.RedrawArea(100, 100, 100, 100);
    return 0;
}

LRESULT BtnStick_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    drawBox1.Stick();
    return 0;
}

LRESULT BtnStickArea_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    drawBox1.StickArea(100, 100, 100, 100);
    return 0;
}

LRESULT BtnDrawRect_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    int boxWidth;
    int boxHeight;

    drawBox1.GetSize(&boxWidth, &boxHeight);

    drawBox1.SetPenStyle(PS_NULL);
    drawBox1.SetBrushStyle(BS_SOLID);
    drawBox1.SetBrushColor(RGB(76,146,111));

    drawBox1.DrawRect(0, 0, boxWidth, boxHeight);

    return 0;
}

LRESULT BtnDrawText_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    string prevFontNameStr;
    BYTE prevFontItalic;
    BYTE prevFontUnderline;
    BYTE prevFontStrikeOut;

    drawBox1.SetForeColor(RGB(20,100,200));
    drawBox1.SetBackMode(TRANSPARENT);

    drawBox1.DrawText("Allo World!", 50, 50);

    prevFontNameStr = drawBox1.GetFontName();
    drawBox1.GetFontStyle(&prevFontItalic, &prevFontUnderline, &prevFontStrikeOut);

    drawBox1.SetFontName("Segoe Print");
    drawBox1.SetFontStyle(TRUE, TRUE, TRUE);

    drawBox1.DrawText("Allo World!", 50, 70);

    drawBox1.SetFontName(prevFontNameStr.c_str());
    drawBox1.SetFontStyle(prevFontItalic, prevFontUnderline, prevFontStrikeOut);

    return 0;
}

LRESULT BtnDrawDots_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    COLORREF currColor;

    for(int x=100; x<=200; x+=8)
    {
        for(int y=100; y<=200; y+=8)
        {
            currColor = drawBox1.GetPoint(x, y);
            currColor += 400;
            drawBox1.SetPoint(x, y, currColor);
        }
    }

    return 0;
}

LRESULT BtnResize_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    int boxWidth;
    int boxHeight;

    drawBox1.GetSize(&boxWidth, &boxHeight);
    boxWidth += 50;
    boxHeight += 50;
    drawBox1.SetSize(boxWidth, boxHeight);

    return 0;
}

LRESULT BtnCopyArea_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    drawBox1.CopyAreaTo(&drawBox2, 0, 0, 50, 50, 200, 200);

    return 0;
}


