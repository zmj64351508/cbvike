/////////////////////////////////////////////////////////////////////////////
// Name:        vike.cpp
// Purpose:     keyevent handler
// Author:      Jia Wei
// Created:     2011/12/17
// Copyright:   (c) Jia Wei
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "vike.h"
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

#include "vike.h"
#include "debugging.h"

#include "vifunc.h"


// event table for wxVike
IMPLEMENT_CLASS(wxVike, wxObject)
IMPLEMENT_CLASS(wxBinderEvtHandler, wxEvtHandler)

BEGIN_EVENT_TABLE(wxBinderEvtHandler, wxEvtHandler)

    // this is obviously the most important event handler; we don't
    // want to intercept wxEVT_KEY_UP because we don't need them:
    // a command must be immediately executed when one of its shortcuts
    // is sent to the window.
    EVT_KEY_DOWN(wxBinderEvtHandler::OnChar)
//    EVT_SET_FOCUS(wxBinderEvtHandler::OnFocus)

#if defined( __WXMSW__	)		// supported only on Win32
#if wxUSE_HOTKEY                // enabled?
#if wxCHECK_VERSION(2, 5, 1)	// and from wxWidgets 2.5.1

    // I don't think this is needed because wxEVT_HOTKEY are generated
    // only in some special cases...
    EVT_HOTKEY(wxID_ANY, wxBinderEvtHandler::OnChar)
#endif
#endif
#endif

END_EVENT_TABLE()

#if !wxCHECK_VERSION(2, 5, 1)

    // with wx previous to 2.5 we need to use wxWindow::Node* instead
    // of wxWindow::compatibility_iterator (thanks to Sebastien Berthet for this)
#define compatibility_iterator			Node*
#endif

// ------------------
// some statics
// ------------------
/*
   int wxCmd::m_nCmdTypes = 0;
   wxCmd::wxCmdType wxCmd::m_arrCmdType[];
   */
// window names allowed for which vike may attach()
wxArrayString wxVike::usableWindows;                    //+v0.4.4

// ----------------------------------------------------------------------------
// wxBinderEvtHandler
// ----------------------------------------------------------------------------

void wxBinderEvtHandler::OnChar(wxKeyEvent &p)
{
    // we'll just call the wxVike OnChar function telling it
    // to execute the command on the next handler in the chain...
    // we do this because only wxVike holds the array of
    // commands & command-shortcuts...
    m_pBinder->OnChar(this->m_pVike, p, GetNextHandler());
}

void wxBinderEvtHandler::OnFocus(wxFocusEvent &p)
{
    LOGIT(_("on focus"));
    m_pBinder->OnFocus(p, GetNextHandler());
    ((wxWindow*)p.GetWindow())->SetFocus();
}

// ----------------------------------------------------------------------------
//  wxVike Attach
// ----------------------------------------------------------------------------
void wxVike::Attach(wxWindow *p, int flag)
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
    wxEvtHandler *h = new wxBinderEvtHandler(this, p, flag);
    m_arrHandlers.Add((void*)h);
}

// ----------------------------------------------------------------------------
//  wxVike FindHandlerIdxFor
// ----------------------------------------------------------------------------
int wxVike::FindHandlerIdxFor(wxWindow *p) const
{
	for (int i=0; i<(int)m_arrHandlers.GetCount(); i++)
		if (((wxBinderEvtHandler *)m_arrHandlers.Item(i))->IsAttachedTo(p))
			return i;

	return wxNOT_FOUND;
}

void wxVike::Detach(wxWindow *p, bool deleteEvtHandler)
{
	if (!p || !IsAttachedTo(p))
		return;		// this is not attached...

    LOGIT(wxT("wxKeyBinder::Detach - detaching from [%s] %p"), p->GetName().c_str(),p);

	// remove the event handler
	int idx = FindHandlerIdxFor(p);
	wxBinderEvtHandler *toremove = (wxBinderEvtHandler*)m_arrHandlers.Item(idx);
	m_arrHandlers.RemoveAt(idx, 1);

	// the wxBinderEvtHandler will remove itself from p's event handlers
	if (deleteEvtHandler) delete toremove;
}

// ----------------------------------------------------------------------------
wxWindow* wxVike::FindWindowRecursively(const wxWindow* parent, const wxWindow* handle)
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
wxWindow* wxVike::winExists(wxWindow *parent)
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
void wxVike::DetachAll()
{
    wxWindow* pwin;
	LOGIT(_T("wxVike::DetachAll - detaching from all my [%d] targets"), GetAttachedWndCount());

	//- delete all handlers (they will automatically remove themselves from
	//- event handler chains)
    //-	for (int i=0; i < (int)m_arrHandlers.GetCount(); i++)
    //-		delete (wxBinderEvtHandler*)m_arrHandlers.Item(i);

	for (int i=0; i < (int)m_arrHandlers.GetCount(); i++)
	 {
        wxBinderEvtHandler* pHdlr = (wxBinderEvtHandler*)m_arrHandlers.Item(i);
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
void wxVike::OnChar(VikeWin *func, wxKeyEvent &event, wxEvtHandler *next)
{
    //begin trap the keycode
    bool retcode = func->execute(event);
    if(!retcode)
    {
        //have not processed by last action
        event.Skip();
    }
}

void wxVike::OnFocus(wxFocusEvent &event, wxEvtHandler *next)
{
    ((wxScintilla*)event.GetEventObject())->StyleResetDefault();
    //((wxScintilla*)event.GetEventObject())->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
    LOGIT(_T("set line"));
}
// ----------------------------------------------------------------------------
