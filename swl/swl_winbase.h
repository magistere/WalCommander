/*
	Copyright (c) by Valery Goryachev (Wal)
*/


#ifndef WINBASE_H
#define WINBASE_H


namespace wal {

extern int uiClassButton;
extern int uiClassEditLine;
extern int uiClassMenuBar;
extern int uiClassPopupMenu;
extern int uiClassSButton;
extern int uiClassScrollBar;
extern int uiClassTextList;
extern int uiClassVListWin;
extern int uiClassStatic;
extern int uiClassToolTip;

enum BASE_COMMANDS {
	_CMD_BASE_BEGIN_POINT_ = -99,
	CMD_ITEM_CLICK,	//one mouse click
	CMD_ITEM_DOUBLECLICK,	
	CMD_ITEM_ENTER,	//enter key
	CMD_ITEM_CHANGED,
	CMD_SBUTTON_INFO,	//with subcommands
	CMD_SCROLL_INFO,		//with subcommands
	CMD_MENU_INFO,		//with subcommands
	CMD_EDITLINE_INFO	//with subcommands
};

void BaseInit();
void Draw3DButtonW2(GC &gc, crect r, unsigned bg, bool up);

extern unicode_t ABCString[];
extern int ABCStringLen;


class Button: public Win {
	bool pressed;
	wal::carray<unicode_t> text;
	cptr<cicon> icon;
	int commandId;
	int key;
	void SendCommand(){ Command(commandId, 0, this, 0); }
public:
	Button(int nId, Win *parent, const unicode_t *txt,  int id, crect *rect = 0, int iconX = 16, int iconY = 16);
	void Set(const unicode_t *txt, int id, int iconX = 16, int iconY = 16);
	int CommandId() const { return commandId; }
	virtual void Paint(GC &gc, const crect &paintRect);
	virtual bool EventFocus(bool recv);
	virtual bool EventMouse(cevent_mouse* pEvent);
	virtual bool EventKey(cevent_key* pEvent);
	virtual bool EventKeyPost(Win *focusWin, cevent_key* pEvent);
	virtual void OnChangeStyles();
	virtual int UiGetClassId();
	virtual ~Button();
};

enum SButtonInfo {
	// subcommands of CMD_SBUTTON_INFO
	SCMD_SBUTTON_CHECKED=1,
	SCMD_SBUTTON_CLEARED=0
};

//Checkbox and Radiobutton
class SButton: public Win {
	bool isSet;
	carray<unicode_t> text;
	int group;
	int key;
public:
	SButton(int nId, Win *parent, unicode_t *txt, int _group, bool _isSet=false, crect *rect = 0);

	bool IsSet(){ return isSet; }
	void Change(bool _isSet);
	virtual void Paint(GC &gc, const crect &paintRect);
	virtual bool EventMouse(cevent_mouse* pEvent);
	virtual bool EventKey(cevent_key* pEvent);
	virtual bool EventKeyPost(Win *focusWin, cevent_key* pEvent);
	virtual bool EventFocus(bool recv);
	virtual bool Broadcast(int id, int subId, Win *win, void *data);
	virtual int UiGetClassId();
	virtual ~SButton();
};


class EditBuf {
	wal::carray<unicode_t> data;
	int size;	//размер буфера
	int count;	//количесво символов в строке
	int cursor;	//позиция курсора
	int marker;	//точка, от которой до курсора - выделенный блок
	
	enum { STEP = 0x100 };
	void SetSize(int n);
	void InsertBlock(int pos, int n);
	void DeleteBlock(int pos, int n);	

public:
	
	void Begin	(bool mark = false)	{ cursor = 0; if (!mark) marker = cursor; }
	void End	(bool mark = false)	{ cursor = count; if (!mark) marker = cursor; }
	void Left	(bool mark = false)	{ if (cursor > 0) cursor--; if (!mark) marker = cursor; }
	void Right	(bool mark = false)	{ if (cursor < count) cursor++; if (!mark) marker = cursor; }
	
	void CtrlLeft	(bool mark = false);
	void CtrlRight	(bool mark = false);

