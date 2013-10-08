#ifndef __VIKE_H__
#define __VIKE_H__

#ifdef __GNUG__
#pragma interface "vike.h"
#endif

#include "debugging.h"
#include "vifunc.h"

#define BIND_NONE   0
#define BIND_EVENT  1

#define SET_START_CARET 1

class wxVike;

class wxBinderEvtHandler : public wxEvtHandler
{
    //! The wxVike called by wxBinderEvtHandler when receiving a wxKeyEvent.
    wxVike *m_pBinder;

    //! The target window which will process the keyevents
    wxWindow *m_pTarget;

    VikeWin *m_pVike;
    protected:

    //! The event handler for wxKeyEvents.
    void OnChar(wxKeyEvent &event);
    void OnFocus(wxFocusEvent &event);

    public:

    //! Attaches this event handler to the given window.
    //! The given vike will be called on each keyevent.
    wxBinderEvtHandler(wxVike *p, wxWindow *tg, int flag)
        : m_pBinder(p), m_pTarget(tg)
    {
        m_pTarget->PushEventHandler(this);
        m_pVike= new VikeWin();
        if(flag == SET_START_CARET){
            m_pVike->SetStartCaret((wxScintilla*)tg);
        }
    }

    //! Removes this event handler from the window you specified
    //! during construction (the target window).
    virtual ~wxBinderEvtHandler()
    {
        if ( m_pTarget ) m_pTarget->RemoveEventHandler(this);
        if ( m_pVike ) delete m_pVike;
    }


    //! Returns TRUE if this event handler is attached to the given window.
    bool IsAttachedTo(wxWindow *p)
    { return p == m_pTarget; }

    //! Returns the wxVike which is called-back by this event handler.
    wxVike *GetBinder() const
    { return m_pBinder; }

    //! Returns the window which this event handler is filtering.
    wxWindow *GetTargetWnd() const
    { return m_pTarget; }

    wxWindow* SetWndInvalid(wxWindow* pnewWnd=0)
    { m_pTarget=pnewWnd; return m_pTarget; }


    private:
    DECLARE_CLASS(wxBinderEvtHandler)
    DECLARE_EVENT_TABLE()
};

class wxVike : public wxObject
{
    public:
        static wxArrayString usableWindows;

    protected:
        wxArrayPtrVoid m_arrHandlers;

    public:        // miscellaneous

        //! The event handler for wxKeyEvents.
        void OnChar(VikeWin *vike, wxKeyEvent &event, wxEvtHandler *next);
        void OnFocus(wxFocusEvent &event, wxEvtHandler *next);

        //! Attaches this class to the given window.
        void Attach(wxWindow *p, int flag);

        wxVike() {}
        virtual ~wxVike() { DetachAll(); }



        void Detach(wxWindow *p, bool deleteEvtHandler = true);

        //! Detaches this event handler from all the window it's attached to.
        void DetachAll();

        int GetAttachedWndCount() const {
            return m_arrHandlers.GetCount();
        }

        wxArrayPtrVoid *GetHandlersArr()        { return &m_arrHandlers; }

        // returns True if window actually exists
        wxWindow* winExists(wxWindow*);
        wxWindow* FindWindowRecursively(const wxWindow* parent, const wxWindow* handle);

        //! Returns TRUE if the given window is attached to this keybinder.
        bool IsAttachedTo(wxWindow *p) const
        { return FindHandlerIdxFor(p) != wxNOT_FOUND; }

        //! Returns the index of the handler for the given window.
        int FindHandlerIdxFor(wxWindow *p) const;


    private:
        DECLARE_CLASS(wxVike)
};//wxVike

#endif //__VIKE_H__
