#ifdef __GNUG__
#pragma implementation "m_pVike.h"
#endif


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if defined(CB_PRECOMP)
#include "sdk.h"
#else
#include "sdk_common.h"
#include "sdk_events.h"
#include "logmanager.h"
#endif

// includes
#include <wx/event.h>

#include "cbvike.h"
#include "debugging.h"

IMPLEMENT_CLASS(cbVike, wxObject)
IMPLEMENT_CLASS(VikeEvtHandler, wxEvtHandler)

// event table for wxVike
BEGIN_EVENT_TABLE(VikeEvtHandler, wxEvtHandler)
    // wxEVT_KEY_DOWN received some special key like ESC, BACKSPCE etc.
    EVT_KEY_DOWN(VikeEvtHandler::OnEspecialKey)
    // wxEVT_CHAR recevied any ASCII characters
    EVT_CHAR(VikeEvtHandler::OnChar)
    EVT_SET_FOCUS(VikeEvtHandler::OnFocus)

#if defined( __WXMSW__	)		// supported only on Win32
#if wxUSE_HOTKEY                // enabled?
#if wxCHECK_VERSION(2, 5, 1)	// and from wxWidgets 2.5.1

    // I don't think this is needed because wxEVT_HOTKEY are generated
    // only in some special cases...
    EVT_HOTKEY(wxID_ANY, VikeEvtHandler::OnChar)
#endif
#endif
#endif

END_EVENT_TABLE()

#if !wxCHECK_VERSION(2, 5, 1)

    // with wx previous to 2.5 we need to use wxWindow::Node* instead
    // of wxWindow::compatibility_iterator (thanks to Sebastien Berthet for this)
#define compatibility_iterator			Node*
#endif

// window names allowed for which vike may attach()
wxArrayString cbVike::usableWindows;                    //+v0.4.4

// ----------------------------------------------------------------------------
// wxBinderEvtHandler
// ----------------------------------------------------------------------------
void VikeEvtHandler::OnChar(wxKeyEvent &p)
{
    m_pVike->OnChar(m_pVikeWin, p);
}

void VikeEvtHandler::OnEspecialKey(wxKeyEvent &p)
{
    m_pVike->OnEspecialKey(m_pVikeWin, p);
}

void VikeEvtHandler::OnFocus(wxFocusEvent &p)
{
    LOGIT(_("on focus"));
    m_pVikeWin->UpdateStatusBar();
    //vike->OnFocus(p, GetNextHandler());
    //((wxWindow*)p.GetWindow())->SetFocus();
    p.Skip();
}

void cbVike::CreateStatusBar()
{
    m_pStatusBar = new wxStatusBar(Manager::Get()->GetAppWindow(),  wxID_ANY, wxSB_NORMAL, wxString((wxChar*)"cbVike"));
    m_pStatusBar->SetFieldsCount(STATUS_FIELD_NUM);
    CodeBlocksDockEvent evt(cbEVT_ADD_DOCK_WINDOW);
    evt.name = _T("Vike status");
    evt.title = _("Vike status");
    evt.pWindow = m_pStatusBar;
    evt.minimumSize.Set(25, 25);
    evt.desiredSize.Set(25, 25);
    evt.floatingSize.Set(150, 25);
    evt.dockSide = CodeBlocksDockEvent::dsBottom;
    evt.hideable = true;
    Manager::Get()->ProcessEvent(evt);
}

void cbVike::ShowStatusBar()
{
    CodeBlocksDockEvent evt(cbEVT_SHOW_DOCK_WINDOW);
    evt.pWindow = m_pStatusBar;
    Manager::Get()->ProcessEvent(evt);
}

void cbVike::HideStatusBar()
{
    CodeBlocksDockEvent evt(cbEVT_HIDE_DOCK_WINDOW);
    evt.pWindow = m_pStatusBar;
    Manager::Get()->ProcessEvent(evt);
}

// ----------------------------------------------------------------------------
//  wxVike Attach
// ----------------------------------------------------------------------------
void cbVike::Attach(wxWindow *p)
{
    //LOGIT( _T("wxVike: Attach for %p and if crash??"), p );
    //((wxScintilla*)p)->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
    LOGIT( _T("wxVike:Attach for [%p]"), p );
    if (!p || IsAttachedTo(p))
        return;		// already attached !!!

    LOGIT( _T("wxVike:Attach2[%p]"), p );

    if (p->GetExtraStyle() & wxWS_EX_TRANSIENT)
        return;		// do not attach ourselves to temporary windows !!

    LOGIT( _T("wxVike:Attach3[%p]"), p );

    //+v0.4.4 we allow only static windows to be attached by codeblocks
    // Disappearing frames/windows cause crashes
    wxString windowName = p->GetName().MakeLower();

    if (wxNOT_FOUND == usableWindows.Index(_T("*"),false)) //+v0.4.4 debugging
        if (wxNOT_FOUND == usableWindows.Index(windowName,false)) //+v0.4.2
        {
            LOGIT( _T("wxVike::Attach skipping [%s]"), p->GetName().c_str() );
            return;
        }

    LOGIT(wxT("wxVike::Attach - attaching to [%s] %p"), p->GetName().c_str(),p);

    // create a new event handler for this window
    wxEvtHandler *h = new VikeEvtHandler(this, p);
    m_arrHandlers.Add((void*)h);
}

