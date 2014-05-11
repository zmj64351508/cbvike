#if defined(__GNUG__)
	#pragma implementation "cbvike.h"
#endif

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/log.h>
#endif
#include "cbstyledtextctrl.h"
#include "cbstatusbar.h"
#include "cbvike_plugin.h"
#include "debugging.h"
#include "vifunc.h"

// Register the plugin
namespace
{
    PluginRegistrant<VikePlugin> reg(_T("cbVike"));
};


VikePlugin::VikePlugin()
{
}

VikePlugin::~VikePlugin()
{
}

void VikePlugin::OnAttach()
{
    pcbWindow = Manager::Get()->GetAppWindow();

    #if LOGGING
        wxLog::EnableLogging(true);
        pMyLog = new wxLogWindow(pcbWindow,wxT("vike"),true,false);
        wxLog::SetActiveTarget(pMyLog);
        LOGIT(_T("vike log open"));
        pMyLog->Flush();
        pMyLog->GetFrame()->Move(20,20);
    #endif
    LOGIT(_T("Enter OnAttach"));

    cbVike::usableWindows.Add(_T("sciwindow"));
    cbVike::usableWindows.Add(_T("flat notebook"));

	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	pInfo->version = wxT(VERSION);

    m_bBound = false;
    pVike = new cbVike();

    LOGIT(_T("Add vike status bar"));
    pVike->CreateStatusBar();

	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnEditorOpen));
	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnEditorClose));
	Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnAppStartupDone));
    Manager::Get()->RegisterEventSink(cbEVT_APP_START_SHUTDOWN, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnAppStartShutdown));

	return;
}//OnAttach

void VikePlugin::OnRelease(bool appShutDown)
{
    //release all event handlers when plugin is disabled.
	pVike->DetachAll();
    delete pVike;
}//OnRelease

void VikePlugin::OnAppStartupDone(CodeBlocksEvent& event)
{
    LOGIT(_T("Enter OnAppStartupDone"));
    if(!m_bBound){
        m_bBound = true;
    }
// Doesn't Concerned about these event except spilt window hack
// Currently ignore split window since a special event being added
/*    Connect( wxEVT_CREATE,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &cbVike::OnWindowCreateEvent);
*/
    Connect( wxEVT_DESTROY,
        (wxObjectEventFunction) (wxEventFunction)
        (wxCommandEventFunction) &VikePlugin::OnWindowDestroyEvent);

    pVike->ShowStatusBar();
    event.Skip();
}//OnAppStartupDone

void VikePlugin::OnAppStartShutdown(CodeBlocksEvent& event)
{
    LOGIT( _T("OnAppStartShutdown") );

    event.Skip();
}//OnAppStartShutdown

void VikePlugin::OnEditorOpen(CodeBlocksEvent& event)
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
        //vike = new cbVike()
        //vike->Attach(thisEditor);
        pVike->Attach(thisEditor);
    }
    event.Skip();
}//OnEditorOpen

void VikePlugin::OnEditorClose(CodeBlocksEvent& event)
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
        LOGIT(_T("this Editor is %p"), thisEditor);
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

void VikePlugin::OnWindowCreateEvent(wxEvent& event)
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
            if ( pWindow && (pRightSplitWin == 0) )
            {
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

void VikePlugin::OnWindowDestroyEvent(wxEvent& event)
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

void VikePlugin::AttachEditor(wxWindow* pWindow)
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
            pVike->Attach(thisEditor);
            #if LOGGING
             LOGIT(_T("cbKB:AttachEditor %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
            #endif
        }
    }
}

void VikePlugin::DetachEditor(wxWindow* pWindow, bool deleteEvtHandler)
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


