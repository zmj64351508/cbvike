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
#include "cbvike.h"
#include "vifunc.h"

const int idVikeViewStatusBar = wxNewId();

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

    #ifdef LOGGING
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
    //Manager::Get()->RegisterEventSink(cbEVT_DOCK_WINDOW_VISIBILITY, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnHideStatusBar));
    Connect( wxEVT_DESTROY,  (wxObjectEventFunction)&VikePlugin::OnWindowDestroyEvent);
    Connect( idVikeViewStatusBar, wxEVT_COMMAND_MENU_SELECTED,  wxCommandEventHandler(VikePlugin::OnToggleStatusBar));
    Connect( idVikeViewStatusBar, wxEVT_UPDATE_UI,  wxUpdateUIEventHandler(VikePlugin::OnUpdateUI));

	return;
}//OnAttach

void VikePlugin::OnRelease(bool appShutDown)
{
    LOGIT(_T("enter OnRelease"));
    //release all event handlers when plugin is disabled.
	//pVike->DetachAll();
    delete pVike;
    LOGIT(_T("exit OnRelease"));
}//OnRelease

void VikePlugin::OnToggleStatusBar(wxCommandEvent& event)
{
    LOGIT(_T("Toggle status bar"));
    if (event.IsChecked()){
        pVike->ShowStatusBar();
    }else{
        pVike->HideStatusBar();
    }
    //event.Skip();
}

void VikePlugin::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    LOGIT(_T("OnUpdateUI"));
    Manager::Get()->GetAppFrame()->GetMenuBar()->Check(idVikeViewStatusBar, IsWindowReallyShown(pVike->GetStatusBar()));
}

void VikePlugin::BuildMenu(wxMenuBar* menuBar){
    //delete system editor hotkeys that conflict with vim
    delAccer(menuBar,_("&Search"),_("Find..."));
    delAccer(menuBar,_("&Search"),_("Replace..."));
    delAccer(menuBar,_("&Edit"),_("Bookmarks"),_("Toggle bookmark"));

    int idx = menuBar->FindMenu(_("&View"));
    if (idx != wxNOT_FOUND)
    {
        wxMenu* view = menuBar->GetMenu(idx);
        wxMenuItemList& items = view->GetMenuItems();
        // find the first separator and insert before it
        bool done = false;
        for (size_t i = 0; i < items.GetCount(); ++i)
        {
            if (items[i]->IsSeparator())
            {
                view->InsertCheckItem(i, idVikeViewStatusBar, _("Vike status"), _("Toggle displaying the status for Vike"));
                done = true;
                break;
            }
        }
        // not found, just append
        if ( !done )
            view->AppendCheckItem(idVikeViewStatusBar, _("Vike status"), _("Toggle displaying the status for Vike"));
    }
}

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
            LOGIT(_T("OnEditorClose %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
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
        LOGIT( _T("OnWindowDestroyEvent Removed %p"), thisWindow);
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
            LOGIT(_T("cbKB:AttachEditor %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
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
            LOGIT(_T("cbKB:DetachEditor %s %p"), thisEditor->GetLabel().c_str(), thisEditor);
         }//if
     }

}//DetachEditor