	void SetCursor	(int n, bool mark = false) { if (n > count) n = count-1; if (n<0) n=0; cursor =n; if (!mark) marker = cursor;}
	void Unmark	()			{ marker = cursor; }
	
	bool Marked() const { return cursor != marker; }
	bool DelMarked();
	bool Marked(int n) const 
		{ return cursor != marker && 
			((cursor<marker && n>=cursor && n<marker) || (cursor>marker && n<cursor && n>=marker)); }

	void Insert(unicode_t t);
	void Insert(const unicode_t *txt);
	void Del();
	void Backspace();
	void Set(const unicode_t *s, bool mark = false);
	
	unicode_t* Ptr(){ return data.ptr(); }
	unicode_t operator [](int n){ return data[n]; }
	
	int	Count()  const { return count; }
	int	Cursor() const { return cursor; }
	int	Marker() const { return marker; }
	
	void	Clear(){ marker = cursor = count = 0; }
	
	EditBuf(const unicode_t *s):size(0), count(0){ Set(s);}
	EditBuf():size(0), count(0), cursor(0), marker(0){}
	
	static int GetCharGroup(unicode_t c); //for word left/right (0 - space)
};

enum EditLineCmd {
	// subcommands of CMD_EDITLINE_INFO
	SCMD_EDITLINE_CHANGED = 100
};


class EditLine: public Win {
public:
	enum FLAGS {
		USEPARENTFOCUS = 1,
		READONLY = 2
	};
private:
	unsigned _flags;
	bool RO() const { return (_flags & READONLY) != 0; }
	EditBuf text;
	int _chars;
	bool cursorVisible;
	bool passwordMode;
	int first;
	bool frame3d;
	CaptureSD captureSD;

	void DrawCursor(GC &gc);
	bool CheckCursorPos(); //true - если нужна перерисовка
	void ClipboardCopy();
	void ClipboardPaste();
	void ClipboardCut();
	int GetCharPos(cpoint p);
	void Changed(){ if (Parent()) Parent()->Command(CMD_EDITLINE_INFO, SCMD_EDITLINE_CHANGED, this, 0); }
	bool NeedDrawFocus();
public:
	EditLine(int nId, Win *parent, const crect *rect, const unicode_t *txt, int chars = 10, bool frame = true, unsigned flags = 0);
	virtual void Paint(GC &gc, const crect &paintRect);
	virtual bool EventMouse(cevent_mouse* pEvent);
	virtual bool EventKey(cevent_key* pEvent);
	virtual void EventTimer(int tid);
	virtual bool EventFocus(bool recv);
	virtual void EventSize(cevent_size *pEvent);
	void Clear();
	void SetText(const unicode_t *txt, bool mark = false);
	void Insert(unicode_t t);
	void Insert(const unicode_t *txt);
	int GetCursorPos(){ return text.Cursor(); }
	void SetCursorPos(int c, bool mark = false){ text.SetCursor(c, mark); }
	carray<unicode_t> GetText();
	void SetPasswordMode(bool enable = true){ passwordMode = enable; Invalidate(); }
	virtual int UiGetClassId();
	virtual void OnChangeStyles();
	virtual ~EditLine();
};

extern cpoint GetStaticTextExtent(GC &gc, const unicode_t *s, cfont *font);

class StaticLine: public Win {
	wal::carray<unicode_t> text;
public:
	StaticLine(int nId, Win *parent, const unicode_t *txt, crect *rect = 0);
	virtual void Paint(GC &gc, const crect &paintRect);
	void SetText(const unicode_t *txt){ text = wal::new_unicode_str(txt); Invalidate(); }
	virtual int UiGetClassId();
	virtual ~StaticLine();
};

//hot key line
class HKStaticLine: public Win {
	int key;
	wal::carray<unicode_t> text;
	Win *controlWin;
public:
	HKStaticLine(int nId, Win *parent, const unicode_t *txt, crect *rect = 0);
	void SetControlWin(Win *w){ controlWin = w; if (IsVisible()) Invalidate(); }
	virtual bool EventKeyPost(Win *focusWin, cevent_key* pEvent);
	virtual int UiGetClassId();
	virtual void Paint(GC &gc, const crect &paintRect);
	virtual ~HKStaticLine();
};

enum ScrollCmd {
	//subcommands of CMD_SCROLL_INFO
	SCMD_SCROLL_VCHANGE=1,
	SCMD_SCROLL_HCHANGE,

