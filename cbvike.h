#ifndef __VIKE_H__
#define __VIKE_H__

#ifdef __GNUG__
#pragma interface "cbvike.h"
#endif

#include "debugging.h"
#include "vifunc.h"
#include "cbstyledtextctrl.h"

#define BIND_NONE   0
#define BIND_EVENT  1

enum VikeMode{ INSERT, NORMAL, VISUAL };
enum VikeState {
    VIKE_START,
    VIKE_SEARCH,        // /
    VIKE_COMMAND,       // :
    VIKE_REPLACE,       // r
    VIKE_FIND_FORWARD,  // f
    VIKE_FIND_BACKWORD, // F
    VIKE_CHANGE,        // c
    VIKE_CHANGE_IN,     // ci
    VIKE_DELETE,        // d
    VIKE_YANK,          // y
    VIKE_EXTRA,         // g
    VIKE_END,
};

enum VikeStatusBarField{
    STATUS_COMMAND,
    STATUS_KEY,
    STATUS_FIELD_NUM,
};

class VikeWin;

/*! Vike status bar */
class VikeStatusBar{

};

/*! Mainly handles the way to attach to the editor */
class cbVike: public wxObject
{
    public:
        static wxArrayString usableWindows;

        cbVike() {}

        /* All the event handler */
        void OnChar(VikeWin *m_pVike, wxKeyEvent &event);
        void OnFocus(wxFocusEvent &event);
        void OnKeyDown(VikeWin *m_pVike, wxKeyEvent &event);

        /* Attaches this class to the given window. */
        void Attach(wxWindow *p);

        virtual ~cbVike()
        {
            if(m_pStatusBar){
                delete m_pStatusBar;
            }
            DetachAll();
        }

        /* Detach a given window */
        void Detach(wxWindow *p, bool deleteEvtHandler = true);

        /* Detaches this event handler from all the window it's attached to */
        void DetachAll();

        /* Get the amount of window attached */
        int GetAttachedWndCount() const{ return m_arrHandlers.GetCount();}

        /* Get the the windows attached array */
        wxArrayPtrVoid *GetHandlersArr(){ return &m_arrHandlers; }

        /* returns True if window actually exists */
        wxWindow* winExists(wxWindow*);
        wxWindow* FindWindowRecursively(const wxWindow* parent, const wxWindow* handle);

        /* Returns TRUE if the given window is attached to this keybinder. */
        bool IsAttachedTo(wxWindow *p) const{ return FindHandlerIdxFor(p) != wxNOT_FOUND; }

        /* Returns the index of the handler for the given window. */
        int FindHandlerIdxFor(wxWindow *p) const;

        /* Create the status bar */
        void CreateStatusBar();

        /* Show the status bar */
        void ShowStatusBar();

        /* Hide the status bar */
        void HideStatusBar();

        /* Get the status bar */
        wxStatusBar *GetStatusBar(){ return m_pStatusBar; };

    protected:
        wxArrayPtrVoid m_arrHandlers;

    private:
        wxStatusBar *m_pStatusBar;
        DECLARE_CLASS(cbVike)
};//cbVike

/*! Bind cbVike to window */
class VikeEvtBinder : public wxEvtHandler
{
    public:
        VikeEvtBinder(cbVike *vike, wxWindow *tg);

        virtual ~VikeEvtBinder();

        /* Returns TRUE if this event handler is attached to the given window. */
        bool IsAttachedTo(wxWindow *p) { return p == m_pTarget; }

        /* Returns the cbVike which is called-back by this event handler. */
        cbVike *GetBinder() const      { return m_pVike; }

        /* Returns the window which this event handler is filtering. */
        wxWindow *GetTargetWnd() const { return m_pTarget; }

        wxWindow* SetWndInvalid(wxWindow* pnewWnd=0) { m_pTarget = pnewWnd; return m_pTarget; }

    protected:
        //! The event handler for wxKeyEvents.
        void OnChar(wxKeyEvent &event);
        void OnFocus(wxFocusEvent &event);
        void OnKeyDown(wxKeyEvent &event);

    private:
        cbVike *m_pVike;        // the plugin
        wxWindow *m_pTarget;    // the windows bind to the plugin
        VikeWin *m_pVikeWin;    // store state for each window

        DECLARE_CLASS(VikeEvtBinder)
        DECLARE_EVENT_TABLE()
};

/*! Manage the highlight state */
class VikeHighlight{
    public:
        VikeHighlight(): m_iIndicator(VIKE_INDIC_START)
        {
            m_colour = wxColor(255, 0, 0);
        }

        /* Disable all the highlight */
        void Hide(wxScintilla *editor)
        {
            editor->SetIndicatorCurrent(m_iIndicator);
            editor->IndicatorClearRange(0, editor->GetLength());
        }

        /* Show the highlight */
        void Show(wxScintilla *editor)
        {
            editor->SetIndicatorCurrent(m_iIndicator);
            editor->IndicatorSetStyle(m_iIndicator, wxSCI_INDIC_HIGHLIGHT);
            editor->IndicatorSetForeground(m_iIndicator, m_colour);
        }

        /* Set highlight colour */
        void SetColour(wxColour colour)
        {
            m_colour = colour;
        }

        /* Set the position to be hightlighted */
        void SetPos(int pos, int len, wxScintilla *editor)
        {
            editor->SetIndicatorCurrent(m_iIndicator);
            editor->IndicatorFillRange(pos, len);
        }

    private:
        int m_iIndicator;
        wxColour m_colour;
};

/*! Every window has an instance of VikeWin which store the state for each window */
class VikeWin{
    public:
        VikeWin(wxStatusBar *sb);

        /* The event handler that do something related to each vike window */
        bool OnChar(wxKeyEvent &event);
        bool OnKeyDown(wxKeyEvent &event);

        /* set caret when startup */
        void SetStartCaret(wxScintilla* editor) const;

        /* Called on each vifunc end */
        void Finish(wxScintilla *editor);

        /* Get the caret position before last action not used yet */
        int GetUndoPos();

        /* Change and Get current VIM mode like NORMAL, VISUAL, INSERT, etc */
        void ChangeMode(VikeMode new_mode);
        int GetMode() const;

        /* Append current pressed key to the key buffer for showing in the bar*/
        void AppendKeyStatus(int key_code);

        /* Clear the pressed key buffer */
        void ClearKeyStatus();

        /* Update status bar */
        void UpdateStatusBar();

        /* Get and Set the state in NORMAL mode */
        void SetState(VikeState new_state);
        int GetState();

        /* Shift the duplicate number and add current num to it */
        void ShiftAddDupNumber(int num);

        /* whether the duplicate number is typed */
        bool IsDup();

        /* Get the duplicate number */
        int GetDupNumber();

        /* Get search command start with '/' */
        VikeSearchCmd &GetSearchCmd();

        /* Get general command start with ':' */
        VikeGeneralCmd &GetGeneralCmd();

        /* Get the highlight state */
        VikeHighlight &GetHighlight();

        /* Clear both search and general command */
        void ClearCmd();

        // the executing function
        ViFunc *func;
        // current input character
        char Char;

    private:
        VikeMode m_iMode;
        VikeState m_iState;

        wxStatusBar *m_pStatusBar;

        VikeSearchCmd m_searchCmd;
        VikeGeneralCmd m_generalCmd;
        VikeHighlight m_highlight;

        wxArrayInt m_arrKey;

        int m_iDupNumber;
        int m_iModifier;
        int m_iCaretPos;
};

#endif //__VIKE_H__
