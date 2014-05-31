#ifndef __VIKE_H__
#define __VIKE_H__

#include "debugging.h"
#include "cbstyledtextctrl.h"
#include "vicmd.h"

#define BIND_NONE   0
#define BIND_EVENT  1

enum VikeMode{ INSERT, NORMAL, VISUAL };
enum VikeStateEnum {
    /* ==== Operator commands ========= */
            VIKE_OPERATOR_START,
            VIKE_CHANGE,        // c
            VIKE_DELETE,        // d
            VIKE_YANK,          // y
            VIKE_OPERATOR_END,
    /* ================================ */

    /* == Common and Motion Commands == */
    /*   (need more than one char)      */
            VIKE_REPLACE,       // r
            VIKE_FIND_FORWARD,  // f
            VIKE_FIND_BACKWARD, // F
            VIKE_TILL_FORWARD,  // t
            VIKE_TILL_BACKWARD, // t
            VIKE_CHANGE_IN,     // ci
            VIKE_GO,            // g
            VIKE_Z,             // z
    /* ================================ */

    /* ==== Extra Commands ============ */
            VIKE_SEARCH,        // / (may treated like motion commads)
            VIKE_COMMAND,       // :
    /* ================================ */

    /* ==== Useful states ============= */
            VIKE_START,
            VIKE_END,
            VIKE_INVALID,
    /* ================================ */
};

enum VikeStatusBarField{
    STATUS_COMMAND,
    STATUS_KEY,
    STATUS_FIELD_NUM,
};

extern const int idVikeViewStatusBar;

class ViFunc;
class VikeWin;
class VikeEvtBinder;

/*! Vike status bar */
class VikeStatusBar: public wxStatusBar{
    public:
        VikeStatusBar(wxWindow *window, wxWindowID id, long flag, const wxString &name)
            :wxStatusBar(window, id, flag, name)
        {
            SetFieldsCount(STATUS_FIELD_NUM);
        }

        VikeStatusBar(wxWindow *window, long flag)
            :wxStatusBar(window, wxID_ANY, flag)
        {
            SetFieldsCount(STATUS_FIELD_NUM);
        }

        void OnHide(CodeBlocksEvent& event)
        {
            LOGIT(_T("VikeStatusBar OnClose"));
            Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idVikeViewStatusBar, false);
            event.Skip();
        }
    private:

};

/*! Mainly handles the way to attach to the editor */
class cbVike//: public wxObject
{
    public:
        cbVike() {}

        virtual ~cbVike()
        {
            DetachAll();
        }

        /* All the event handler */
        void OnChar(VikeWin *vike, wxKeyEvent &event);
        void OnFocus(wxFocusEvent &event);
        void OnKeyDown(VikeWin *vike, wxKeyEvent &event);

        /* Attaches this class to the given window. */
        void Attach(cbStyledTextCtrl *controller, cbEditor *editor);

        /* Detach a given window */
        void Detach(cbStyledTextCtrl *controller, bool deleteEvtHandler = true);
        void Detach(cbEditor *editor);

        /* Detach the brother of a given window */
        void DetachBrother(cbStyledTextCtrl *controller);

        /* Detaches this event handler from all the window it's attached to */
        void DetachAll();

        /* Get the amount of window attached */
        int GetAttachedWndCount() const{ return m_arrHandlers.GetCount();}

        /* Get the the windows attached array */
        wxArrayPtrVoid *GetHandlersArr(){ return &m_arrHandlers; }

        /* Returns TRUE if the given window is attached to this keybinder. */
        bool IsAttachedTo(cbStyledTextCtrl *p) const{ return FindHandlerIdxFor(p) != wxNOT_FOUND; }

        /* Returns the index of the handler for the given window. */
        int FindHandlerIdxFor(cbStyledTextCtrl *controller) const;

        /* Return the handler binder for the given window */
        wxArrayPtrVoid *FindHandlerFor(cbEditor *editor) const;
        VikeEvtBinder *FindHandlerFor(cbStyledTextCtrl *controller)
        {
            int index = FindHandlerIdxFor(controller);
            if(index != wxNOT_FOUND){
                return (VikeEvtBinder *)m_arrHandlers[index];
            }
            return nullptr;
        }

        VikeEvtBinder *FindBrother(cbStyledTextCtrl *controller);

    protected:
        wxArrayPtrVoid m_arrHandlers;

    private:
        void ReAttach(cbStyledTextCtrl *controller, cbEditor *editor);
        DECLARE_CLASS(cbVike)
};//cbVike

