#include <sstream>
#include "..\WinGUI.h"

using namespace std;


CImage* bgImgPtr;
CSpriteFrameSheet* bubbaSheetPtr;
CSpriteFrameSheet* carSheetPtr;
CSpriteFrameSheet* shipSheetPtr;
CSpriteFrameSheet* magusSheetPtr;
CSprite* bubbaSpritePtr;
CSprite* carSpritePtr;
CSprite* shipSpritePtr;
CSprite* magusSpritePtr;
CMainWindow mainWin;
CDrawBox viewBox;
CButton btnDraw;
CButton btnPos;
CButton btnSize;
CButton btnVisible;
CButton btnPlaying;
CButton btnMirrX;
CButton btnMirrY;
CButton btnSpeed;
CButton btnTicks;
CButton btnCycles;
CButton btnRange;

void SetupSprites(void);
void CleanupSprites(void);
WPARAM DrawLoop(void);
LRESULT MainWin_OnClose(CWindow*, const CWinEvent&);
LRESULT BtnDraw_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnPos_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnSize_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnVisible_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnPlaying_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnMirrX_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnMirrY_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnSpeed_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnTicks_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnCycles_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnRange_OnClick(CWindow*, const CWinEvent&);

string ValToStr(double);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WPARAM retVal;

    if(!WinGUISetup())
        return 0;

    SetupSprites();

    //Create main window
    mainWin.Create("Sprite Test", 300, 100, 750, 545, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);

    //Create controls
    viewBox.Create(&mainWin, 15, 90, 700, 400, WS_VISIBLE | WS_CHILD | WS_BORDER);
    btnDraw.Create(&mainWin, "Draw Frame", 10, 10, 100, 30, WS_VISIBLE | WS_CHILD);
    btnPos.Create(&mainWin, "Position", 120, 10, 100, 30, WS_VISIBLE | WS_CHILD);
    btnSize.Create(&mainWin, "Size", 230, 10, 100, 30, WS_VISIBLE | WS_CHILD);
    btnVisible.Create(&mainWin, "Visible", 340, 10, 100, 30, WS_VISIBLE | WS_CHILD);
    btnPlaying.Create(&mainWin, "Playing", 450, 10, 100, 30, WS_VISIBLE | WS_CHILD);
    btnMirrX.Create(&mainWin, "Mirrored X", 10, 50, 100, 30, WS_VISIBLE | WS_CHILD);
    btnMirrY.Create(&mainWin, "Mirrored Y", 120, 50, 100, 30, WS_VISIBLE | WS_CHILD);
    btnSpeed.Create(&mainWin, "Speed", 230, 50, 100, 30, WS_VISIBLE | WS_CHILD);
    btnTicks.Create(&mainWin, "Frame Ticks", 340, 50, 100, 30, WS_VISIBLE | WS_CHILD);
    btnCycles.Create(&mainWin, "Max Cycles", 450, 50, 100, 30, WS_VISIBLE | WS_CHILD);
    btnRange.Create(&mainWin, "Frame Range", 560, 10, 100, 30, WS_VISIBLE | WS_CHILD);

    //Set the event handlers
    mainWin.AddEvent(WM_CLOSE, MainWin_OnClose, WINEVENT_MESSAGE);
    btnDraw.AddEvent(BN_CLICKED, BtnDraw_OnClick, WINEVENT_COMMAND);
    btnPos.AddEvent(BN_CLICKED, BtnPos_OnClick, WINEVENT_COMMAND);
    btnSize.AddEvent(BN_CLICKED, BtnSize_OnClick, WINEVENT_COMMAND);
    btnVisible.AddEvent(BN_CLICKED, BtnVisible_OnClick, WINEVENT_COMMAND);
    btnPlaying.AddEvent(BN_CLICKED, BtnPlaying_OnClick, WINEVENT_COMMAND);
    btnMirrX.AddEvent(BN_CLICKED, BtnMirrX_OnClick, WINEVENT_COMMAND);
    btnMirrY.AddEvent(BN_CLICKED, BtnMirrY_OnClick, WINEVENT_COMMAND);
    btnSpeed.AddEvent(BN_CLICKED, BtnSpeed_OnClick, WINEVENT_COMMAND);
    btnTicks.AddEvent(BN_CLICKED, BtnTicks_OnClick, WINEVENT_COMMAND);
    btnCycles.AddEvent(BN_CLICKED, BtnCycles_OnClick, WINEVENT_COMMAND);
    btnRange.AddEvent(BN_CLICKED, BtnRange_OnClick, WINEVENT_COMMAND);

    //Draw sprites to the back buffer
    viewBox.UseRedrawDC();

    //Display the main window
    mainWin.Show();

    //Run drawing loop
    retVal = DrawLoop();

    CleanupSprites();

    WinGUICleanup();

    return retVal;
}

