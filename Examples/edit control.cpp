#include <sstream>
#include "..\WinGUI.h"

using namespace std;


CMainWindow mainWin;
CEditBox editBox;
CEditBox editLineIdx;
CEditBox editLineInfo;
CButton btnGetInfo;

LRESULT MainWin_OnClose(CWindow*, const CWinEvent&);
LRESULT GetInfoButton_OnClick(CWindow*, const CWinEvent&);

string NumToStr(double val)
//Convert the given number into a string
{
    return static_cast<ostringstream*>(&(ostringstream() << val))->str();
}

double StrToNum(const string& str)
//Convert a string object to a number
{
	double tmpNum;
    istringstream(str) >> tmpNum;
    return tmpNum;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WPARAM retVal;
    
    if(!WinGUISetup())
        return 0;
    
    //Create main window
    mainWin.Create("Edit Control", 200, 200, 550, 350, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN);
    
    //Create controls
    editBox.Create(&mainWin, "", 50, 50, 200, 200, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_NOHIDESEL);
    editLineIdx.Create(&mainWin, "", 300, 50, 150, 30, WS_VISIBLE | WS_CHILD | WS_BORDER);
    editLineInfo.Create(&mainWin, "", 300, 85, 150, 150, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE);
    btnGetInfo.Create(&mainWin, "Get Line Info", 300, 250, 100, 30, WS_VISIBLE | WS_CHILD);
    
    //Set the event handlers
    mainWin.AddEvent(WM_CLOSE, MainWin_OnClose, WINEVENT_MESSAGE);
    btnGetInfo.AddEvent(BN_CLICKED, GetInfoButton_OnClick, WINEVENT_COMMAND);
    
    //Set edit control's font
    editBox.SetFontName("Times New Roman");
    editBox.SetFontWeight(50);
    
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
    
    //Signal the main loop to end
    PostQuitMessage(0);
    
    return 0;
}

LRESULT GetInfoButton_OnClick(CWindow* btnPtr, const CWinEvent& eventObj)
//
{
    int lineIndex;
    int lineCount;
    int linePos;
    int lineLen;
    string lineTextStr;
    
    lineIndex = (int) StrToNum(editLineIdx.GetText());
    lineCount = editBox.GetLineCount();
    linePos = editBox.GetLinePos(lineIndex);
    lineLen = editBox.GetLineLen(lineIndex);
    lineTextStr = editBox.GetLineText(lineIndex);
    
    editLineInfo.SetText("");
    editLineInfo.AppendText(("# of lines: " + NumToStr(lineCount) + "\r\n").c_str());
    editLineInfo.AppendText(("Line position: " + NumToStr(linePos) + "\r\n").c_str());
    editLineInfo.AppendText(("Line length: " + NumToStr(lineLen) + "\r\n").c_str());
    editLineInfo.AppendText(("Line text: '" + lineTextStr + "'\r\n").c_str());
    
    return 0;
}


