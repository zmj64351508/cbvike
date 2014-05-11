#ifndef __VIKE_H__
#define __VIKE_H__

#ifdef __GNUG__
#pragma interface "m_pVike.h"
#endif

#include "debugging.h"
#include "vifunc.h"

#define BIND_NONE   0
#define BIND_EVENT  1

#define SET_START_CARET 1

class cbVike: public wxObject
{
    public:
        static wxArrayString usableWindows;

    protected:
        wxArrayPtrVoid m_arrHandlers;

    public:        // miscellaneous

        //! The event handler for wxKeyEvents.
        void OnChar(VikeWin *m_pVike, wxKeyEvent &event);
        void OnFocus(wxFocusEvent &event);
        void OnEspecialKey(VikeWin *m_pVike, wxKeyEvent &event);

        //! Attaches this class to the given window.
        void Attach(wxWindow *p);
        cbVike() {}
        virtual ~cbVike() { DetachAll(); }



        void Detach(wxWindow *p, bool deleteEvtHandler = true);

        //! Detaches this event handler from all the window it's attached to.
        void DetachAll();

        int GetAttachedWndCount() const{ return m_arrHandlers.GetCount();}

        wxArrayPtrVoid *GetHandlersArr(){ return &m_arrHandlers; }

        // returns True if window actually exists
        wxWindow* winExists(wxWindow*);
        wxWindow* FindWindowRecursively(const wxWindow* parent, const wxWindow* handle);

        //! Returns TRUE if the given window is attached to this keybinder.
        bool IsAttachedTo(wxWindow *p) const{ return FindHandlerIdxFor(p) != wxNOT_FOUND; }

        //! Returns the index of the handler for the given window.
        int FindHandlerIdxFor(wxWindow *p) const;

        // status bar
        void CreateStatusBar();
        void ShowStatusBar();
        void HideStatusBar();
        wxStatusBar *GetStatusBar(){ return m_pStatusBar; };

    private:
        wxStatusBar *m_pStatusBar;
        DECLARE_CLASS(cbVike)
};//cbVike

class VikeEvtHandler : public wxEvtHandler
{
    //! The wxVike called by wxBinderEvtHandler when receiving a wxKeyEvent.
    cbVike *m_pVike;

    //! The target window which will process the keyevents
    wxWindow *m_pTarget;

    VikeWin *m_pVikeWin;
    protected:

    //! The event handler for wxKeyEvents.
    void OnChar(wxKeyEvent &event);
    void OnFocus(wxFocusEvent &event);
    void OnEspecialKey(wxKeyEvent &event);

    public:

    //! Attaches this event handler to the given window.
    //! The given vike will be called on each keyevent.
    VikeEvtHandler(cbVike *vike, wxWindow *tg)
        : m_pVike(vike), m_pTarget(tg)
    {
        m_pVikeWin = new VikeWin(vike->GetStatusBar());
        //if(flag == SET_START_CARET){
        m_pVikeWin->SetStartCaret((wxScintilla*)tg);
        //}
        m_pTarget->PushEventHandler(this);
    }

    //! Removes this event handler from the window you specified
    //! during construction (the target window).
    virtual ~VikeEvtHandler()
    {
        if ( m_pTarget ) m_pTarget->RemoveEventHandler(this);
        if ( m_pVikeWin ) delete m_pVikeWin;
    }


    //! Returns TRUE if this event handler is attached to the given window.
    bool IsAttachedTo(wxWindow *p)
    { return p == m_pTarget; }

    //! Returns the wxVike which is called-back by this event handler.
    cbVike *GetBinder() const
    { return m_pVike; }

    //! Returns the window which this event handler is filtering.
    wxWindow *GetTargetWnd() const
    { return m_pTarget; }

    wxWindow* SetWndInvalid(wxWindow* pnewWnd=0)
    { m_pTarget = pnewWnd; return m_pTarget; }


    private:
    DECLARE_CLASS(VikeEvtHandler)
    DECLARE_EVENT_TABLE()
};

#endif //__VIKE_H__