void SetupSprites(void)
//Load and initialize sprites
{
    //Load background image and frame sheets
    bgImgPtr = new CImage("bg2.bmp");
    bubbaSheetPtr = new CSpriteFrameSheet("bubba.bmp", RGB(255,0,128), 1, 2);
    carSheetPtr = new CSpriteFrameSheet("car.bmp", RGB(255,0,128), 1, 1);
    shipSheetPtr = new CSpriteFrameSheet("ship.bmp", RGB(255,0,128), 1, 2);
    magusSheetPtr = new CSpriteFrameSheet("magus.bmp", RGB(255,0,128), 1, 5);

    //Create sprites
    bubbaSpritePtr = new CSprite(bubbaSheetPtr);
    carSpritePtr = new CSprite(carSheetPtr);
    shipSpritePtr = new CSprite(shipSheetPtr);
    magusSpritePtr = new CSprite(magusSheetPtr);

    //Set sprite properties
    bubbaSpritePtr->drawLeft = 100; bubbaSpritePtr->drawTop = 200;
    bubbaSpritePtr->ticksPerFrame = 300;
    carSpritePtr->drawLeft = 350; carSpritePtr->drawTop = 210;
    shipSpritePtr->drawLeft = 100; shipSpritePtr->drawTop = 50;
    shipSpritePtr->ticksPerFrame = 200;
    magusSpritePtr->drawLeft = 500; magusSpritePtr->drawTop = 220;
    magusSpritePtr->ticksPerFrame = 200;
    magusSpritePtr->SetFrameRange(3, 4);
}

void CleanupSprites(void)
//Delete sprites and images
{
    //Unload sprites
    delete magusSpritePtr;
    delete shipSpritePtr;
    delete carSpritePtr;
    delete bubbaSpritePtr;

    //Unload images
    delete magusSheetPtr;
    delete shipSheetPtr;
    delete carSheetPtr;
    delete bubbaSheetPtr;
    delete bgImgPtr;
}

WPARAM DrawLoop(void)
//Do drawing operations and process messages, returning when WM_QUIT is received
{
    MSG msgInfo;

    while(true)
    {
        FlushWinEvents(&msgInfo);

        if(msgInfo.message == WM_QUIT)
            return msgInfo.wParam;

        viewBox.DrawImage(bgImgPtr, 0, 0);
        bubbaSpritePtr->Draw(&viewBox);
        carSpritePtr->Draw(&viewBox);
        shipSpritePtr->Draw(&viewBox);
        magusSpritePtr->Draw(&viewBox);
        viewBox.Redraw();

        Sleep(1);
    }
}

LRESULT MainWin_OnClose(CWindow* winPtr, const CWinEvent& eventObj)
//
{
    if(MessageBox(winPtr->winHandle, "Are you sure you want to exit?", "WinGUI Example", MB_OKCANCEL | MB_ICONQUESTION) == IDOK)
    {
        //Destroy main window
        winPtr->Destroy();

        //Signal the main loop to end
        PostQuitMessage(0);
    }

    return 0;
}

LRESULT BtnDraw_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{

    return 0;
}

LRESULT BtnPos_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{

    return 0;
}

LRESULT BtnSize_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    magusSpritePtr->drawWidth = (magusSpritePtr->GetFrameSheet()->frameWidth) * 2;
    magusSpritePtr->drawHeight = (magusSpritePtr->GetFrameSheet()->frameHeight) * 2;
    return 0;
}

LRESULT BtnVisible_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    bubbaSpritePtr->isVisible = !(bubbaSpritePtr->isVisible);
    return 0;
}

LRESULT BtnPlaying_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    bubbaSpritePtr->isPlaying = !(bubbaSpritePtr->isPlaying);
    return 0;
}

LRESULT BtnMirrX_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    bubbaSpritePtr->isMirroredX = !(bubbaSpritePtr->isMirroredX);
    return 0;
}

LRESULT BtnMirrY_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    bubbaSpritePtr->isMirroredY = !(bubbaSpritePtr->isMirroredY);
    return 0;
}

LRESULT BtnSpeed_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    //bubbaSpritePtr->
    return 0;
}

LRESULT BtnTicks_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    bubbaSpritePtr->ticksPerFrame = 900;
    return 0;
}

LRESULT BtnCycles_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    bubbaSpritePtr->maxCycles = 5;
    return 0;
}

LRESULT BtnRange_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    magusSpritePtr->SetFrameRange(0, 2);
    return 0;
}


