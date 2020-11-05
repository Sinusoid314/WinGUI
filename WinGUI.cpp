#include <algorithm>
#include "WinGUI.h"

using namespace std;


const char* mainWinClassName = "WinGUI_MainWindow";
const char* modalWinClassName = "WinGUI_ModalWindow";
const char* drawBoxClassName = "WinGUI_DrawBox";

bool WinGUISetup(void)
//Register the window classes
{
    HINSTANCE hThisInstance = (HINSTANCE) GetModuleHandle(NULL);
    WNDCLASSEX wincl;
    INITCOMMONCONTROLSEX InitCtrlEx;

    //Initialize common controls
    InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCtrlEx.dwICC  = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&InitCtrlEx);

    //Register the main window class
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = mainWinClassName;
    wincl.lpfnWndProc = MainWinProc;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_3DFACE;
    if(!RegisterClassEx (&wincl))
    {
        return false;
    }

    //Register the modal window class
    wincl.lpszClassName = modalWinClassName;
    wincl.lpfnWndProc = ModalWinProc;
    if(!RegisterClassEx (&wincl))
    {
        return false;
    }

    //Register the DrawBox class
    wincl.lpszClassName = drawBoxClassName;
    wincl.lpfnWndProc = DrawBoxProc;
    wincl.style = CS_DBLCLKS | CS_OWNDC;
    if(!RegisterClassEx (&wincl))
    {
        return false;
    }

    return true;
}

bool WinGUICleanup(void)
//Unregister the main window class
{
    HINSTANCE hThisInstance = (HINSTANCE) GetModuleHandle(NULL);

    if(UnregisterClass(drawBoxClassName, hThisInstance) == 0)
	    return false;

    if(UnregisterClass(modalWinClassName, hThisInstance) == 0)
	    return false;

    if(UnregisterClass(mainWinClassName, hThisInstance) == 0)
	    return false;

    return true;
}

WPARAM InputWinEvents(void)
//Run the main Windows message loop
{
	MSG msgInfo;

    while(GetMessage(&msgInfo, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msgInfo);
        DispatchMessage(&msgInfo);
    }

    return msgInfo.wParam;
}

void FlushWinEvents(MSG* msgInfoPtr)
//Process all pending messages
{
    MSG msgInfo;

    //If no MSG structure is given, use our own
    if(msgInfoPtr == NULL)
        msgInfoPtr = &msgInfo;

    while(PeekMessage(msgInfoPtr, NULL, 0, 0, PM_REMOVE))
    {
        if(msgInfoPtr->message == WM_QUIT)
            break;
        TranslateMessage(msgInfoPtr);
        DispatchMessage(msgInfoPtr);
    }
}