	SCMD_SCROLL_LINE_UP,
	SCMD_SCROLL_LINE_DOWN,
	SCMD_SCROLL_LINE_LEFT,
	SCMD_SCROLL_LINE_RIGHT,
	SCMD_SCROLL_PAGE_UP,
	SCMD_SCROLL_PAGE_DOWN,
	SCMD_SCROLL_PAGE_LEFT,
	SCMD_SCROLL_PAGE_RIGHT,
	SCMD_SCROLL_TRACK
};

struct ScrollInfo {
	int size,pageSize,pos;
	ScrollInfo():size(0),pageSize(0),pos(0){}
	ScrollInfo(int _size, int _pageSize, int _pos)
		:size(_size),pageSize(_pageSize),pos(_pos){}
};

class ScrollBar: public Win {
	bool vertical;
	ScrollInfo si;

	int len; //���������� ����� �������� ��������
	int bsize; // ������ ������� ������
	int bpos; // ���������� �� ������� ������ �� ������

	crect b1Rect, b2Rect, b3Rect;

	bool trace; // ��������� �����������
	int traceBPoint; // ����� ������� ���� �� ������ ������� ������ (���� ��� �����������)

	bool autoHide;

	bool b1Pressed;
	bool b2Pressed;

	Win *managedWin;

	CaptureSD captureSD;

	void Recalc(cpoint *newSize=0);
	void SendManagedCmd(int subId, void *data);
public:
	ScrollBar(int nId, Win *parent, bool _vertical, bool _autoHide = true,  crect *rect = 0);
	void SetScrollInfo(ScrollInfo *s);
	void SetManagedWin(Win *w = 0){ managedWin =w; SetScrollInfo(0); }
	virtual void Paint(GC &gc, const crect &paintRect);
	virtual bool Command(int id, int subId, Win *win, void *data);
	virtual bool EventMouse(cevent_mouse* pEvent);
	virtual void EventTimer(int tid);
	virtual void EventSize(cevent_size *pEvent);
	virtual int UiGetClassId();
	virtual ~ScrollBar();
};


class VListWin: public Win {
public:
	enum SelectType { NO_SELECT=0, SINGLE_SELECT, MULTIPLE_SELECT };
	enum BorderType { NO_BORDER=0, SINGLE_BORDER, BORDER_3D};
private:
	SelectType selectType;
	BorderType borderType;

	int itemHeight;
	int itemWidth;

	int xOffset;
	int count;
	int first;
	int current;
	int pageSize;
	int captureDelta;

	unsigned borderColor; //used only for SINGLE_BORDER
	unsigned bgColor; //for 3d border and scroll block

	CaptureSD captureSD;

	ScrollBar vScroll;
	ScrollBar hScroll;

	Layout layout;

	crect listRect; //rect � ������� ���� �������� ������
	crect scrollRect;

	IntList selectList;
	bool CheckPage(bool mustVisibge);
protected:
	void SetCount(int);
	void SetItemSize(int h, int w);
	void CalcScroll();
//	void SetBlockLSize(int);
//	void SetBorderColor(unsigned);

	int GetCount(){ return count; }
	int GetItemHeight(){ return itemHeight; }
	int GetItemWidth(){ return itemWidth; }

	void SetCurrent(int);
public:
	VListWin(WTYPE t, unsigned hints, int nId, Win *_parent, SelectType st, BorderType bt, crect *rect);
	virtual void DrawItem(GC &gc, int n, crect rect);
	
	void MoveCurrent(int n, bool mustVisible=true);
	void MoveFirst(int n);
	void MoveXOffset(int n);
	void SetNoCurrent();

	int GetCurrent() const { return current; }
	int GetPageFirstItem() const { return first; }
	int GetPageItemCount() const { return pageSize; }


	void ClearSelection();
	void ClearSelected(int n1, int n2);
	void SetSelected(int n1, int n2);
	bool IsSelected(int);