/*! Bind cbVike to window */
class VikeEvtBinder : public wxEvtHandler
{
    public:
        VikeEvtBinder(cbVike *vike, cbStyledTextCtrl* controller, cbEditor *editor);

        virtual ~VikeEvtBinder();

        /* Returns TRUE if this event handler is attached to the given window. */
        bool IsAttachedTo(cbStyledTextCtrl* p) { return p == m_pTarget; }
        bool IsAttachedTo(cbEditor *p) {return p == m_pEditor; }

        /* Re-attach to current the window */
        void ReAttach(cbStyledTextCtrl *controller, cbEditor *editor);

        /* Returns the cbVike which is called-back by this event handler. */
        cbVike *GetBinder() const      { return m_pVike; }

        VikeWin *GetVikeWin() const    { return m_pVikeWin; }
        cbEditor *GetEditor() const    { return m_pEditor; }
        cbStyledTextCtrl *GetController() const{ return m_pTarget; }

    protected:
        //! The event handler for wxKeyEvents.
        void OnChar(wxKeyEvent &event);
        void OnFocus(wxFocusEvent &event);
        void OnKeyDown(wxKeyEvent &event);

    private:
        cbVike *m_pVike;              // the plugin
        cbStyledTextCtrl* m_pTarget;  // the windows bind to the plugin
        cbEditor *m_pEditor;          // the second windows for split window edit
        VikeWin *m_pVikeWin;          // store state for each window

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

/* Vike Running state */
struct VikeState{
    VikeState(VikeStateEnum newState, int newDupNum = 0)
        :state(newState), dupNum(newDupNum)
    {}

    VikeStateEnum state;
    int dupNum;
};

WX_DEFINE_ARRAY_PTR(struct VikeState *, VikeStateStack);

/*! Every window has an instance of VikeWin which store the state for each window */
class VikeWin{
    public:
        /* Only called with attach */
        VikeWin(cbStyledTextCtrl *target, cbEditor *editor, VikeStatusBar *bar);

        /* Only called with detach where the window is destroied */
        ~VikeWin();

        /* Get embedded status bar */
        VikeStatusBar *GetStatusBar() const { return m_pBuiltinStatusBar; }

        /* Layout status bar */
        void LayoutStatusBar(cbStyledTextCtrl *controller, cbEditor *editor);

        /* The event handler that do something related to each vike window */
        bool OnChar(wxKeyEvent &event);
        bool OnKeyDown(wxKeyEvent &event);

        /* Update caret by current mode */
        void UpdateCaret(wxScintilla* editor);

        /* Called on each vifunc end */
        void Finish(wxScintilla *editor);

        /* Get the caret position before last action not used yet */
        int GetUndoPos();

        /* Change and Get current VIM mode like NORMAL, VISUAL, INSERT, etc */
        void ChangeMode(VikeMode new_mode, wxScintilla *editor);
        int GetMode() const;

        /* Append current pressed key to the key buffer for showing in the bar*/
        void AppendKeyStatus(int key_code);

        /* Clear the pressed key buffer */
        void ClearKeyStatus();

        /* Update status bar */
        void UpdateStatusBar();

        /* Get and Set the state in NORMAL mode */
        void SetState(VikeStateEnum newState);
        void PushState(VikeStateEnum newState);
        void PopState();
        VikeStateEnum GetState();
        VikeStateEnum GetLastState()
        {
            int count = m_state.Count();
            if(count <= 1){
                return VIKE_INVALID;
            }
            return m_state[count - 2]->state;
        }
        void ResetState();

        /* State count means how many duplicate number area appears
           eg. 2d4w -- count = 2
               2dw  -- count = 1  */
        int GetStateCount();

        /* Shift the duplicate number and add current num to it */
        void ShiftAddDupNumber(int num);
        /* whether the duplicate number is typed */
        bool IsDup();
        /* Get the duplicate number */
        int GetDupNumber();
        /* Clear duplicate number */
        void ClearDupNumber();

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

    private:
        void GeneralHandler(int keyCode, bool skip);

        VikeMode m_iMode;
        VikeStateStack m_state;

        /* some wxWindow */
        VikeStatusBar *m_pBuiltinStatusBar;

        VikeSearchCmd m_searchCmd;
        VikeGeneralCmd m_generalCmd;
        VikeHighlight m_highlight;

        wxArrayInt m_arrKey;

        int m_iCaretPos;
};



#endif //__VIKE_H__