LRESULT CALLBACK MainWinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//Process main window events
{
	CMainWindow* winObjPtr = 0;
    LRESULT retVal = 0;
    bool callDefProc = true;

    if(message == WM_NCCREATE)
    {
        winObjPtr = (CMainWindow*) SetupWinObj(hwnd, (CREATESTRUCT*) lParam);
        if(winObjPtr == NULL)
        {
            return FALSE;
        }
        //Return TRUE in case there is a custom message handler for
        //WM_NCCREATE that does not call DefWindowProc
        retVal = TRUE;
    }
    else
    {
	    //Get the CWindow object pointer linked with the window handle
        winObjPtr = (CMainWindow*) GetWindowLong(hwnd, GWL_USERDATA);
        if(winObjPtr == 0)
        {
	        return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    //Call the custom message handler, if present
    if(CallWinEventFunc(winObjPtr, message, wParam, lParam, retVal))
    {
        callDefProc = false;
    }

    if(message == WM_NCDESTROY)
    {
		CleanupWinObj(winObjPtr);
    }

    //Do default processing if message has not been handled
    if(callDefProc)
    {
        retVal = DefWindowProc(hwnd, message, wParam, lParam);
    }

    return retVal;
}

LRESULT CALLBACK ModalWinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//Process modal window events
{
	CModalWindow* winObjPtr = 0;
    LRESULT retVal = 0;
    bool callDefProc = true;
    HWND ownerHandle;

    if(message == WM_NCCREATE)
    {
        winObjPtr = (CModalWindow*) SetupWinObj(hwnd, (CREATESTRUCT*) lParam);
        if(winObjPtr == NULL)
        {
            return FALSE;
        }
        //Return TRUE in case there is a custom message handler for
        //WM_NCCREATE that does not call DefWindowProc
        retVal = TRUE;
    }
    else
    {
	    //Get the CWindow object pointer linked with the window handle
        winObjPtr = (CModalWindow*) GetWindowLong(hwnd, GWL_USERDATA);
        if(winObjPtr == 0)
        {
	        return DefWindowProc(hwnd, message, wParam, lParam);
        }

        switch(message)
        {
            case WM_CREATE:
                ownerHandle = GetWindow(hwnd, GW_OWNER);
                winObjPtr->ownerOrigState = IsWindowEnabled(ownerHandle);
                EnableWindow(ownerHandle, FALSE);
                callDefProc = false;
                break;

            case WM_DESTROY:
                ownerHandle = GetWindow(hwnd, GW_OWNER);
                EnableWindow(ownerHandle, winObjPtr->ownerOrigState);
                SetActiveWindow(ownerHandle);
                callDefProc = false;
                break;
        }
    }

    //Call the custom message handler, if present
    if(CallWinEventFunc(winObjPtr, message, wParam, lParam, retVal))
    {
        callDefProc = false;
    }

    if(message == WM_NCDESTROY)
    {
		CleanupWinObj(winObjPtr);
    }

    //Do default processing if message has not been handled
    if(callDefProc)
    {
        retVal = DefWindowProc(hwnd, message, wParam, lParam);
    }

    return retVal;
}

LRESULT CALLBACK ControlProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//Process control window events
{
	CControl* ctlObjPtr = 0;
    LRESULT retVal = 0;

    //Get the CWindow object pointer linked with the window handle
    ctlObjPtr = (CControl*) GetWindowLong(hwnd, GWL_USERDATA);
    if(ctlObjPtr == 0)
    {
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    //Call the custom message handler, if present
    CallWinEventFunc(ctlObjPtr, message, wParam, lParam, retVal);

    //Do default message processing
    retVal = CallWindowProc(ctlObjPtr->oldCtlProc, hwnd, message, wParam, lParam);

    if(message == WM_DESTROY)
    {
        ctlObjPtr->CleanupControlData();
    }
    else if(message == WM_NCDESTROY)
    {
		CleanupWinObj(ctlObjPtr);
    }

    return retVal;
}

LRESULT CALLBACK DrawBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
//Process DrawBox events
{
	CDrawBox* dboxObjPtr = 0;
    LRESULT retVal = 0;
    bool callDefProc = true;
    PAINTSTRUCT paintInfo;

    if(message == WM_NCCREATE)
    {
        dboxObjPtr = (CDrawBox*) SetupWinObj(hwnd, (CREATESTRUCT*) lParam);
        if(dboxObjPtr == NULL)
        {
            return FALSE;
        }
        //Return TRUE in case there is a custom message handler for
        //WM_NCCREATE that does not call DefWindowProc
        retVal = TRUE;
    }
    else
    {
	    //Get the CWindow object pointer linked with the window handle
        dboxObjPtr = (CDrawBox*) GetWindowLong(hwnd, GWL_USERDATA);
        if(dboxObjPtr == 0)
        {
	        return DefWindowProc(hwnd, message, wParam, lParam);
        }

        switch(message)
        {
            case WM_CREATE:
                //Create and initialize the redraw data
                dboxObjPtr->SetupDrawData();
                callDefProc = false;
                break;

            case WM_PAINT:
                //Repaint the given area of DrawBox's view
                if(dboxObjPtr->autoRedraw)
                {
                    if(BeginPaint(hwnd, &paintInfo))
                    {

                        dboxObjPtr->RedrawArea(paintInfo.rcPaint.left, paintInfo.rcPaint.top,
                                               paintInfo.rcPaint.right - paintInfo.rcPaint.left,
                                               paintInfo.rcPaint.bottom - paintInfo.rcPaint.top);
                        EndPaint(hwnd, &paintInfo);
                    }
                    callDefProc = false;
                }
                break;
        }
    }

    //Call the custom message handler, if present
    if(CallWinEventFunc(dboxObjPtr, message, wParam, lParam, retVal))
    {
        callDefProc = false;
    }

    if(message == WM_DESTROY)
    {
        dboxObjPtr->CleanupDrawData();
        callDefProc = false;
    }
    else if(message == WM_NCDESTROY)
    {
        CleanupWinObj(dboxObjPtr);
    }

    //Do default processing if message has not been handled
    if(callDefProc)
    {
        retVal = DefWindowProc(hwnd, message, wParam, lParam);
    }

    return retVal;
}

CWindow* SetupWinObj(HWND hwnd, CREATESTRUCT* setupInfoPtr)
//Link the object pointer with the window handle
{
    CWindow* winObjPtr;

    //Get the CWindow object pointer from lParam
    winObjPtr = (CWindow*) (setupInfoPtr->lpCreateParams);
    if(winObjPtr == 0)
    {
        return NULL;
    }

    //Store the window handle in the object
    winObjPtr->winHandle = hwnd;

    //Store the object pointer in the window's user data
    SetLastError(0);
    if((SetWindowLong(hwnd, GWL_USERDATA, (LONG) winObjPtr) == 0) && (GetLastError() != 0))
    {
        return NULL;
    }

    return winObjPtr;
}

void CleanupWinObj(CWindow* winObjPtr)
//Unlink the object pointer from the window handle
{
    //Clear the window's user data
    SetWindowLong(winObjPtr->winHandle, GWL_USERDATA, 0);

    if(winObjPtr->deleteOnWinDestroy)
    {
        if(!(winObjPtr->isBeingDeleted))
        {
            winObjPtr->isBeingDeleted = true;
            delete winObjPtr;
        }
    }
    else
    {
        winObjPtr->winHandle = NULL;
    }
}

bool CallWinEventFunc(CWindow* winObjPtr, UINT message, WPARAM wParam, LPARAM lParam, LRESULT& retVal)
//
{
    CWindow* childObjPtr = 0;
    int winEventIdx = -1;
    int childEventIdx = -1;
    bool eventFound = false;

    //If the message is a notification from a child window
    if((message == WM_COMMAND) && (lParam != 0))
    {
        childObjPtr = (CWindow*) GetWindowLong((HWND) lParam, GWL_USERDATA);
        if(childObjPtr != 0)
        {
	        //Get the index of the event object tied to the notification ID
            childEventIdx = childObjPtr->GetEventIndex(HIWORD(wParam), WINEVENT_COMMAND);
            if(childEventIdx > -1)
            {
                //Call the matching event function
                childObjPtr->winEventList[childEventIdx].wParam = wParam;
                childObjPtr->winEventList[childEventIdx].lParam = lParam;
                retVal = (*((childObjPtr->winEventList[childEventIdx]).eventFuncPtr))(childObjPtr, childObjPtr->winEventList[childEventIdx]);
                eventFound = true;
            }
        }
    }
    else if(message == WM_NOTIFY)
    {
        childObjPtr = (CWindow*) GetWindowLong(((LPNMHDR)lParam)->hwndFrom, GWL_USERDATA);
        if(childObjPtr != 0)
        {
	        //Get the index of the event object tied to the notification ID
            childEventIdx = childObjPtr->GetEventIndex(((LPNMHDR)lParam)->code, WINEVENT_NOTIFY);
            if(childEventIdx > -1)
            {
                //Call the matching event function
                childObjPtr->winEventList[childEventIdx].wParam = wParam;
                childObjPtr->winEventList[childEventIdx].lParam = lParam;
                retVal = (*((childObjPtr->winEventList[childEventIdx]).eventFuncPtr))(childObjPtr, childObjPtr->winEventList[childEventIdx]);
                eventFound = true;
            }
        }
    }

    //Get the index of the event object tied to message
    winEventIdx = winObjPtr->GetEventIndex(message, WINEVENT_MESSAGE);

    if(winEventIdx > -1)
    {
	    //Call the matching event function
        winObjPtr->winEventList[winEventIdx].wParam = wParam;
        winObjPtr->winEventList[winEventIdx].lParam = lParam;
        retVal = (*((winObjPtr->winEventList[winEventIdx]).eventFuncPtr))(winObjPtr, winObjPtr->winEventList[winEventIdx]);
        eventFound = true;
    }

    if(!eventFound)
    {
	    if(winObjPtr->defEventFuncPtr != NULL)
        {
            //Call the default event function
            (*(winObjPtr->defEventFuncPtr))(winObjPtr, message, wParam, lParam);
        }
    }

    return eventFound;
}

CWinEvent::CWinEvent(void)
//Setup event object
{
    eventID = 0;
    eventType = WINEVENT_MESSAGE;
    eventFuncPtr = NULL;
    userData = 0;
    wParam = 0;
    lParam = 0;
}

CWinEvent::CWinEvent(UINT newEventID, LRESULT (*newEventFuncPtr)(CWindow*, const CWinEvent&), WinEventType newEventType, LONG newUserData)
//Setup event object with the given arguments
{
    eventID = newEventID;
    eventType = newEventType;
    eventFuncPtr = newEventFuncPtr;
    userData = newUserData;
    wParam = 0;
    lParam = 0;
}

CWindow::CWindow(void)
//Setup generic window object
{
    winName = "";
    winHandle = NULL;
    winClassName = "";
    winDefStyle = 0;
    winDefExStyle = 0;
    deleteOnWinDestroy = false;
    isBeingDeleted = false;
    userData = 0;
    defEventFuncPtr = NULL;
}

CWindow::~CWindow(void)
//Cleanup generic window object
{
    if(!isBeingDeleted)
    {
        isBeingDeleted = true;

	    if(winHandle != NULL)
        {
            //Close the window handle
            DestroyWindow(winHandle);
        }
    }

    //Clear event list
    winEventList.clear();
}

void CWindow::AddEvent(UINT newEventID, LRESULT (*newEventFuncPtr)(CWindow*, const CWinEvent&), WinEventType newEventType, LONG newUserData)
//Add a new window event with the given message/function-pointer pair
{
    int eventIdx;

    //Get the index of the event object (if present)
    eventIdx = GetEventIndex(newEventID, newEventType);

    if(eventIdx == -1)
    {
        //Add new event object to winEventList
        winEventList.push_back(CWinEvent(newEventID, newEventFuncPtr, newEventType, newUserData));
    }
    else
    {
        //Update the existing event with the new function pointer and user data
        winEventList[eventIdx].eventFuncPtr = newEventFuncPtr;
        winEventList[eventIdx].userData = newUserData;
    }
}

bool CWindow::RemoveEvent(UINT eventID, WinEventType eventType)
//Remove the event object identified by the given window message
{
    int eventIdx;

    //Get the index of the event object (if present)
    eventIdx = GetEventIndex(eventID, eventType);

    if(eventIdx == -1)
    {
	    //Event not found
	    return false;
    }
    else
    {
	    //Remove event
        winEventList.erase(winEventList.begin() + eventIdx);
        return true;
    }
}

int CWindow::GetEventIndex(UINT eventID, WinEventType eventType)
//Return the index of the event that matches the given window message
{
    for(unsigned int n=0; n < winEventList.size(); n++)
    {
	    if((winEventList[n].eventID == eventID) && (winEventList[n].eventType == eventType))
        {
	        return n;
        }
    }

    return -1;
}

void CWindow::Destroy(void)
//Destroy the window handle
{
    DestroyWindow(winHandle);
}

void CWindow::Show(void)
//Make the window visible
{
    ShowWindow(winHandle, SW_SHOW);
}

void CWindow::Hide(void)
//Make the window invisible
{
    ShowWindow(winHandle, SW_HIDE);
}

void CWindow::Enable(void)
//Enable input on the window
{
    EnableWindow(winHandle, TRUE);
}

void CWindow::Disable(void)
//Disable input on the window
{
    EnableWindow(winHandle, FALSE);
}

string CWindow::GetText(void)
//Return the window's text as a string object
{
    char* textStr;
    int textStrLen;
    string retStr;

    textStrLen = GetWindowTextLength(winHandle) + 1;
    textStr = new char[textStrLen];
    GetWindowText(winHandle, textStr, textStrLen);

    retStr = textStr;
    delete [] textStr;

    return retStr;
}

void CWindow::SetText(const char* newTextStr)
//Set the window's text to newTextStr
{
    SetWindowText(winHandle, newTextStr);
}

void CWindow::GetPos(int* winLeft, int* winTop)
//
{
    RECT winRect;

    GetWindowRect(winHandle, &winRect);
    MapWindowPoints(HWND_DESKTOP, GetParent(winHandle), (LPPOINT)&winRect, 2);

    *winLeft = winRect.left;
    *winTop = winRect.top;
}

void CWindow::GetSize(int* winWidth, int* winHeight)
//
{
    RECT winRect;

    GetWindowRect(winHandle, &winRect);

    *winWidth = winRect.right - winRect.left;
    *winHeight = winRect.bottom - winRect.top;
}

void CWindow::GetClientSize(int* winWidth, int* winHeight)
//
{
    RECT winRect;

    GetClientRect(winHandle, &winRect);
    *winWidth = winRect.right;
    *winHeight = winRect.bottom;
}

void CWindow::SetPos(int winLeft, int winTop)
//
{
    SetWindowPos(winHandle, 0, winLeft, winTop, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void CWindow::SetSize(int winWidth, int winHeight)
//
{
    SetWindowPos(winHandle, 0, 0, 0, winWidth, winHeight, SWP_NOMOVE | SWP_NOZORDER);
}

void CWindow::SetClientSize(int clientWidth, int clientHeight)
//
{
    RECT winRect = {0, 0, clientWidth, clientHeight};

    AdjustWindowRectEx(&winRect, GetStyle(), FALSE, GetStyleEx());
    SetSize(winRect.right - winRect.left, winRect.bottom - winRect.top);
}

LONG CWindow::GetStyle(void)
//
{
    return GetWindowLongA(winHandle, GWL_STYLE);
}

LONG CWindow::GetStyleEx(void)
//
{
    return GetWindowLongA(winHandle, GWL_EXSTYLE);
}

LONG CWindow::SetStyle(LONG newStyle)
//
{
    return SetWindowLong(winHandle, GWL_STYLE, newStyle);
}

LONG CWindow::SetStyleEx(LONG newStyle)
//
{
    return SetWindowLong(winHandle, GWL_EXSTYLE, newStyle);
}

CWindow* CWindow::GetParentWin(void)
//
{
    CWindow* winParentPtr;
    HWND parentHandle;

    parentHandle = GetParent(winHandle);

    if(parentHandle == HWND_DESKTOP)
    {
        winParentPtr = NULL;
    }
    else
    {
        winParentPtr = (CWindow*) GetWindowLong(parentHandle, GWL_USERDATA);
    }

    return winParentPtr;
}

void CWindow::SetParentWin(CWindow* winParentPtr)
//
{
    if(winParentPtr == NULL)
    {
        SetParent(winHandle, NULL);
    }
    else
    {
        SetParent(winHandle, winParentPtr->winHandle);
    }
}

void CWindow::GetFontInfo(LOGFONT* currFontInfo)
//Default NOP member for derived classes that don't use GetFontInfo()
{
    return;
}

void CWindow::SetFontInfo(LOGFONT* newFontInfo)
//Default NOP member for derived classes that don't use SetFontInfo()
{
    return;
}

string CWindow::GetFontName(void)
//Return the current font name
{
    LOGFONT fontInfo;

    GetFontInfo(&fontInfo);
    return string(fontInfo.lfFaceName);
}

void CWindow::SetFontName(const char* newNameStr)
//Set the current font name
{
    LOGFONT fontInfo;
    string faceNameStr;
    size_t faceNameLen;

    faceNameStr = newNameStr;

    GetFontInfo(&fontInfo);
    faceNameLen = faceNameStr.copy(fontInfo.lfFaceName, 31);
    fontInfo.lfFaceName[faceNameLen] = '\0';
    SetFontInfo(&fontInfo);
}

void CWindow::GetFontSize(int* currWidth, int* currHeight)
//Return the current font size
{
    LOGFONT fontInfo;

    GetFontInfo(&fontInfo);
    *currWidth = fontInfo.lfWidth;
    *currHeight = fontInfo.lfHeight;
}

void CWindow::SetFontSize(int newWidth, int newHeight)
//Set the current font size
{
    LOGFONT fontInfo;

    GetFontInfo(&fontInfo);
    fontInfo.lfWidth = newWidth;
    fontInfo.lfHeight = newHeight;
    SetFontInfo(&fontInfo);
}

LONG CWindow::GetFontWeight(void)
//Return the thickness of the current font
{
    LOGFONT fontInfo;

    GetFontInfo(&fontInfo);
    return fontInfo.lfWeight;
}

void CWindow::SetFontWeight(LONG newWeight)
//Set the thickness of the current font
{
    LOGFONT fontInfo;

    GetFontInfo(&fontInfo);
    fontInfo.lfWeight = newWeight;
    SetFontInfo(&fontInfo);
}

void CWindow::GetFontStyle(BYTE* isItalic, BYTE* isUnderline, BYTE* isStrikeOut)
//Return the style settings for the current font
{
    LOGFONT fontInfo;

    GetFontInfo(&fontInfo);
    *isItalic = fontInfo.lfItalic;
    *isUnderline = fontInfo.lfUnderline;
    *isStrikeOut = fontInfo.lfStrikeOut;
}

void CWindow::SetFontStyle(BYTE isItalic, BYTE isUnderline, BYTE isStrikeOut)
//Set the style settings for the current font
{
    LOGFONT fontInfo;

    GetFontInfo(&fontInfo);
    fontInfo.lfItalic = isItalic;
    fontInfo.lfUnderline = isUnderline;
    fontInfo.lfStrikeOut = isStrikeOut;
    SetFontInfo(&fontInfo);
}

CMainWindow::CMainWindow(void)
//Setup main window object
{
    Init();
}

CMainWindow::CMainWindow(const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup main window object and create its window
{
    Init();
    Create(winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CMainWindow::Init(void)
//Initialize members
{
    winClassName = mainWinClassName;
}

bool CMainWindow::Create(const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Create a main window
{
	if(winHandle != NULL)
        return false;

    if(CreateWindowEx(winExStyle, winClassName.c_str(), winText, winStyle, winX, winY, winWidth, winHeight,
                      HWND_DESKTOP, NULL, (HINSTANCE) GetModuleHandle(NULL), (LPVOID) this) == NULL)
    {
        return false;
    }

    return true;
}

CModalWindow::CModalWindow(void)
//Setup modal window object
{
    Init();
}

CModalWindow::CModalWindow(CWindow* winOwnerPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup main window object and create its window
{
    Init();
    Create(winOwnerPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CModalWindow::Init(void)
//Initialize members
{
    winClassName = modalWinClassName;
    ownerOrigState = TRUE;
}

bool CModalWindow::Create(CWindow* winOwnerPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Create a main window
{
	if(winHandle != NULL)
        return false;

    if(CreateWindowEx(winExStyle, winClassName.c_str(), winText, winStyle, winX, winY, winWidth, winHeight,
                      winOwnerPtr->winHandle, NULL, (HINSTANCE) GetModuleHandle(NULL), (LPVOID) this) == NULL)
    {
        return false;
    }

    return true;
}

CDrawBox::CDrawBox(void)
//
{
    Init();
}

CDrawBox::CDrawBox(CWindow* winParentPtr, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//
{
    Init();
    Create(winParentPtr, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CDrawBox::Init(void)
//
{
	winClassName = drawBoxClassName;
    redrawBMP = NULL;
    redrawDefBMP = NULL;
    redrawDefPen = NULL;
    redrawDefBrush = NULL;
    redrawDefFont = NULL;
    redrawWidth = 0;
    redrawHeight = 0;
    redrawDC = NULL;
    viewDefPen = NULL;
    viewDefBrush = NULL;
    viewDefFont = NULL;
    viewDC = NULL;
    drawPen = NULL;
    drawBrush = NULL;
    drawFont = NULL;
    activeDC = NULL;
    autoRedraw = false;
}

bool CDrawBox::Create(CWindow* winParentPtr, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//
{
    HWND parentHandle = HWND_DESKTOP;

    if(winHandle != NULL)
        return false;

    if(winParentPtr != NULL)
        parentHandle = winParentPtr->winHandle;

    if(CreateWindowEx(winExStyle, winClassName.c_str(), "", winStyle, winX, winY, winWidth, winHeight,
                      parentHandle, NULL, (HINSTANCE) GetModuleHandle(NULL), (LPVOID) this) == NULL)
    {
        return false;
    }

    return true;
}

void CDrawBox::SetupDrawData(void)
//Create drawing resources
{
    LOGPEN penInfo;
    LOGBRUSH brushInfo;
    LOGFONT fontInfo;

    if(redrawBMP != NULL)
        return;

    //Store the DrawBox's display DC handle
    viewDC = GetDC(winHandle);

    //Set redraw size to the current screen size
    redrawWidth = GetSystemMetrics(SM_CXSCREEN);
    redrawHeight = GetSystemMetrics(SM_CYSCREEN);

    //Create redraw data
    redrawBMP = CreateCompatibleBitmap(viewDC, redrawWidth, redrawHeight);
    redrawDC = CreateCompatibleDC(viewDC);
    redrawDefBMP = (HBITMAP) SelectObject(redrawDC, redrawBMP);

    //Create the draw pen
    penInfo.lopnStyle = PS_SOLID;
    penInfo.lopnWidth.x = 1;
    penInfo.lopnColor = RGB(0,0,0);
    drawPen = CreatePenIndirect(&penInfo);

    //Create the draw brush
    brushInfo.lbStyle = BS_HOLLOW;
    brushInfo.lbColor = 0;
    brushInfo.lbHatch = 0;
    drawBrush = CreateBrushIndirect(&brushInfo);

    //Create the draw font
    GetObject(GetCurrentObject(viewDC, OBJ_FONT), sizeof(fontInfo), &fontInfo);
    drawFont = CreateFontIndirect(&fontInfo);

    //Select the draw pen into the display and redraw DCs
    viewDefPen = (HPEN) SelectObject(viewDC, drawPen);
    redrawDefPen = (HPEN) SelectObject(redrawDC, drawPen);

    //Select the draw brush into the display and redraw DCs
    viewDefBrush = (HBRUSH) SelectObject(viewDC, drawBrush);
    redrawDefBrush = (HBRUSH) SelectObject(redrawDC, drawBrush);

    //Select the draw font into the display and redraw DCs
    viewDefFont = (HFONT) SelectObject(viewDC, drawFont);
    redrawDefFont = (HFONT) SelectObject(redrawDC, drawFont);

    //Draw everything to the display DC
    UseViewDC();
}

void CDrawBox::CleanupDrawData(void)
//Delete drawing resources
{
    if(redrawBMP == NULL)
        return;

    //Select default pens back into display and redraw DCs
    SelectObject(viewDC, viewDefPen);
    SelectObject(redrawDC, redrawDefPen);

    //Select default brushes back into display and redraw DCs
    SelectObject(viewDC, viewDefBrush);
    SelectObject(redrawDC, redrawDefBrush);

    //Select default fonts back into display and redraw DCs
    SelectObject(viewDC, viewDefFont);
    SelectObject(redrawDC, redrawDefFont);

    //Delete drawing objects
    DeleteObject(drawPen);
    DeleteObject(drawBrush);
    DeleteObject(drawFont);

    SelectObject(redrawDC, redrawDefBMP);
    DeleteDC(redrawDC);
    DeleteObject(redrawBMP);
}

void CDrawBox::ResizeRedrawDC(int winWidth, int winHeight)
//Change the size of the redraw bitmap to fit the new window size
{
    HDC newDC;
    HBITMAP newBMP;
    HBITMAP newDefBMP;
    HPEN newDefPen;
    HBRUSH newDefBrush;
    HFONT newDefFont;
    int newWidth;
    int newHeight;

    if(redrawBMP == NULL)
        return;

///*
    //No update needed if new size isn't larger than redraw size
    if((winWidth <= redrawWidth) && (winHeight <= redrawHeight))
        return;

    //Set new redraw size to the DrawBox's client size
    if(winWidth > redrawWidth)
        newWidth = winWidth; //+ int(GetSystemMetrics(SM_CXSCREEN) * 0.1);
    else
        newWidth = redrawWidth;
    if(winHeight > redrawHeight)
        newHeight = winHeight; //+ int(GetSystemMetrics(SM_CYSCREEN) * 0.1);
    else
        newHeight = redrawHeight;
//*/

    //Create new redraw DC and BMP
    newDC = CreateCompatibleDC(viewDC);
    newBMP = CreateCompatibleBitmap(viewDC, newWidth, newHeight);
    newDefBMP = (HBITMAP) SelectObject(newDC, newBMP);

    //Select the current pen, brush, and font into the new redraw DC
    newDefPen = (HPEN) SelectObject(newDC, drawPen);
    newDefBrush = (HBRUSH) SelectObject(newDC, drawBrush);
    newDefFont = (HFONT) SelectObject(newDC, drawFont);

    //Transfer redraw image from old DC to new DC
    BitBlt(newDC, 0, 0, redrawWidth, redrawHeight, redrawDC, 0, 0, SRCCOPY);

    //Select the default pen, brush, and font back into the old redraw DC
    SelectObject(redrawDC, redrawDefPen);
    SelectObject(redrawDC, redrawDefBrush);
    SelectObject(redrawDC, redrawDefFont);

    //Delete old redraw data
    SelectObject(redrawDC, redrawDefBMP);
    DeleteDC(redrawDC);
    DeleteObject(redrawBMP);

    //Update all redraw data
    redrawDC = newDC;
    redrawBMP = newBMP;
    redrawDefBMP = newDefBMP;
    redrawDefPen = newDefPen;
    redrawDefBrush = newDefBrush;
    redrawDefFont = newDefFont;
    redrawWidth = newWidth;
    redrawHeight = newHeight;
}

void CDrawBox::UseRedrawDC(void)
//
{
    activeDC = redrawDC;
}

void CDrawBox::UseViewDC(void)
//
{
    activeDC = viewDC;
}

void CDrawBox::Redraw(void)
//Copy redrawDC to viewDC
{
    int width;
    int height;

    GetClientSize(&width, &height);
    BitBlt(viewDC, 0, 0, width, height, redrawDC, 0, 0, SRCCOPY);
}

void CDrawBox::RedrawArea(int left, int top, int width, int height)
//Copy the given area from redrawDC to viewDC
{
    BitBlt(viewDC, left, top, width, height, redrawDC, left, top, SRCCOPY);
}

void CDrawBox::Stick(void)
//Copy viewDC to redrawDC
{
	int width;
    int height;

    GetClientSize(&width, &height);
    BitBlt(redrawDC, 0, 0, width, height, viewDC, 0, 0, SRCCOPY);
}

void CDrawBox::StickArea(int left, int top, int width, int height)
//Copy the given area of viewDC to redrawDC
{
    BitBlt(redrawDC, left, top, width, height, viewDC, left, top, SRCCOPY);
}

void CDrawBox::CopyAreaTo(CDrawBox* destBoxPtr, int destLeft, int destTop, int srcLeft, int srcTop, int srcWidth, int srcHeight)
//Copy a portion of this DrawBox to another DrawBox
{
    BitBlt(destBoxPtr->activeDC, destLeft, destTop, srcWidth, srcHeight, activeDC, srcLeft, srcTop, SRCCOPY);
}

int CDrawBox::GetBackMode(void)
//Return the current background mode
{
    return GetBkMode(activeDC);
}

void CDrawBox::SetBackMode(int newMode)
//Set the background mode of the DrawBox
{
    SetBkMode(viewDC, newMode);
    SetBkMode(redrawDC, newMode);
}

int CDrawBox::GetROP(void)
//Return the current mix mode
{
    return GetROP2(activeDC);
}

void CDrawBox::SetROP(int newROP)
//Set the mix mode
{
    SetROP2(viewDC, newROP);
    SetROP2(redrawDC, newROP);
}

COLORREF CDrawBox::GetBackColor(void)
//Return the current background color
{
    return GetBkColor(activeDC);
}

void CDrawBox::SetBackColor(COLORREF newColor)
//Set the background color of the DrawBox
{
    SetBkColor(viewDC, newColor);
    SetBkColor(redrawDC, newColor);
}

COLORREF CDrawBox::GetForeColor(void)
//Return the current text color
{
    return GetTextColor(activeDC);
}

void CDrawBox::SetForeColor(COLORREF newColor)
//Set the text color of the DrawBox
{
    SetTextColor(viewDC, newColor);
    SetTextColor(redrawDC, newColor);
}

void CDrawBox::GetFontInfo(LOGFONT* currFontInfo)
//Return info about the current font
{
    GetObject(drawFont, sizeof(LOGFONT), currFontInfo);
}

void CDrawBox::SetFontInfo(LOGFONT* newFontInfo)
//Set the info describing the current font
{
    HFONT newFont;

    newFont = CreateFontIndirect(newFontInfo);
    SelectObject(viewDC, newFont);
    SelectObject(redrawDC, newFont);
    DeleteObject(drawFont);
    drawFont = newFont;
}

void CDrawBox::GetPenInfo(LOGPEN* currPenInfo)
//Return info about the current drawing pen
{
    GetObject(drawPen, sizeof(LOGPEN), currPenInfo);
}

void CDrawBox::SetPenInfo(LOGPEN* newPenInfo)
//Set the info describing the current pen
{
    HPEN newPen;

    newPen = CreatePenIndirect(newPenInfo);
    SelectObject(viewDC, newPen);
    SelectObject(redrawDC, newPen);
    DeleteObject(drawPen);
    drawPen = newPen;
}

UINT CDrawBox::GetPenStyle(void)
//Return the current pen's size
{
    LOGPEN penInfo;

    GetPenInfo(&penInfo);
    return penInfo.lopnStyle;
}

void CDrawBox::SetPenStyle(UINT newStyle)
//Set the current pen's size
{
    LOGPEN penInfo;

    GetPenInfo(&penInfo);
    penInfo.lopnStyle = newStyle;
    SetPenInfo(&penInfo);
}

LONG CDrawBox::GetPenSize(void)
//Return the current pen's size
{
    LOGPEN penInfo;

    GetPenInfo(&penInfo);
    return penInfo.lopnWidth.x;
}

void CDrawBox::SetPenSize(LONG newSize)
//Set the current pen's size
{
    LOGPEN penInfo;

    GetPenInfo(&penInfo);
    penInfo.lopnWidth.x = newSize;
    SetPenInfo(&penInfo);
}

COLORREF CDrawBox::GetPenColor(void)
//Return the current pen's color
{
    LOGPEN penInfo;

    GetPenInfo(&penInfo);
    return penInfo.lopnColor;
}

void CDrawBox::SetPenColor(COLORREF newColor)
//Set the current pen's color
{
    LOGPEN penInfo;

    GetPenInfo(&penInfo);
    penInfo.lopnColor = newColor;
    SetPenInfo(&penInfo);
}

void CDrawBox::GetBrushInfo(LOGBRUSH* currBrushInfo)
//Return info about the current drawing brush
{
    GetObject(drawBrush, sizeof(LOGBRUSH), currBrushInfo);
}

void CDrawBox::SetBrushInfo(LOGBRUSH* newBrushInfo)
//Set the info describing the current brush
{
    HBRUSH newBrush;

    newBrush = CreateBrushIndirect(newBrushInfo);
    SelectObject(viewDC, newBrush);
    SelectObject(redrawDC, newBrush);
    DeleteObject(drawBrush);
    drawBrush = newBrush;
}

UINT CDrawBox::GetBrushStyle(void)
//Return the current brush's style
{
    LOGBRUSH brushInfo;

    GetBrushInfo(&brushInfo);
    return brushInfo.lbStyle;
}

void CDrawBox::SetBrushStyle(UINT newStyle)
//Set the current brush's style
{
    LOGBRUSH brushInfo;

    GetBrushInfo(&brushInfo);
    brushInfo.lbStyle = newStyle;
    SetBrushInfo(&brushInfo);
}

COLORREF CDrawBox::GetBrushColor(void)
//Return the current brush's color
{
    LOGBRUSH brushInfo;

    GetBrushInfo(&brushInfo);
    return brushInfo.lbColor;
}

void CDrawBox::SetBrushColor(COLORREF newColor)
//Set the current brush's color
{
    LOGBRUSH brushInfo;

    GetBrushInfo(&brushInfo);
    brushInfo.lbColor = newColor;
    SetBrushInfo(&brushInfo);
}

COLORREF CDrawBox::GetPoint(int left, int top)
//Return the color of the given pixel
{
    return GetPixel(activeDC, left, top);
}

void CDrawBox::SetPoint(int left, int top, COLORREF color)
//Set the given pixel to the given color
{
    SetPixel(activeDC, left, top, color);
}

void CDrawBox::DrawText(const char* textStr, int left, int top)
//Draw the given string on the DrawBox
{
    TextOut(activeDC, left, top, textStr, char_traits<char>::length(textStr));
}

void CDrawBox::DrawLine(int startLeft, int startTop, int endLeft, int endTop)
//Draw the given line on the DrawBox
{
    MoveToEx(activeDC, startLeft, startTop, NULL);
    LineTo(activeDC, endLeft, endTop);
}

void CDrawBox::DrawRect(int left, int top, int width, int height)
//Draw the given rectangle on the DrawBox
{
    Rectangle(activeDC, left, top, (left + width), (top + height));
}

void CDrawBox::DrawCircle(int centerLeft, int centerTop, int radius)
//Draw the given circle on the DrawBox
{
    Ellipse(activeDC, (centerLeft - radius), (centerTop - radius),
            (centerLeft + radius), (centerTop + radius));
}

void CDrawBox::DrawImage(CImage* imgPtr, int left, int top, DWORD rOp)
//Draw the given image on the DrawBox
{
    HDC tmpDC;
    HBITMAP tmpDefBmp;

    tmpDC = CreateCompatibleDC(NULL);
    tmpDefBmp = (HBITMAP) SelectObject(tmpDC, imgPtr->imgHandle);
    BitBlt(activeDC, left, top, imgPtr->imgWidth, imgPtr->imgHeight, tmpDC, 0, 0, rOp);
    SelectObject(tmpDC, tmpDefBmp);
    DeleteDC(tmpDC);
}

void CDrawBox::DrawImageScaled(CImage* imgPtr, int left, int top, int scaleWidth, int scaleHeight, DWORD rOp)
//Draw the given image on the DrawBox at the given scale size
{
    HDC tmpDC;
    HBITMAP tmpDefBmp;
    int scaleLeft;
    int scaleTop;

    scaleLeft = left - ((scaleWidth < 0) ? scaleWidth : 0);
    scaleTop = top - ((scaleHeight < 0) ? scaleWidth : 0);

    tmpDC = CreateCompatibleDC(NULL);
    tmpDefBmp = (HBITMAP) SelectObject(tmpDC, imgPtr->imgHandle);
    StretchBlt(activeDC, scaleLeft, scaleTop, scaleWidth, scaleHeight, tmpDC, 0, 0, imgPtr->imgWidth, imgPtr->imgHeight, rOp);
    SelectObject(tmpDC, tmpDefBmp);
    DeleteDC(tmpDC);
}

CControl::CControl(void)
//
{
    Init();
}

void CControl::Init(void)
//
{
    oldCtlProc = NULL;
    ctlDefFont = NULL;
    ctlFont = NULL;
}

bool CControl::Create(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Create and subclass a control
{
    HWND parentHandle = HWND_DESKTOP;

    if(winHandle != NULL)
        return false;

    if(winParentPtr != NULL)
        parentHandle = winParentPtr->winHandle;

    //Include default window style (if any)
    winStyle = winStyle | winDefStyle;
    winExStyle = winExStyle | winDefExStyle;

    //Create button
    winHandle = CreateWindowEx(winExStyle, winClassName.c_str(), winText, winStyle, winX, winY, winWidth, winHeight,
                               parentHandle, NULL, (HINSTANCE) GetModuleHandle(NULL), NULL);
    if(winHandle == NULL)
        return false;

    //Link the object pointer with the window handle
    SetLastError(0);
    if((SetWindowLong(winHandle, GWL_USERDATA, (LONG) this) == 0) && (GetLastError() != 0))
    {
        return false;
    }

    //Subclass button control
    oldCtlProc = (WNDPROC) SetWindowLong(winHandle, GWL_WNDPROC, (LONG) ControlProc);

    SetupControlData();

    return true;
}

void CControl::SetupControlData(void)
//
{
    LOGFONT fontInfo;

    GetObject(GetStockObject(SYSTEM_FONT), sizeof(fontInfo), &fontInfo);
    ctlFont = CreateFontIndirect(&fontInfo);
    ctlDefFont = (HFONT) SendMessage(winHandle, WM_GETFONT, 0, 0);
    SendMessage(winHandle, WM_SETFONT, (WPARAM) ctlFont, MAKELPARAM(TRUE, 0));
}

void CControl::CleanupControlData(void)
//
{
    SendMessage(winHandle, WM_SETFONT, (WPARAM) ctlDefFont, MAKELPARAM(FALSE, 0));
    DeleteObject(ctlFont);
}

void CControl::GetFontInfo(LOGFONT* currFontInfo)
//Return info about the current font
{
    GetObject(ctlFont, sizeof(LOGFONT), currFontInfo);
}

void CControl::SetFontInfo(LOGFONT* newFontInfo)
//Set the info describing the current font
{
    HFONT newFont;

    newFont = CreateFontIndirect(newFontInfo);
    SendMessage(winHandle, WM_SETFONT, (WPARAM) newFont, MAKELPARAM(TRUE, 0));
    DeleteObject(ctlFont);
    ctlFont = newFont;
}

CButton::CButton(void)
//Setup button control object
{
    Init();
}

CButton::CButton(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup button control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CButton::Init(void)
//
{
    winClassName = "BUTTON";
}

void CButton::SetImage(CImage* imgPtr)
//Set the button's image
{
    SendMessage(winHandle, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(imgPtr->imgHandle));
}

CRadioButton::CRadioButton(void)
//Setup radio button control object
{
    Init();
}

CRadioButton::CRadioButton(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup radio button control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CRadioButton::Init(void)
//
{
    winClassName = "BUTTON";
    winDefStyle = BS_AUTORADIOBUTTON;
}

LRESULT CRadioButton::GetState(void)
//Return the radio button's check state
{
    return SendMessage(winHandle, BM_GETCHECK, 0, 0);
}

void CRadioButton::SetState(WPARAM newState)
//Set the radio button's check state
{
    SendMessage(winHandle, BM_SETCHECK, newState, 0);
}


CCheckBox::CCheckBox(void)
//Setup check box control object
{
    Init();
}

CCheckBox::CCheckBox(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup check box control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CCheckBox::Init(void)
//
{
    winClassName = "BUTTON";
    winDefStyle = BS_AUTOCHECKBOX;
}

LRESULT CCheckBox::GetState(void)
//Return the radio button's check state
{
    return SendMessage(winHandle, BM_GETCHECK, 0, 0);
}

void CCheckBox::SetState(WPARAM newState)
//Set the check box's check state
{
    SendMessage(winHandle, BM_SETCHECK, newState, 0);
}

CGroupBox::CGroupBox(void)
//Setup group box control object
{
    Init();
}

CGroupBox::CGroupBox(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup group box control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CGroupBox::Init(void)
//
{
    winClassName = "BUTTON";
    winDefStyle = BS_GROUPBOX;
}

CLabel::CLabel(void)
//Setup label control object
{
    Init();
}

CLabel::CLabel(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup label control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CLabel::Init(void)
//
{
    winClassName = "STATIC";
}

CEditBox::CEditBox(void)
//Setup edit control object
{
    Init();
}

CEditBox::CEditBox(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup edit control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CEditBox::Init(void)
//
{
    winClassName = "EDIT";
    winDefExStyle = WS_EX_CLIENTEDGE;
}

void CEditBox::AppendText(const char* appTextStr)
//Add string to the end of the edit control
{
    int textStrLen;

    textStrLen = GetWindowTextLength( winHandle );
    SendMessage( winHandle, EM_SETSEL, (WPARAM)textStrLen, (LPARAM)textStrLen );
    SendMessage( winHandle, EM_REPLACESEL, FALSE, (LPARAM) ((LPSTR) appTextStr) );
}

string CEditBox::GetSelText(void)
//
{
    string textStr;
    int selStart;
    int selLen;

    textStr = GetText();
    GetSelRange(&selStart, &selLen);
    return textStr.substr(selStart, selLen);
}

void CEditBox::SetSelText(const char* selTextStr)
//
{
    SendMessage( winHandle, EM_REPLACESEL, FALSE, (LPARAM) ((LPSTR) selTextStr) );
}

void CEditBox::GetSelRange(int* selStart, int* selLen)
//Return the current selection range
{
    int selEnd;

    SendMessage(winHandle, EM_GETSEL, (WPARAM) selStart, (LPARAM) &selEnd);
    *selLen = selEnd - (*selStart);
}

void CEditBox::SetSelRange(int selStart, int selLen)
//Select the given range of text
{
    int selEnd;

    selEnd = selStart + selLen;
    SendMessage(winHandle, EM_SETSEL, (WPARAM) selStart, (LPARAM) selEnd);
}

string CEditBox::GetLineText(int lineIndex)
//
{
    char* tmpStr;
    string lineTextStr;
    int lineLen;

    lineLen = GetLineLen(lineIndex);
    tmpStr = new char[lineLen + 1];
    *((WORD*)tmpStr) = lineLen;

    SendMessage(winHandle, EM_GETLINE, (WPARAM) lineIndex, (LPARAM)tmpStr);
    tmpStr[lineLen] = '\0';
    lineTextStr = tmpStr;

    delete [] tmpStr;
    return lineTextStr;
}

int CEditBox::GetLineLen(int lineIndex)
//
{
    return SendMessage(winHandle, EM_LINELENGTH, (WPARAM) GetLinePos(lineIndex), 0);
}

int CEditBox::GetLinePos(int lineIndex)
//
{
    return SendMessage(winHandle, EM_LINEINDEX, (WPARAM) lineIndex, 0);
}

int CEditBox::GetLineCount(void)
//
{
    return SendMessage(winHandle, EM_GETLINECOUNT, 0, 0);
}

CListBox::CListBox(void)
//Setup listbox control object
{
    Init();
}

CListBox::CListBox(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup listbox control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CListBox::Init(void)
//
{
    winClassName = "LISTBOX";
    winDefStyle = LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_VSCROLL;
    winDefExStyle = WS_EX_CLIENTEDGE;
}

bool CListBox::AddItem(const char* itemTextStr)
//Add item to the end of the listbox
{
    if(SendMessage(winHandle, LB_ADDSTRING, 0, (LPARAM) itemTextStr) < 0)
        return false;
    else
        return true;
}

bool CListBox::InsertItem(const char* itemTextStr, int itemPos)
//Insert item into the listbox at the given position
{
    if(SendMessage(winHandle, LB_INSERTSTRING, itemPos, (LPARAM) itemTextStr) < 0)
        return false;
    else
        return true;
}

bool CListBox::RemoveItem(int itemPos)
//Remove an item from the listbox
{
    if(SendMessage(winHandle, LB_DELETESTRING, itemPos, 0) < 0)
        return false;
    else
        return true;
}

int CListBox::GetItemCount(void)
//Return the number of items in the list box
{
    return SendMessage(winHandle, LB_GETCOUNT, 0, 0);
}

bool CListBox::GetItemText(int itemPos, string& itemTextStr)
//Get the text of the given item
{
    char* textStr;
    int textStrLen;

    textStrLen = SendMessage(winHandle, LB_GETTEXTLEN, itemPos, 0);
    if(textStrLen < 0)
        return false;

    textStr = new char[textStrLen + 1];

    if(SendMessage(winHandle, LB_GETTEXT, itemPos, (LPARAM) textStr) < 0)
    {
	    delete [] textStr;
        return false;
    }

    itemTextStr = textStr;
    delete [] textStr;

    return true;
}

bool CListBox::SetItemText(int itemPos, const char* itemTextStr)
//Set the text of the given item
{
    if(SendMessage(winHandle, LB_DELETESTRING, itemPos, 0) < 0)
    {
        return false;
    }

    if(SendMessage(winHandle, LB_INSERTSTRING, itemPos, (LPARAM) itemTextStr) < 0)
    {
        return false;
    }

    return true;
}

int CListBox::GetSelIndex(void)
//Get the index of the currently selected item
{
    return SendMessage(winHandle, LB_GETCURSEL, 0, 0);
}

bool CListBox::SetSelIndex(int newSelPos)
//Select the given item
{
    if((SendMessage(winHandle, LB_SETCURSEL, newSelPos, 0) < 0) && (newSelPos != -1))
        return false;
    else
        return true;
}


CComboBox::CComboBox(void)
//Setup combobox control object
{
    Init();
}

CComboBox::CComboBox(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup combobox control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CComboBox::Init(void)
//
{
    winClassName = "COMBOBOX";
    winDefStyle = CBS_DROPDOWN | CBS_AUTOHSCROLL | WS_VSCROLL;
    winDefExStyle = WS_EX_CLIENTEDGE;
}

bool CComboBox::AddItem(const char* itemTextStr)
//Add item to the end of the combobox
{
    if(SendMessage(winHandle, CB_ADDSTRING, 0, (LPARAM) itemTextStr) < 0)
        return false;
    else
        return true;
}

bool CComboBox::InsertItem(const char* itemTextStr, int itemPos)
//Insert item into the combobox at the given position
{
    if(SendMessage(winHandle, CB_INSERTSTRING, itemPos, (LPARAM) itemTextStr) < 0)
        return false;
    else
        return true;
}

bool CComboBox::RemoveItem(int itemPos)
//Remove an item from the combobox
{
    if(SendMessage(winHandle, CB_DELETESTRING, itemPos, 0) < 0)
        return false;
    else
        return true;
}

int CComboBox::GetItemCount(void)
//Return the number of items in the combobox
{
    return SendMessage(winHandle, CB_GETCOUNT, 0, 0);
}

bool CComboBox::GetItemText(int itemPos, string& itemTextStr)
//Get the text of the given item
{
    char* textStr;
    int textStrLen;

    textStrLen = SendMessage(winHandle, CB_GETLBTEXTLEN, itemPos, 0);
    if(textStrLen < 0)
        return false;

    textStr = new char[textStrLen + 1];

    if(SendMessage(winHandle, CB_GETLBTEXT, itemPos, (LPARAM) textStr) < 0)
    {
	    delete [] textStr;
        return false;
    }

    itemTextStr = textStr;
    delete [] textStr;

    return true;
}

bool CComboBox::SetItemText(int itemPos, const char* itemTextStr)
//Set the text of the given item
{
    if(SendMessage(winHandle, CB_DELETESTRING, itemPos, 0) < 0)
    {
        return false;
    }

    if(SendMessage(winHandle, CB_INSERTSTRING, itemPos, (LPARAM) itemTextStr) < 0)
    {
        return false;
    }

    return true;
}

int CComboBox::GetSelIndex(void)
//Get the index of the currently selected item
{
    return SendMessage(winHandle, CB_GETCURSEL, 0, 0);
}

bool CComboBox::SetSelIndex(int newSelPos)
//Select the given item
{
    if((SendMessage(winHandle, CB_SETCURSEL, newSelPos, 0) < 0) && (newSelPos != -1))
        return false;
    else
        return true;
}

CTabList::CTabList(void)
//Setup tab control object
{
    Init();
}

CTabList::CTabList(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup tab control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CTabList::Init(void)
//
{
    winClassName = WC_TABCONTROL;
}

bool CTabList::AddTab(const char* tabTextStr)
//
{
    int tabPos;

    tabPos = SendMessage(winHandle, TCM_GETITEMCOUNT, 0, 0);
    return InsertTab(tabTextStr, tabPos);
}

bool CTabList::InsertTab(const char* tabTextStr, int tabPos)
//Insert a new tab at the given position
{
    TCITEM tabInfo;

    tabInfo.mask = TCIF_TEXT;
    tabInfo.pszText = (LPTSTR) tabTextStr;

    if(SendMessage(winHandle, TCM_INSERTITEM, tabPos, (LPARAM) &tabInfo) < 0)
        return false;
    else
        return true;
}

bool CTabList::RemoveTab(int tabPos)
//
{
    return (SendMessage(winHandle, TCM_DELETEITEM, tabPos, 0) != FALSE);
}

int CTabList::GetTabCount(void)
//
{
    return SendMessage(winHandle, TCM_GETITEMCOUNT, 0, 0);
}

bool CTabList::GetTabText(int tabPos, string& tabTextStr)
//Get the text of the given tab
{
    TCITEM tabInfo;
    char textBuff[256];
    bool retVal;

    tabInfo.mask = TCIF_TEXT;
    tabInfo.cchTextMax = 256;
    tabInfo.pszText = (LPTSTR) textBuff;

    retVal = (SendMessage(winHandle, TCM_GETITEM, tabPos, (LPARAM) &tabInfo) != FALSE);
    tabTextStr = textBuff;
    return retVal;
}

bool CTabList::SetTabText(int tabPos, const char* tabTextStr)
//Set the text of the given tab
{
    TCITEM tabInfo;

    tabInfo.mask = TCIF_TEXT;
    tabInfo.pszText = (LPTSTR) tabTextStr;

    return (SendMessage(winHandle, TCM_SETITEM, tabPos, (LPARAM) &tabInfo) != FALSE);
}

int CTabList::GetSelIndex(void)
//Get the index of the currently selected tab
{
    return SendMessage(winHandle, TCM_GETCURSEL, 0, 0);
}

bool CTabList::SetSelIndex(int newSelPos)
//Select the given tab
{
    if(SendMessage(winHandle, TCM_SETCURSEL, newSelPos, 0) < 0)
        return false;
    else
        return true;
}

CProgressBar::CProgressBar(void)
//Setup progress bar control object
{
    Init();
}

CProgressBar::CProgressBar(CWindow* winParentPtr, const char* winText, int winX, int winY, int winWidth, int winHeight, DWORD winStyle, DWORD winExStyle)
//Setup progress bar control object and create its window
{
    Init();
    Create(winParentPtr, winText, winX, winY, winWidth, winHeight, winStyle, winExStyle);
}

void CProgressBar::Init(void)
//
{
    winClassName = PROGRESS_CLASS;
}

void CProgressBar::GetRange(int* rangeMin, int* rangeMax)
//
{
    PBRANGE rangeInfo;

    SendMessage(winHandle, PBM_GETRANGE, FALSE, (LPARAM) &rangeInfo);

    *rangeMin = rangeInfo.iLow;
    *rangeMax = rangeInfo.iHigh;
}

void CProgressBar::SetRange(int rangeMin, int rangeMax)
//
{
    SendMessage(winHandle, PBM_SETRANGE, 0, MAKELPARAM(rangeMin, rangeMax));
}

int CProgressBar::GetPos(void)
//
{
    return SendMessage(winHandle, PBM_GETPOS, 0, 0);
}

void CProgressBar::SetPos(int newPos)
//
{
    SendMessage(winHandle, PBM_SETPOS, newPos, 0);
}

CImage::CImage(void)
//Construct blank image object
{
    Init();
}

CImage::CImage(const char* imgFileName)
//Construct image object from a given image file
{
    Init();
    Load(imgFileName);
}

CImage::~CImage(void)
//Delete current image handle (if any)
{
	Unload();
}

bool CImage::Load(const char* newImgFileName, bool loadFromResource)
//Load an image handle from a file or resource
{
    HBITMAP newImgHandle;
    BITMAP imgInfo;

    if(loadFromResource)
        newImgHandle = (HBITMAP) LoadImage(GetModuleHandle(NULL), newImgFileName, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
    else
        newImgHandle = (HBITMAP) LoadImage(NULL, newImgFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    if(newImgHandle == NULL)
        return false;

    Unload();

    imgFileName = newImgFileName;
    imgHandle = newImgHandle;
    GetObject(imgHandle, sizeof(imgInfo), &imgInfo);
    imgWidth = imgInfo.bmWidth;
    imgHeight = imgInfo.bmHeight;

    return true;
}

void CImage::Unload(void)
//
{
	if(imgHandle == NULL)
        return;

    DeleteObject(imgHandle);

    Init();
}

void CImage::Init(void)
//Initialize image object properties
{
    imgFileName = "";
    imgHandle = NULL;
    imgWidth = 0;
    imgHeight = 0;
}

CSpriteFrameSheet::CSpriteFrameSheet(void)
//Construct blank frame sheet object
{
    Init();
}

CSpriteFrameSheet::CSpriteFrameSheet(const char* newSheetFileName, COLORREF maskColor, unsigned int frameRows, unsigned int frameCols)
//Contruct frame sheet object from a given image file
{
    Init();
    Load(newSheetFileName, maskColor, frameRows, frameCols);
}

CSpriteFrameSheet::~CSpriteFrameSheet(void)
//Delete current frame list (if any)
{
    Unload();
}

bool CSpriteFrameSheet::WriteToFile(std::ofstream& fileObj)
//
{
    return false;
}

bool CSpriteFrameSheet::Load(const char* newSheetFileName, COLORREF maskColor, unsigned int frameRows, unsigned int frameCols, bool loadFromResource)
//Load a sheet image from a file or resource
{
    CImage sheetImg;
    HDC screenDC;
    HDC sheetDC;
    HDC frameDC;
    HDC maskDC;
    COLORREF frameDefColor;
    HBITMAP sheetDefBMP;
    HBITMAP frameDefBMP;
    HBITMAP maskDefBMP;
    HBITMAP frameBMP;
    HBITMAP maskBMP;
    unsigned int cutLeft;
    unsigned int cutTop;
    unsigned int tmpIndex;

    if((frameCols < 1) || (frameRows < 1))
        return false;

    if(!sheetImg.Load(newSheetFileName, loadFromResource))
        return false;

    Unload();

    //Setup device contexts
    screenDC = GetDC(NULL);
    sheetDC = CreateCompatibleDC(screenDC);
    frameDC = CreateCompatibleDC(screenDC);
    maskDC = CreateCompatibleDC(screenDC);

    //Select sheet image into its DC, and save default bitmaps
    sheetDefBMP = (HBITMAP) SelectObject(sheetDC, sheetImg.imgHandle);
    frameDefBMP = (HBITMAP) GetCurrentObject(frameDC, OBJ_BITMAP);
    maskDefBMP = (HBITMAP) GetCurrentObject(maskDC, OBJ_BITMAP);

    //Set the frames's background color to mask color
    frameDefColor = SetBkColor(frameDC, maskColor);

    //Set frame dimensions
    sheetFileName = newSheetFileName;
    frameWidth = sheetImg.imgWidth / frameCols;
    frameHeight = sheetImg.imgHeight / frameRows;
    frameCount = frameRows * frameCols;
    frameBMPList = new HBITMAP[frameCount];
    maskBMPList = new HBITMAP[frameCount];

    //Loop through each frame in sheet image
    for(unsigned int currRow = 0; currRow < frameRows; currRow++)
    {
        cutTop = (currRow * frameHeight);
        for(unsigned int currCol = 0; currCol < frameCols; currCol++)
        {
            cutLeft = (currCol * frameWidth);

            //Create the frame and mask bitmaps
            frameBMP = CreateCompatibleBitmap(screenDC, frameWidth, frameHeight);
            maskBMP = CreateBitmap(frameWidth, frameHeight, 1, 1, NULL);

            //Select bitmaps into their DCs
            SelectObject(frameDC, frameBMP);
            SelectObject(maskDC, maskBMP);

            //Copy the frame bitmap from the sheet bitmap
            BitBlt(frameDC, 0, 0, frameWidth, frameHeight, sheetDC, cutLeft, cutTop, SRCCOPY);

            //Set the proper black and white areas of the mask from the frame's background color
            BitBlt(maskDC, 0, 0, frameWidth, frameHeight, frameDC, 0, 0, SRCCOPY);

            //Set the mask-color areas of the frame to black
            BitBlt(frameDC, 0, 0, frameWidth, frameHeight, maskDC, 0, 0, SRCINVERT);

            //Add frame and mask bitmaps to their lists
            tmpIndex = (currRow * frameCols) + currCol;
            frameBMPList[tmpIndex] = frameBMP;
            maskBMPList[tmpIndex] = maskBMP;
        }
    }

    //Set background color back to the default
    SetBkColor(frameDC, frameDefColor);

    //Set all the DC bitmaps back to the defaults
    SelectObject(maskDC, maskDefBMP);
    SelectObject(frameDC, frameDefBMP);
    SelectObject(sheetDC, sheetDefBMP);

    //Cleanup device contexts
    DeleteDC(maskDC);
    DeleteDC(frameDC);
    DeleteDC(sheetDC);
    ReleaseDC(NULL, screenDC);

    return true;
}

void CSpriteFrameSheet::Unload(void)
//Delete both the frame/mask images and the frame/mask lists
{
    if(frameCount == 0)
        return;

    for(unsigned int n = 0; n < frameCount; n++)
    {
        DeleteObject(frameBMPList[n]);
        DeleteObject(maskBMPList[n]);
    }

    delete [] frameBMPList;
    delete [] maskBMPList;

    Init();
}

void CSpriteFrameSheet::Init(void)
//Initialize frame sheet properties
{
    sheetFileName = "";
    frameWidth = 0;
    frameHeight = 0;
    frameCount = 0;
    frameBMPList = NULL;
    maskBMPList = NULL;
}

CSprite::CSprite(void)
//Construct blank sprite object
{
    Init();
}

CSprite::CSprite(CSpriteFrameSheet* newFrameSheetPtr)
//Construct sprite object with a frame sheet
{
    Init();
    SetFrameSheet(newFrameSheetPtr);
}

CSprite::~CSprite(void)
//Cleanup sprite resources
{
    DeleteDC(drawSrcDC);
}

bool CSprite::WriteToFile(std::ofstream& fileObj)
//
{
    return false;
}

void CSprite::SetFrameSheet(CSpriteFrameSheet* newFrameSheetPtr, bool setDrawSizeToFrameSize)
//
{
    frameSheetPtr = newFrameSheetPtr;

    if(frameSheetPtr == NULL)
    {
        drawWidth = 0;
        drawHeight = 0;
        SetFrameRange(0, 0);
    }
    else
    {
        if(setDrawSizeToFrameSize)
        {
            drawWidth = frameSheetPtr->frameWidth;
            drawHeight = frameSheetPtr->frameHeight;
        }
        SetFrameRange(0, (frameSheetPtr->frameCount) - 1);
    }
}

CSpriteFrameSheet* CSprite::GetFrameSheet(void)
//
{
    return frameSheetPtr;
}

unsigned int CSprite::GetCurrFrameIndex(void)
//
{
    return currFrameIndex;
}

void CSprite::SetCurrFrameIndex(unsigned int newFrameIndex)
//Make sure the new index is in bounds
{
    if(frameSheetPtr == NULL)
        return;

    if(newFrameIndex >= frameSheetPtr->frameCount)
        return;

    currFrameIndex = newFrameIndex;
}

void CSprite::GetFrameRange(unsigned int* currStartIndex, unsigned int* currEndIndex)
//Return the current frame range
{
    *currStartIndex = startFrameIndex;
    *currEndIndex = endFrameIndex;
}

void CSprite::SetFrameRange(unsigned int newStartIndex, unsigned int newEndIndex)
//Set a new frame range
{
    if(frameSheetPtr != NULL)
    {
        //Return if new range is out bounds
        if((newStartIndex >= frameSheetPtr->frameCount) || (newEndIndex >= frameSheetPtr->frameCount))
        {
            return;
        }
    }

    startFrameIndex = min(newStartIndex, newEndIndex);
    endFrameIndex = max(newStartIndex, newEndIndex);
    currFrameIndex = startFrameIndex;
}

void CSprite::Draw(CDrawBox* dboxPtr, int offsetLeft, int offsetTop)
//Draw the current frame to the given DrawBox, and advance to the next frame (if applicable)
{
    int destLeft;
    int destTop;
    int destWidth;
    int destHeight;
    HBITMAP drawSrcDefBMP;

    if(frameSheetPtr == NULL)
        return;

    if(frameSheetPtr->frameCount == 0)
        return;

    if(isVisible)
    {
        //Adjust the draw position to the scaling, mirror state, and offset position.
        destWidth = (isScaled ? (frameSheetPtr->frameWidth * scaleWidth) : drawWidth);
        destHeight = (isScaled ? (frameSheetPtr->frameHeight * scaleHeight) : drawHeight);
        destLeft = drawLeft + (isMirroredX ? destWidth : 0) + offsetLeft;
        destTop = drawTop + (isMirroredY ? destHeight : 0) + offsetTop;
        destWidth = (isMirroredX ? -destWidth : destWidth);
        destHeight = (isMirroredY ? -destHeight : destHeight);

        //Draw the sprite frame with mask
        drawSrcDefBMP = (HBITMAP) SelectObject(drawSrcDC, frameSheetPtr->maskBMPList[currFrameIndex]);
        StretchBlt(dboxPtr->activeDC, destLeft, destTop, destWidth, destHeight, drawSrcDC, 0, 0, frameSheetPtr->frameWidth, frameSheetPtr->frameHeight, SRCAND);
        SelectObject(drawSrcDC, frameSheetPtr->frameBMPList[currFrameIndex]);
        StretchBlt(dboxPtr->activeDC, destLeft, destTop, destWidth, destHeight, drawSrcDC, 0, 0, frameSheetPtr->frameWidth, frameSheetPtr->frameHeight, SRCPAINT);
        SelectObject(drawSrcDC, drawSrcDefBMP);
    }

    if(isPlaying)
    {
        //Switch to the next frame if it is time to
        frameDrawCount++;
        if(frameDrawCount >= drawsPerFrame)
        {
            frameDrawCount = 0;
            currFrameIndex++;
            if(currFrameIndex > endFrameIndex)
            {
	            currFrameIndex = startFrameIndex;

                //Switch to the next frame cycle if there is a cycle limit
                if(maxCycles > 0)
                {
                    currCycle++;
                    if(currCycle >= maxCycles)
                    {
                        //Reached the cycle limit, so stop switching frames
                        currCycle = 0;
                        isPlaying = false;
                    }
                }
            }
        }
    }
}

void CSprite::Init(void)
//Initialize sprite properties
{
    spriteName = "";
    drawLeft = 0;
    drawTop = 0;
    drawWidth = 0;
    drawHeight = 0;
    scaleWidth = 1.0f;
    scaleHeight = 1.0f;
    isVisible = true;
    isPlaying = true;
    isMirroredX = false;
    isMirroredY = false;
    isScaled = false;
    drawsPerFrame = 1;
    maxCycles = 0;
    currCycle = 0;
    frameSheetPtr = NULL;
    startFrameIndex = 0;
    endFrameIndex = 0;
    currFrameIndex = 0;
    frameDrawCount = 0;
    drawSrcDC = CreateCompatibleDC(NULL);
    userData = 0;
}