	virtual void Paint(GC &gc, const crect &paintRect);
	virtual void EventSize(cevent_size *pEvent);
	virtual bool EventKey(cevent_key* pEvent);
	virtual bool EventMouse(cevent_mouse* pEvent);
	virtual bool EventFocus(bool recv);
	virtual bool Command(int id, int subId, Win *win, void *data);
	virtual void EventTimer(int tid);
	virtual int UiGetClassId();
	virtual ~VListWin();
};


struct TLNode{
	int pixelWidth;
	carray<unicode_t> str;
	long intData;
	void *ptrData;
	TLNode() : pixelWidth(-1), intData(0), ptrData(0){}
	TLNode(const unicode_t *s, int i=0, void *p=0) : pixelWidth(-1), str(new_unicode_str(s)), intData(i), ptrData(p) {}
};


typedef cfbarray<TLNode,0x100,0x100> TLList;

class TextList: public VListWin {
	TLList list;
	bool valid;
	int fontH;
	int fontW;
public:
	TextList(WTYPE t, unsigned hints, int nId, Win *_parent, SelectType st, BorderType bt, crect *rect);
	virtual void DrawItem(GC &gc, int n, crect rect);
	void Clear();
	void Append(const unicode_t *txt, int i=0, void *p=0);
	void DataRefresh();
	
	const unicode_t* GetCurrentString(){ int cnt = list.count(), c = GetCurrent(); return c<0 || c>=cnt ? 0: list.get(c).str.ptr(); }
	void* GetCurrentPtr(){ int cnt = list.count(), c = GetCurrent(); return c<0 || c>=cnt ? 0: list.get(c).ptrData; }
	int GetCurrentInt(){ int cnt = list.count(), c = GetCurrent(); return c<0 || c>=cnt ? -1: list.get(c).intData; }
	
	void SetHeightRange(LSRange range); //in characters
	void SetWidthRange(LSRange range); //in characters
	virtual int UiGetClassId();
	virtual ~TextList();
};



enum CmdMenuInfo {
	//subcommands of CMD_MENU_INFO
		SCMD_MENU_CANCEL=0
};

class PopupMenu;

class MenuData {
public:
	enum TYPE {	CMD = 1, SPLIT,	SUB	};
	
	struct Node {
		int type;
		int id;
		wal::carray<unicode_t> leftText;
		wal::carray<unicode_t> rightText;
		MenuData *sub;
		Node():type(0), sub(0){}
		Node(int _type, int _id, const unicode_t *s,  const unicode_t *rt, MenuData *_sub)
			:type(_type), id(_id), sub(_sub)
			{ 
				if (s) leftText = new_unicode_str(s); 
				if (rt) rightText = new_unicode_str(rt); 
			}
	};
	
private:
	friend class PopupMenu;
	
	wal::ccollect<Node> list;
public:
	MenuData();
	int Count(){ return list.count(); }
	void AddCmd(int id, const unicode_t *s, const unicode_t *rt = 0){ Node node(CMD,id,s,rt,0); list.append(node); }
	void AddCmd(int id, const char *s, const char *rt = 0){ AddCmd(id, utf8_to_unicode(s).ptr(), rt ? utf8_to_unicode(rt).ptr() : 0); }
	void AddSplit(){ Node node(SPLIT,0,0,0,0); list.append(node); }
	void AddSub(const unicode_t *s, MenuData *data){ Node node(SUB,0,s,0,data); list.append(node); } 
	void AddSub(const char *s, MenuData *data){ AddSub(utf8_to_unicode(s).ptr(), data); }
	~MenuData(){}
};


class PopupMenu: public Win {
	struct Node {
		MenuData::Node *data;
		crect rect;
		bool enabled;
	};
	wal::ccollect<Node> list;
	int selected;
	wal::cptr<PopupMenu> sub;
	bool SetSelected(int n);
	bool OpenSubmenu();
	Win *cmdOwner;

	bool IsCmd(int n);
	bool IsSplit(int n);
	bool IsSub(int n);
	bool IsEnabled(int n);

	int fontHeight;
	int leftWidth;
	int rightWidth;

