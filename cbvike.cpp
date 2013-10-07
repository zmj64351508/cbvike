/////////////////////////////////////////////////////////////////////////////
// Name:        cbvike.cpp
// Purpose:     cb plugin initial
// Author:      Jia Wei
// Created:     2011/12/17
// Copyright:   (c) Jia Wei
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__)
	#pragma implementation "cbvike.h"
#endif

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/log.h>
#endif
#include "cbstyledtextctrl.h"
#include "cbvike.h"
#include "debugging.h"
#include "vifunc.h"

// Register the plugin
namespace
{
    PluginRegistrant<cbVike> reg(_T("cbVike"));
};


// ----------------------------------------------------------------------------
cbVike::cbVike()
{
}
// ----------------------------------------------------------------------------
cbVike::~cbVike()
// ----------------------------------------------------------------------------
{
}

// ----------------------------------------------------------------------------
void cbVike::OnAttach()
// ----------------------------------------------------------------------------
{
    LOGIT(_T("Enter OnAttach"));
    pcbWindow = Manager::Get()->GetAppWindow();

    #if LOGGING
        wxLog::EnableLogging(true);
        pMyLog = new wxLogWindow(pcbWindow,wxT("vike"),true,false);
        wxLog::SetActiveTarget(pMyLog);
        LOGIT(_T("vike log open"));
        pMyLog->Flush();
        pMyLog->GetFrame()->Move(20,20);
    #endif

    wxVike::usableWindows.Add(_T("sciwindow"));
    wxVike::usableWindows.Add(_T("flat notebook"));

	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	pInfo->version = wxT(VERSION);

    m_bBound = FALSE;

	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<cbVike, CodeBlocksEvent>(this, &cbVike::OnEditorOpen));
	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<cbVike, CodeBlocksEvent>(this, &cbVike::OnEditorClose));
	Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<cbVike, CodeBlocksEvent>(this, &cbVike::OnAppStartupDone));
    Manager::Get()->RegisterEventSink(cbEVT_APP_START_SHUTDOWN, new cbEventFunctor<cbVike, CodeBlocksEvent>(this, &cbVike::OnAppStartShutdown));

	return;
}//OnAttach

// ----------------------------------------------------------------------------
void cbVike::OnRelease(bool appShutDown)
// ----------------------------------------------------------------------------
{
    //release all event handlers when plugin is disabled.
	pVike->DetachAll();
    delete pVike;
}//OnRelease

void cbVike::OnAppStartupDone(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    LOGIT(_T("Enter OnAppStartupDone"));
    if(!m_bBound) {
        m_bBound = TRUE;
        pVike = new wxVike();
    }

// Doesn't Concerned about these event except spilt window hack
// Currently ignore split window since a special event being added
/*    Connect( wxEVT_CREATE,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &cbVike::OnWindowCreateEvent);

    Connect( wxEVT_DESTROY,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &cbVike::OnWindowDestroyEvent);
        */

    event.Skip();
}//OnAppStartupDone


// ----------------------------------------------------------------------------
void cbVike::OnAppStartShutdown(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    LOGIT( _T("OnAppStartShutdown") );

    event.Skip();
}//OnAppStartShutdown

void cbVike::OnEditorOpen(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    LOGIT(_T("Enter OnEditorOpen"));
    if (IsAttached())
    {
        if(!m_bBound) {
            OnAppStartupDone(event);
        }
        wxWindow* thisWindow = event.GetEditor();
        wxWindow* thisEditor = thisWindow->FindWindowByName(_T("SCIwindow"),thisWindow);

        cbEditor* ed = 0;
        EditorBase* eb = event.GetEditor();

        if (eb && eb->IsBuiltinEditor())
        {
            ed = static_cast<cbEditor*>(eb);
            thisEditor = ed->GetControl();
        }

        /* Temparory hack */
        //((wxScintilla*)thisEditor)->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
        // pVike->Startup(thisEditor);

        LOGIT(_T("Attach in OnEditorOpen"));
        pVike->Attach(thisEditor, SET_START_CARET);
    }
    event.Skip();
}//OnEditorOpen

