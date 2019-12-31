#include <sstream>
#include "..\WinGUI.h"

using namespace std;


CMainWindow mainWin;
CLabel labelStat;
CButton btnAddTab;
CButton btnRemTab;
CTabList tabBox;
CButton btnDec;
CButton btnInc;
CProgressBar progBar;

LRESULT MainWin_OnClose(CWindow*, const CWinEvent&);
LRESULT BtnAddTab_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnRemTab_OnClick(CWindow*, const CWinEvent&);
LRESULT Tab_OnSelChange(CWindow*, const CWinEvent&);
LRESULT BtnDec_OnClick(CWindow*, const CWinEvent&);
LRESULT BtnInc_OnClick(CWindow*, const CWinEvent&);

string ValToStr(double val)
//Convert the given number into a string
{
    return static_cast<ostringstream*>(&(ostringstream() << val))->str();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WPARAM retVal;
    
    if(!WinGUISetup())
        return 0;
    
    //Create main window
    mainWin.Create("Button Switch", 200, 200, 550, 450, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);
    
    //Create controls
    labelStat.Create(&mainWin, "", 50, 10, 200, 25, WS_VISIBLE | WS_CHILD | WS_BORDER);
    btnAddTab.Create(&mainWin, "Add Tab", 50, 45, 100, 30, WS_VISIBLE | WS_CHILD);
    btnRemTab.Create(&mainWin, "Remove Tab", 160, 45, 100, 30, WS_VISIBLE | WS_CHILD);
    tabBox.Create(&mainWin, "", 50, 100, 400, 300, WS_VISIBLE | WS_CHILD);
    btnDec.Create(&tabBox, "<<", 50, 100, 50, 25, WS_VISIBLE | WS_CHILD);
    btnInc.Create(&tabBox, ">>", 110, 100, 50, 25, WS_VISIBLE | WS_CHILD);
    progBar.Create(&tabBox, "", 50, 150, 200, 25, WS_VISIBLE | WS_CHILD);
    
    //Set the event handlers
    mainWin.AddEvent(WM_CLOSE, MainWin_OnClose, WINEVENT_MESSAGE);
    btnAddTab.AddEvent(BN_CLICKED, BtnAddTab_OnClick, WINEVENT_COMMAND);
    btnRemTab.AddEvent(BN_CLICKED, BtnRemTab_OnClick, WINEVENT_COMMAND);
    tabBox.AddEvent(TCN_SELCHANGE, Tab_OnSelChange, WINEVENT_NOTIFY);
    btnDec.AddEvent(BN_CLICKED, BtnDec_OnClick, WINEVENT_COMMAND);
    btnInc.AddEvent(BN_CLICKED, BtnInc_OnClick, WINEVENT_COMMAND);
    
    //Add some tabs
    tabBox.AddTab("Tab 0");
    tabBox.AddTab("Tab 1");
    
    //Set progress range
    progBar.SetRange(0, 10);
    
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
    
        //Signal the main loop to end
        PostQuitMessage(0);
    }
    
    return 0;
}

LRESULT BtnAddTab_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    int tabCount;
    
    tabCount = tabBox.GetTabCount();
    tabBox.AddTab(("Tab " + ValToStr(tabCount)).c_str());
    tabBox.SetSelIndex(tabCount);
    tabBox.SetTabText(tabCount, "NEW TAB");
    
    return 0;
}

LRESULT BtnRemTab_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    int tabCount;
    
    tabCount = tabBox.GetTabCount();
    tabBox.RemoveTab(tabCount-1);
    tabBox.SetSelIndex(tabCount-2);
    
    return 0;
}

LRESULT Tab_OnSelChange(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    int selIndex;
    string tabTextStr;
    
    selIndex = tabBox.GetSelIndex();
    tabBox.GetTabText(selIndex, tabTextStr);
    
    labelStat.SetText(("'" + tabTextStr + "' selected").c_str());
    
    if(selIndex == 0)
    {
        btnDec.Show();
        btnInc.Show();
        progBar.Show();
    }
    else
    {
        btnDec.Hide();
        btnInc.Hide();
        progBar.Hide();
    }
    
    return 0;
}

LRESULT BtnDec_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    int rangeMin;
    int rangeMax;
    int progPos;
    
    labelStat.SetText("Progress decreased.");
    
    progBar.GetRange(&rangeMin, &rangeMax);
    progPos = progBar.GetPos();
    if(progPos > rangeMin)
    {
        progBar.SetPos(progPos - 1);
    }
    
    return 0;
}

LRESULT BtnInc_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    int rangeMin;
    int rangeMax;
    int progPos;
    
    labelStat.SetText("Progress increased.");
    
    progBar.GetRange(&rangeMin, &rangeMax);
    progPos = progBar.GetPos();
    if(progPos < rangeMax)
    {
        progBar.SetPos(progPos + 1);
    }
    
    return 0;
}


