//WinGUI Wrapper v1.03
//Written by Andrew Sturges
//July 2019

#ifndef _WINGUI_H
#define _WINGUI_H

//For commctrl.h
#define _WIN32_IE 0x0300

#include <string>
#include <vector>
#include <windows.h>
#include <commctrl.h>

extern const char* mainWinClassName;
extern const char* modalWinClassName;
extern const char* drawBoxClassName;

enum WinEventType
{
    WINEVENT_MESSAGE,
    WINEVENT_COMMAND,
    WINEVENT_NOTIFY
};

class CWinEvent;
class CWindow;
class CMainWindow;
class CModalWindow;
class CDrawBox;
class CControl;
class CButton;
class CRadioButton;
class CCheckBox;
class CGroupBox;
class CImageButton;
class CLabel;
class CEditBox;
class CListBox;
class CComboBox;
class CTabList;
class CProgressBar;
class CImage;
class CSpriteFrameSheet;
class CSprite;

bool WinGUISetup(void);
bool WinGUICleanup(void);
WPARAM InputWinEvents(void);
void FlushWinEvents(MSG* msgInfoPtr = NULL);
LRESULT CALLBACK MainWinProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ModalWinProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ControlProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DrawBoxProc(HWND, UINT, WPARAM, LPARAM);
CWindow* SetupWinObj(HWND, CREATESTRUCT*);
void CleanupWinObj(CWindow*);
bool CallWinEventFunc(CWindow*, UINT, WPARAM, LPARAM, LRESULT&);

class CWinEvent
{
  public:

    UINT eventID;
    WinEventType eventType;
    LRESULT (*eventFuncPtr)(CWindow*, const CWinEvent&);
    LONG userData;
    WPARAM wParam;
    LPARAM lParam;

    CWinEvent(void);
    CWinEvent(UINT, LRESULT (*newEventFuncPtr)(CWindow*, const CWinEvent&), WinEventType, LONG);
};

class CWindow
{
  public:

    std::string winName;
    HWND winHandle;
    std::string winClassName;
    DWORD winDefStyle;
    DWORD winDefExStyle;
    bool deleteOnWinDestroy;
    bool isBeingDeleted;
    LONG userData;
    void (*defEventFuncPtr)(CWindow*, UINT, WPARAM, LPARAM);
    std::vector<CWinEvent> winEventList;

    CWindow(void);
    virtual ~CWindow(void);

    void AddEvent(UINT, LRESULT (*newEventFuncPtr)(CWindow*, const CWinEvent&), WinEventType newEventType = WINEVENT_MESSAGE, LONG newUserData = 0);
    bool RemoveEvent(UINT, WinEventType);
    int GetEventIndex(UINT, WinEventType);
    void Destroy(void);
    void Show(void);
    void Hide(void);
    void Enable(void);
    void Disable(void);
    std::string GetText(void);
    void SetText(const char*);
    void GetPos(int*, int*);
    void GetSize(int*, int*);
    void GetClientSize(int*, int*);
    void SetPos(int, int);
    void SetSize(int, int);
    void SetClientSize(int, int);
    LONG GetStyle(void);
    LONG GetStyleEx(void);
    LONG SetStyle(LONG);
    LONG SetStyleEx(LONG);
    CWindow* GetParentWin(void);
    void SetParentWin(CWindow*);
    virtual void GetFontInfo(LOGFONT*);
    virtual void SetFontInfo(LOGFONT*);
    std::string GetFontName(void);
    void SetFontName(const char*);
    void GetFontSize(int*, int*);
    void SetFontSize(int, int);
    LONG GetFontWeight(void);
    void SetFontWeight(LONG);
    void GetFontStyle(BYTE*, BYTE*, BYTE*);
    void SetFontStyle(BYTE, BYTE, BYTE);
};

class CMainWindow : public CWindow
{
  public:

    CMainWindow(void);
    CMainWindow(const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    bool Create(const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);
};

class CModalWindow : public CWindow
{
  public:

    BOOL ownerOrigState;

    CModalWindow(void);
    CModalWindow(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    bool Create(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);
};

class CDrawBox : public CWindow
{
  public:

    HBITMAP redrawBMP;
    HBITMAP redrawDefBMP;
    HPEN redrawDefPen;
    HBRUSH redrawDefBrush;
    HFONT redrawDefFont;
    int redrawWidth;
    int redrawHeight;
    HDC redrawDC;
    HPEN viewDefPen;
    HBRUSH viewDefBrush;
    HFONT viewDefFont;
    HDC viewDC;
    HPEN drawPen;
    HBRUSH drawBrush;
    HFONT drawFont;
    HDC activeDC;
    bool autoRedraw;

    CDrawBox(void);
    CDrawBox(CWindow*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    bool Create(CWindow*, int, int, int, int, DWORD, DWORD winExStyle = 0);
    void SetupDrawData(void);
    void CleanupDrawData(void);
    void ResizeRedrawDC(int, int);
    void UseRedrawDC(void);
    void UseViewDC(void);
    void Redraw(void);
    void RedrawArea(int, int, int, int);
    void Stick(void);
    void StickArea(int, int, int, int);
    void CopyAreaTo(CDrawBox*, int, int, int, int, int, int);
    int GetBackMode(void);
    void SetBackMode(int);
    int GetROP(void);
    void SetROP(int);
    COLORREF GetBackColor(void);
    void SetBackColor(COLORREF);
    COLORREF GetForeColor(void);
    void SetForeColor(COLORREF);
    void GetFontInfo(LOGFONT*);
    void SetFontInfo(LOGFONT*);
    void GetPenInfo(LOGPEN*);
    void SetPenInfo(LOGPEN*);
    UINT GetPenStyle(void);
    void SetPenStyle(UINT);
    LONG GetPenSize(void);
    void SetPenSize(LONG);
    COLORREF GetPenColor(void);
    void SetPenColor(COLORREF);
    void GetBrushInfo(LOGBRUSH*);
    void SetBrushInfo(LOGBRUSH*);
    UINT GetBrushStyle(void);
    void SetBrushStyle(UINT);
    COLORREF GetBrushColor(void);
    void SetBrushColor(COLORREF);
    COLORREF GetPoint(int, int);
    void SetPoint(int, int, COLORREF);
    void DrawText(const char*, int, int);
    void DrawLine(int, int, int, int);
    void DrawRect(int, int, int, int);
    void DrawCircle(int, int, int);
    void DrawImage(CImage*, int, int, DWORD rOp = SRCCOPY);
    void DrawImageScaled(CImage* imgPtr, int, int, int, int, DWORD rOp = SRCCOPY);
};

class CControl : public CWindow
{
  public:

    WNDPROC oldCtlProc;
    HFONT ctlDefFont;
    HFONT ctlFont;

    CControl(void);

    void Init(void);
    bool Create(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);
    void SetupControlData(void);
    void CleanupControlData(void);
    void GetFontInfo(LOGFONT*);
    void SetFontInfo(LOGFONT*);
};

class CButton : public CControl
{
  public:

    CButton(void);
    CButton(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    void SetImage(CImage*);
};

class CRadioButton : public CControl
{
  public:

    CRadioButton(void);
    CRadioButton(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    LRESULT GetState(void);
    void SetState(WPARAM);
};

class CCheckBox : public CControl
{
  public:

    CCheckBox(void);
    CCheckBox(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    LRESULT GetState(void);
    void SetState(WPARAM);
};

class CGroupBox : public CControl
{
  public:

    CGroupBox(void);
    CGroupBox(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
};

class CLabel : public CControl
{
  public:

    CLabel(void);
    CLabel(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
};

class CEditBox : public CControl
{
  public:

    CEditBox(void);
    CEditBox(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    void AppendText(const char*);
    std::string GetSelText(void);
    void SetSelText(const char*);
    void GetSelRange(int*, int*);
    void SetSelRange(int, int);
    std::string GetLineText(int);
    int GetLineLen(int);
    int GetLinePos(int);
    int GetLineCount(void);
};

class CListBox : public CControl
{
  public:

    CListBox(void);
    CListBox(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    bool AddItem(const char*);
    bool InsertItem(const char*, int);
    bool RemoveItem(int);
    int GetItemCount(void);
	bool GetItemText(int, std::string&);
	bool SetItemText(int, const char*);
	int GetSelIndex(void);
	bool SetSelIndex(int);
};

class CComboBox : public CControl
{
  public:

    CComboBox(void);
    CComboBox(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    bool AddItem(const char*);
    bool InsertItem(const char*, int);
    bool RemoveItem(int);
    int GetItemCount(void);
	bool GetItemText(int, std::string&);
	bool SetItemText(int, const char*);
	int GetSelIndex(void);
	bool SetSelIndex(int);
};

class CTabList : public CControl
{
  public:

    CTabList(void);
    CTabList(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    bool AddTab(const char*);
    bool InsertTab(const char*, int);
    bool RemoveTab(int);
    int GetTabCount(void);
	bool GetTabText(int, std::string&);
	bool SetTabText(int, const char*);
	int GetSelIndex(void);
	bool SetSelIndex(int);
};

class CProgressBar : public CControl
{
  public:

    CProgressBar(void);
    CProgressBar(CWindow*, const char*, int, int, int, int, DWORD, DWORD winExStyle = 0);

    void Init(void);
    void GetRange(int*, int*);
    void SetRange(int, int);
    int GetPos(void);
    void SetPos(int);
};

class CImage
{
  public:

    std::string imgName;
    std::string imgFileName;
    HBITMAP imgHandle;
	unsigned int imgWidth;
	unsigned int imgHeight;

    CImage(void);
    CImage(const char*);
    ~CImage(void);

    bool Load(const char*, bool loadFromResource = false);
    void Unload(void);

  private:

    void Init(void);
};

class CSpriteFrameSheet
{
  public:

    std::string sheetName;
    std::string sheetFileName;
    HBITMAP* frameBMPList;
    HBITMAP* maskBMPList;
    unsigned int frameCount;
    unsigned int frameWidth;
    unsigned int frameHeight;

    CSpriteFrameSheet(void);
    CSpriteFrameSheet(const char*, COLORREF, unsigned int, unsigned int);
    ~CSpriteFrameSheet(void);

    bool Load(const char*, COLORREF, unsigned int, unsigned int, bool loadFromResource = false);
    void Unload(void);

  private:

    void Init(void);
};

class CSprite
{
  public:

    std::string spriteName;
    int drawLeft;
    int drawTop;
    int drawWidth;
    int drawHeight;
    float scaleWidth;
    float scaleHeight;
    bool isVisible;
    bool isPlaying;
    bool isMirroredX;
    bool isMirroredY;
    bool isScaled;
    unsigned int drawsPerFrame;
    unsigned int maxCycles;
    unsigned int currCycle;
    LONG userData;

    CSprite(void);
    CSprite(CSpriteFrameSheet*);
    ~CSprite(void);

    void SetFrameSheet(CSpriteFrameSheet*, bool setDrawSizeToFrameSize = true);
    CSpriteFrameSheet* GetFrameSheet(void);
    unsigned int GetCurrFrameIndex(void);
    void SetCurrFrameIndex(unsigned int);
    void GetFrameRange(unsigned int*, unsigned int*);
    void SetFrameRange(unsigned int, unsigned int);
    void Draw(CDrawBox*, int offsetLeft = 0, int offsetTop = 0);

  private:

    void Init(void);
    CSpriteFrameSheet* frameSheetPtr;
    unsigned int startFrameIndex;
    unsigned int endFrameIndex;
    unsigned int currFrameIndex;
    unsigned long frameDrawCount;
    HDC drawSrcDC;
};

#endif