// ----------------------------------------------------------------------------
void cbVike::OnEditorClose(CodeBlocksEvent& event)
// ----------------------------------------------------------------------------
{
    LOGIT(_T("Enter OnEditorClose"));
    if (IsAttached() && m_bBound)
    {
        wxWindow* thisWindow = event.GetEditor();

        wxWindow* thisEditor = thisWindow->FindWindowByName(_T("SCIwindow"), thisWindow);

        // find editor window the Code::Blocks way
        // find the cbStyledTextCtrl wxScintilla "SCIwindow" to this EditorBase
        cbEditor* ed = 0;
        EditorBase* eb = event.GetEditor();
        if ( eb && eb->IsBuiltinEditor() )
        {
            ed = static_cast<cbEditor*>(eb);
            thisEditor = ed->GetControl();
        }
        //------------------------------------------------------------------
        // This code never executed because ed->GetControl no longer exists
        // See OnWindowDestroyEvent() which gets the window as an event.object
        //------------------------------------------------------------------
        if(thisEditor) {
            pVike->Detach(thisEditor);
            #if LOGGING
                LOGIT(_T("OnEditorClose %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
            #endif
        }
    }
    event.Skip();
}//OnEditorClose

void cbVike::OnWindowCreateEvent(wxEvent& event)
// ----------------------------------------------------------------------------
{
    LOGIT(_T("Enter OnWindowCreateEvent"));
    if ( m_bBound )
    {
        wxWindow* pWindow = (wxWindow*)(event.GetEventObject());
        cbEditor* ed = 0;
        cbStyledTextCtrl* p_cbStyledTextCtrl = 0;
        cbStyledTextCtrl* pLeftSplitWin = 0;
        cbStyledTextCtrl* pRightSplitWin = 0;
        ed  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();

        if (ed)
        {
            p_cbStyledTextCtrl = ed->GetControl();
            pLeftSplitWin = ed->GetLeftSplitViewControl();
            pRightSplitWin = ed->GetRightSplitViewControl();
            //Has this window been split?
            //**This is a temporary hack until some cbEvents are defined**
            // Can't set startup caret when using this hack, if you want to use this hack
            // you need to disable the startup caret and set vike to startup at INSERT mode
            if ( pWindow && (pRightSplitWin == 0) )
            {
                //-if (pRightSplitWin eq pWindow)
                //-{    Attach(pRightSplitWin);
                if (pWindow->GetParent() == ed)
                {
                    LOGIT( _T("OnWindowCreateEvent Attaching:%p"), pWindow );
                    AttachEditor(pWindow);
                }
            }
        }
    }//if m_bBound...


    event.Skip();
}//OnWindowCreateEvent

void cbVike::OnWindowDestroyEvent(wxEvent& event)
// ----------------------------------------------------------------------------
{
    //NB: event.GetGetEventObject() is a SCIwindow*, not and EditorBase*

    // wxEVT_DESTROY entry
    // This routine simply clears the memorized Editor pointers
    // that dont get cleared by OnEditorClose, which doesnt get
    // entered for split windows. CodeBlocks doesnt yet have events
    // when opening/closing split windows.
    if (!m_bBound){ event.Skip(); return;}

    wxWindow* thisWindow = (wxWindow*)(event.GetEventObject());
    //-Detach(pWindow); causes crash
    if ( (thisWindow))
    {
        // deleteing the EvtHandler here will crash CB
        // detach before removing the ed ptr
        DetachEditor(thisWindow, /*DeleteEvtHander*/false);

        #ifdef LOGGING
         LOGIT( _T("OnWindowDestroyEvent Removed %p"), thisWindow);
        #endif //LOGGING
    }
    event.Skip();
}//OnWindowClose

// ----------------------------------------------------------------------------
void cbVike::AttachEditor(wxWindow* pWindow)
// ----------------------------------------------------------------------------
{
    LOGIT(_T("Enter AttachEditor"));
    if (IsAttached())
    {
        wxWindow* thisEditor = pWindow->FindWindowByName(_T("SCIwindow"),pWindow);

        //skip editors that we already have
        if ( thisEditor)
        {
            LOGIT(_T("Attach in AttachEditor"));
            //Rebind keys to newly opened windows
            pVike->Attach(thisEditor, 0);
            #if LOGGING
             LOGIT(_T("cbKB:AttachEditor %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
            #endif
        }
    }
}
// ----------------------------------------------------------------------------
void cbVike::DetachEditor(wxWindow* pWindow, bool deleteEvtHandler)
// ----------------------------------------------------------------------------
{
    if (IsAttached())
     {

         wxWindow* thisWindow = pWindow;

         // Cannot use GetBuiltinActiveEditor() because the active Editor is NOT the
         // one being closed!!
         // wxWindow* thisEditor
         //  = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()->GetControl();

         //find the cbStyledTextCtrl wxScintilla window
         wxWindow* thisEditor = thisWindow->FindWindowByName(_T("SCIwindow"), thisWindow);

        if ( thisEditor)
         {
            pVike->Detach(thisEditor, deleteEvtHandler);
            #if LOGGING
                 LOGIT(_T("cbKB:DetachEditor %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
            #endif
         }//if
     }

}//DetachEditor

// ----------------------------------------------------------------------------

