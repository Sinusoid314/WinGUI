#include "..\WinGUI.h"

using namespace std;

CImage* imgPtr;
CMainWindow mainWin;
CGroupBox groupOne;
CGroupBox groupTwo;
CButton btnOne;
CButton btnTwo;
CRadioButton rBtnThree;
CRadioButton rBtnFour;
CRadioButton rBtnFive;
CCheckBox cBtnSix;
CLabel labelInfo;

LRESULT MainWin_OnClose(CWindow*, const CWinEvent&);
LRESULT btnOne_OnClick(CWindow*, const CWinEvent&);
LRESULT btnTwo_OnClick(CWindow*, const CWinEvent&);
LRESULT rBtnFour_OnClick(CWindow*, const CWinEvent&);
LRESULT cBtnSix_OnClick(CWindow*, const CWinEvent&);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WPARAM retVal;

    if(!WinGUISetup())
        return 0;

    imgPtr = new CImage;
    imgPtr->Load("FrigButton.bmp");
    //imgPtr->Load("frig", true);

    //Create main window
    mainWin.Create("Button Switch", 200, 200, 550, 450, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);

    //Create controls
    labelInfo.Create(&mainWin, "", 200, 50, 100, 25, WS_VISIBLE | WS_CHILD | WS_BORDER);
    groupOne.Create(&mainWin, "Group 1", 50, 100, 200, 200, WS_VISIBLE | WS_CHILD);
    groupTwo.Create(&mainWin, "Group 2", 260, 100, 200, 200, WS_VISIBLE | WS_CHILD);
    btnOne.Create(&groupOne, "Button 1", 50, 60, 100, 30, WS_VISIBLE | WS_CHILD);
    btnTwo.Create(&groupOne, "Button 2", 50, 100, 100, 40, WS_VISIBLE | WS_CHILD | BS_BITMAP);
    rBtnThree.Create(&groupOne, "Button 3", 50, 160, 100, 20, WS_VISIBLE | WS_CHILD);
    rBtnFour.Create(&groupTwo, "Button 4", 50, 60, 100, 20, WS_VISIBLE | WS_CHILD);
    rBtnFive.Create(&groupTwo, "Button 5", 50, 100, 100, 20, WS_VISIBLE | WS_CHILD);
    cBtnSix.Create(&groupTwo, "Button 6", 50, 140, 100, 20, WS_VISIBLE |WS_CHILD);
    btnTwo.SetImage(imgPtr);

    //Set the event handlers
    mainWin.AddEvent(WM_CLOSE, MainWin_OnClose, WINEVENT_MESSAGE);
    btnOne.AddEvent(BN_CLICKED, btnOne_OnClick, WINEVENT_COMMAND);
    btnTwo.AddEvent(BN_CLICKED, btnTwo_OnClick, WINEVENT_COMMAND);
    rBtnFour.AddEvent(BN_CLICKED, rBtnFour_OnClick, WINEVENT_COMMAND);
    cBtnSix.AddEvent(BN_CLICKED, cBtnSix_OnClick, WINEVENT_COMMAND);

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
	//Destroy main window
    winPtr->Destroy();

    //Unload image
    delete imgPtr;

    //Signal the main loop to end
    PostQuitMessage(0);

    return 0;
}

LRESULT btnOne_OnClick(CWindow* btnPtr, const CWinEvent& eventObj)
//
{
    int btnState;

    btnState = (cBtnSix.GetState() ? 0 : 1);
    cBtnSix.SetState(btnState);
    labelInfo.SetText("* ONE *");
    return 0;
}

LRESULT btnTwo_OnClick(CWindow* btnPtr, const CWinEvent& eventObj)
//
{
    labelInfo.SetText("* TWO *");
    return 0;
}

LRESULT rBtnFour_OnClick(CWindow* btnPtr, const CWinEvent& eventObj)
//
{
    int btnState;

    btnState = rBtnFour.GetState();
    rBtnThree.SetState(btnState);
    labelInfo.SetText("* THREE *");
    return 0;
}

LRESULT cBtnSix_OnClick(CWindow* btnPtr, const CWinEvent& eventObj)
//
{
    int btnState;

    btnState = cBtnSix.GetState();
    rBtnThree.SetState(btnState);
    labelInfo.SetText("* SIX *");
    return 0;
}