	void DrawItem(GC &gc, int n);
public:
	PopupMenu(int nId, Win*parent, MenuData *d, int x, int y, Win *_cmdOwner=0);
	virtual bool EventMouse(cevent_mouse* pEvent);
	virtual bool EventKey(cevent_key* pEvent);
	virtual bool EventKeyPost(Win *focusWin, cevent_key* pEvent);
	virtual void Paint(GC &gc, const crect &paintRect);
	virtual bool Command(int id, int subId, Win *win, void *data);
	virtual int UiGetClassId();
	virtual ~PopupMenu();
};

int DoPopupMenu(int nId, Win *parent, MenuData *d, int x, int y);

class MenuBar: public Win {
	struct Node {
		wal::carray<unicode_t> text;
		MenuData *data;
	};
	
	wal::ccollect<Node> list;
	wal::ccollect<crect> rectList;
	int select;
	int lastMouseSelect;
	wal::cptr<PopupMenu> sub;

	crect ItemRect(int n);
	void InvalidateRectList(){ rectList.clear(); }
	void DrawItem(GC &gc, int n);
	void SetSelect(int n);
	void OpenSub();
	void Left() { SetSelect( select <= 0 ? (list.count()-1) : (select - 1) ); }
	void Right() { SetSelect( select == list.count()-1  ? 0 : select + 1 ); }
	int GetPointItem(cpoint p);
public:
	MenuBar(int nId, Win *parent, crect *rect = 0);
	void Paint(GC &gc, const crect &paintRect);
	virtual bool EventMouse(cevent_mouse* pEvent);
	virtual bool EventKey(cevent_key* pEvent);
	virtual bool EventKeyPost(Win *focusWin, cevent_key* pEvent);
	virtual bool Command(int id, int subId, Win *win, void *d);
	virtual bool EventFocus(bool recv);
	virtual void EventEnterLeave(cevent *pEvent);
	virtual void EventSize(cevent_size *pEvent);
	virtual int UiGetClassId();
	virtual void OnChangeStyles();

	void Clear(){ list.clear(); InvalidateRectList(); }
	void Add(MenuData *data, const unicode_t *text);
	virtual ~MenuBar();
};

////////////////////////// ComboBox

class ComboBox:public Win {
public:
	enum FLAGS {
		MODE_UP  = 1,
		READONLY = 2,
		FRAME3D  = 4
	};
private:
	unsigned _flags;
	CaptureSD captureSD;
	Layout _lo;
	struct Node {
		carray<unicode_t> text;
		void *data;
	};
	EditLine _edit;
	crect _buttonRect;
	ccollect<Node, 0x100> _list;
	cptr<TextList> _box;
	int _cols;
	int _rows;
	int _current;

	void OpenBox();
	void CloseBox();

public:
	ComboBox(int nId, Win *parent, int cols, int rows, unsigned flags = 0,  crect *rect = 0);
	virtual void Paint(GC &gc, const crect &paintRect);
	virtual bool EventMouse(cevent_mouse* pEvent);
	virtual bool EventKey(cevent_key* pEvent);
	virtual bool EventFocus(bool recv);
	virtual bool Command(int id, int subId, Win *win, void *d);
	virtual void OnChangeStyles();
	virtual int UiGetClassId();
	void Clear();
	void Append(const unicode_t *text, void *data = 0);
	void Append(const char *text, void *data = 0);

	carray<unicode_t> GetText();
	int Count() const { return _list.count(); }
	int Current() const { return _current; }
	const unicode_t* ItemText(int n);
	void * ItemData(int n);
	void MoveCurrent(int n);
	
	virtual ~ComboBox();
};



//ToolTip один на все приложение, поэтому установка нового, удаляет предыдущий
void ToolTipShow(Win *w, int x, int y, const unicode_t *s);
void ToolTipShow(int x, int y, const char *s);
void ToolTipHide();

void HkStringTextOut(GC& gc, int x, int y, const unicode_t *str, int color_text, int color_hotkey);
void HkStringTextOutF(GC& gc, int x, int y, const unicode_t *str, int color_text, int color_hotkey);
cpoint HkStringGetTextExtents(GC& gc, const unicode_t *str);
unicode_t HkStringKey(const unicode_t *str);

}; // namespace wal


#endif