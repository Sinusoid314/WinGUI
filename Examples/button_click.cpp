#include "..\WinGUI.h"

using namespace std;


CMainWindow mainWin;
CButton btnTarget;

LRESULT MainWin_OnClose(CWindow*, const CWinEvent&);
LRESULT Button_OnClick(CWindow*, const CWinEvent&);
LRESULT Button_OnMouseMove(CWindow*, const CWinEvent&);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WPARAM retVal;
    
    if(!WinGUISetup())
        return 0;
    
    //Create main window
    mainWin.Create("Button Click", 200, 200, 500, 300, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);
    
    //Create a button
    btnTarget.Create(&mainWin, "", 100, 100, 150, 40, WS_VISIBLE | WS_CHILD);
    
    //Set the event handlers
    mainWin.AddEvent(WM_CLOSE, MainWin_OnClose, WINEVENT_MESSAGE);
    btnTarget.AddEvent(BN_CLICKED, Button_OnClick, WINEVENT_COMMAND);
    btnTarget.AddEvent(WM_MOUSEMOVE, Button_OnMouseMove, WINEVENT_MESSAGE);
    
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
	//Destroy main window (will destroy its child windows automatically)
    winPtr->Destroy();
    
    //Signal the main loop to end
    PostQuitMessage(0);
    
    return 0;
}

LRESULT Button_OnClick(CWindow* btnPtr, const CWinEvent& eventObj)
//
{
    btnPtr->SetText("Clicked!");
    btnPtr->RemoveEvent(BN_CLICKED, WINEVENT_COMMAND);
    btnPtr->RemoveEvent(WM_MOUSEMOVE, WINEVENT_MESSAGE);
    
    return 0;
}

LRESULT Button_OnMouseMove(CWindow* btnPtr, const CWinEvent& eventObj)
//
{
    int btnLeft;
    int btnTop;
    int winWidth;
    int winHeight;
    btnPtr->SetText("Mouse Moved.");
    mainWin.GetClientSize(&winWidth, &winHeight);
    
    btnLeft = rand() % winWidth;
    btnTop = rand() % winHeight;
    
    btnPtr->SetPos(btnLeft, btnTop);
    
    return 0;
}


