#include <sstream>
#include "..\WinGUI.h"

using namespace std;


const char* subProgStr[5] = {"MainWin_OnClose", "btnAdd_OnClick", "btnRemove_OnClick", "listBox_OnSelChange", "btnAdd_OnMouseMove"};

CMainWindow mainWin;
CListBox listBox;
CComboBox comboBox;
CEditBox editAdd;
CButton btnAdd;
CButton btnRemove;
CLabel labelStat;

LRESULT MainWin_OnClose(CWindow*, const CWinEvent&);
LRESULT btnAdd_OnClick(CWindow*, const CWinEvent&);
LRESULT btnRemove_OnClick(CWindow*, const CWinEvent&);
LRESULT listBox_OnSelChange(CWindow*, const CWinEvent&);
LRESULT btnAdd_OnMouseMove(CWindow*, const CWinEvent&);

string ValToStr(double);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WPARAM retVal;
    
    if(!WinGUISetup())
        return 0;
    
    //Create main window
    mainWin.Create("Button Switch", 200, 200, 550, 450, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);
    
    //Create controls
    labelStat.Create(&mainWin, "", 50, 10, 200, 25, WS_VISIBLE | WS_CHILD | WS_BORDER);
    editAdd.Create(&mainWin, "", 50, 50, 100, 25, WS_VISIBLE | WS_CHILD);
    btnAdd.Create(&mainWin, "Add", 170, 50, 80, 30, WS_VISIBLE | WS_CHILD | WS_BORDER);
    btnRemove.Create(&mainWin, "Remove", 260, 50, 80, 30, WS_VISIBLE | WS_CHILD | WS_BORDER);
    listBox.Create(&mainWin, "", 50, 100, 150, 200, WS_VISIBLE | WS_CHILD);
    comboBox.Create(&mainWin, "", 220, 100, 150, 200, WS_VISIBLE | WS_CHILD);
    
    //Set the event handlers
    mainWin.AddEvent(WM_CLOSE, MainWin_OnClose, WINEVENT_MESSAGE, 0);
    btnAdd.AddEvent(BN_CLICKED, btnAdd_OnClick, WINEVENT_COMMAND, 1);
    btnRemove.AddEvent(BN_CLICKED, btnRemove_OnClick, WINEVENT_COMMAND, 2);
    listBox.AddEvent(CBN_SELCHANGE, listBox_OnSelChange, WINEVENT_COMMAND, 3);
    btnAdd.AddEvent(WM_MOUSEMOVE, btnAdd_OnMouseMove, WINEVENT_MESSAGE, 4);
    
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
    
    labelStat.SetText(subProgStr[eventObj.userData]);
    
    return 0;
}

LRESULT btnAdd_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    string newItemStr;
    
    newItemStr = editAdd.GetText();
    listBox.InsertItem(newItemStr.c_str(), 0);
    
    labelStat.SetText(subProgStr[eventObj.userData]);
    
    return 0;
}

LRESULT btnRemove_OnClick(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    int itemIdx;
    
    itemIdx = listBox.GetSelIndex();
    listBox.RemoveItem(itemIdx);
    listBox.SetSelIndex(listBox.GetItemCount() - 1);
    
    labelStat.SetText(subProgStr[eventObj.userData]);
    
    return 0;
}

LRESULT listBox_OnSelChange(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    int itemIdx;
    string itemStr;
    
    itemIdx = listBox.GetSelIndex();
    listBox.GetItemText(itemIdx, itemStr);
    MessageBox(NULL, itemStr.c_str(), "ComboBox", MB_OK);
    
    labelStat.SetText(subProgStr[eventObj.userData]);
    
    return 0;
}

LRESULT btnAdd_OnMouseMove(CWindow* ctlPtr, const CWinEvent& eventObj)
//
{
    btnAdd.SetText("MOVE");
    
    labelStat.SetText(subProgStr[eventObj.userData]);
    
    return 0;
}

string ValToStr(double val)
//Convert the given number into a string
{
    return static_cast<ostringstream*>(&(ostringstream() << val))->str();
}