// ----------------------------------------------------------------------------
//  wxVike FindHandlerIdxFor
// ----------------------------------------------------------------------------
int cbVike::FindHandlerIdxFor(wxWindow *p) const
{
	for (int i=0; i<(int)m_arrHandlers.GetCount(); i++)
		if (((VikeEvtHandler *)m_arrHandlers.Item(i))->IsAttachedTo(p))
			return i;

	return wxNOT_FOUND;
}

void cbVike::Detach(wxWindow *p, bool deleteEvtHandler)
{
	if (!p || !IsAttachedTo(p))
		return;		// this is not attached...

    LOGIT(wxT("wxKeyBinder::Detach - detaching from [%s] %p"), p->GetName().c_str(),p);

	// remove the event handler
	int idx = FindHandlerIdxFor(p);
	VikeEvtHandler *toremove = (VikeEvtHandler*)m_arrHandlers.Item(idx);
	m_arrHandlers.RemoveAt(idx, 1);

	// the wxBinderEvtHandler will remove itself from p's event handlers
	if (deleteEvtHandler) delete toremove;
}

// ----------------------------------------------------------------------------
wxWindow* cbVike::FindWindowRecursively(const wxWindow* parent, const wxWindow* handle)
// ----------------------------------------------------------------------------
{//+v0.4.4
    if ( parent )
    {
        // see if this is the one we're looking for
        if ( parent == handle )
            return (wxWindow *)parent;

        // It wasn't, so check all its children
        for ( wxWindowList::compatibility_iterator node = parent->GetChildren().GetFirst();
              node;
              node = node->GetNext() )
        {
            // recursively check each child
            wxWindow *win = (wxWindow *)node->GetData();
            wxWindow *retwin = FindWindowRecursively(win, handle);
            if (retwin)
                return retwin;
        }
    }

    // Not found
    return NULL;
}

// ----------------------------------------------------------------------------
wxWindow* cbVike::winExists(wxWindow *parent)
// ----------------------------------------------------------------------------
{ //+v0.4.4

    if ( !parent )
    {
        return NULL;
    }

    // start at very top of wx's windows
    for ( wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetFirst();
          node;
          node = node->GetNext() )
    {
        // recursively check each window & its children
        wxWindow* win = node->GetData();
        wxWindow* retwin = FindWindowRecursively(win, parent);
        if (retwin)
            return retwin;
    }

    return NULL;
}

// ----------------------------------------------------------------------------
//  wxVike DetachAll
// ----------------------------------------------------------------------------
void cbVike::DetachAll()
{
    wxWindow* pwin;
	LOGIT(_T("wxVike::DetachAll - detaching from all my [%d] targets"), GetAttachedWndCount());

	//- delete all handlers (they will automatically remove themselves from
	//- event handler chains)
    //-	for (int i=0; i < (int)m_arrHandlers.GetCount(); i++)
    //-		delete (wxBinderEvtHandler*)m_arrHandlers.Item(i);

	for (int i=0; i < (int)m_arrHandlers.GetCount(); i++)
	 {
        VikeEvtHandler* pHdlr = (VikeEvtHandler*)m_arrHandlers.Item(i);
        pwin = pHdlr->GetTargetWnd();     //+v0.4
        if  (!winExists( pwin ) )
        {   //+v0.4.9
            // tell dtor not to crash by using RemoveEventHander()
            pHdlr->SetWndInvalid(0);
            LOGIT( _T("WxKeyBinder:DetachAll:window NOT found %p <----------"), pwin); //+v0.4.6
        }
        #if LOGGING
         if (pHdlr->GetTargetWnd())
           LOGIT( _T("WxKeyBinder:DetachAll:Deleteing EvtHdlr for [%s] %p"), pwin->GetLabel().GetData(), pwin);     //+v0.4
        #endif
        delete pHdlr;
	 }

	// and clear the array
	m_arrHandlers.Clear();

}//DetachAll

// ----------------------------------------------------------------------------
//  wxVike OnChar
// ----------------------------------------------------------------------------
void cbVike::OnChar(VikeWin *vikeWin, wxKeyEvent &event)
{
    //begin trap the keycode
    bool skip = vikeWin->NormalKeyHandler(event);
    if(skip){
        event.Skip();
    }
}

void cbVike::OnEspecialKey(VikeWin *vikeWin, wxKeyEvent &event)
{
    //begin trap the keycode
    bool skip = vikeWin->EspecialKeyHandler(event);
    if(skip){
        event.Skip();
    }
}

void cbVike::OnFocus(wxFocusEvent &event)
{
    ((wxScintilla*)event.GetEventObject())->StyleResetDefault();
    //((wxScintilla*)event.GetEventObject())->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
    LOGIT(_T("set line"));
}
// ----------------------------------------------------------------------------
