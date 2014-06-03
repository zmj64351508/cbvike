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

	PluginInfo* pInfo = (PluginInfo*)(Manager::Get()->GetPluginManager()->GetPluginInfo(this));
	//pInfo->version = wxT(VERSION);

    m_bBound = false;
    pVike = new cbVike();

	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnEditorOpen));
	/* We detach the editor in OnWindowDestroy because the editor controller is destroied in EVT_EDITOR_CLOSE */
	/* We bind the event handler in OnAppStartupDone */
	//Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnEditorClose));
	Manager::Get()->RegisterEventSink(cbEVT_APP_STARTUP_DONE, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnAppStartupDone));
	#if defined EVT_EDITOR_SPLIT && defined EVT_EDITOR_UNSPLIT
        Manager::Get()->RegisterEventSink(cbEVT_EDITOR_SPLIT, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnEditorSplit));
        Manager::Get()->RegisterEventSink(cbEVT_EDITOR_UNSPLIT, new cbEventFunctor<VikePlugin, CodeBlocksEvent>(this, &VikePlugin::OnEditorUnsplit));
	#endif

	return;
}//OnAttach

void VikePlugin::OnRelease(bool appShutDown)
{
    LOGIT(_T("enter OnRelease"));
    //release all event handlers when plugin is disabled.
    Disconnect(wxEVT_DESTROY, wxWindowDestroyEventHandler(VikePlugin::OnWindowDestroy));
    #if not defined EVT_EDITOR_SPLIT || not defined EVT_EDITOR_UNSPLIT
        Disconnect(wxEVT_CREATE, wxWindowCreateEventHandler(VikePlugin::OnWindowCreate));
    #endif
    delete pVike;
    LOGIT(_T("exit OnRelease"));
}//OnRelease

void VikePlugin::OnAppStartupDone(CodeBlocksEvent& event)
{
    LOGIT(_T("Enter OnAppStartupDone"));
    if(!m_bBound){
        m_bBound = true;
    }
    Connect(wxEVT_DESTROY, wxWindowDestroyEventHandler(VikePlugin::OnWindowDestroy));
    //Connect(Manager::Get()->GetEditorManager()->GetNotebook()->GetId(),
            //wxEVT_SIZE, wxSizeEventHandler(VikePlugin::OnPaint));

    #if not defined EVT_EDITOR_SPLIT || not defined EVT_EDITOR_UNSPLIT
        Connect(wxEVT_CREATE, wxWindowCreateEventHandler(VikePlugin::OnWindowCreate));
    #endif
    event.Skip();
}//OnAppStartupDone

void VikePlugin::BuildMenu(wxMenuBar* menuBar)
{
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
                //view->InsertCheckItem(i, idVikeViewStatusBar, _("Vike status"), _("Toggle displaying the status for Vike"));
                done = true;
                break;
            }
        }
        // not found, just append
        if ( !done ){
            //view->AppendCheckItem(idVikeViewStatusBar, _("Vike status"), _("Toggle displaying the status for Vike"));
        }
    }
}

void VikePlugin::OnEditorOpen(CodeBlocksEvent& event)
{
    LOGIT(_T("Enter OnEditorOpen"));
    if (IsAttached())
    {
//        if(!m_bBound){
//            OnAppStartupDone(event);
//        }
        cbEditor* editor = nullptr;
        EditorBase* eb = event.GetEditor();
        /* Only work with builtin editor */
        if (eb && eb->IsBuiltinEditor())
        {
            editor = static_cast<cbEditor*>(eb);
            cbStyledTextCtrl *thisEditor = editor->GetControl();
            LOGIT(_T("Attach in OnEditorOpen"));
            pVike->Attach(thisEditor, editor);

            //Manager::Get()->GetEditorManager()
        }
    }
    event.Skip();
}//OnEditorOpen

void VikePlugin::OnEditorSplit(CodeBlocksEvent& event)
{
    LOGIT(_T("OnEditorSplit"));
    cbStyledTextCtrl *leftEditor = nullptr;
    cbStyledTextCtrl *rightEditor = nullptr;
    cbEditor *ed = nullptr;
    EditorBase *eb = event.GetEditor();
    if(eb && eb->IsBuiltinEditor()){
        ed = static_cast<cbEditor*>(eb);
        leftEditor = ed->GetLeftSplitViewControl();
        // Re-attach left
        pVike->Attach(leftEditor, ed);
        rightEditor = ed->GetRightSplitViewControl();
        // Attach right
        pVike->Attach(rightEditor, ed);
    }else{
        LOGIT(_T("Not builtin editor"));
        return;
    }
    LOGIT(_T("left is %p, right is %p"), leftEditor, rightEditor);
    event.Skip();
}

void VikePlugin::OnEditorUnsplit(CodeBlocksEvent& event)
{
    LOGIT(_T("OnEditorUnsplit"));
    cbStyledTextCtrl *leftEditor = nullptr;
    cbStyledTextCtrl *rightEditor = nullptr;
    cbEditor *ed = nullptr;
    EditorBase *eb = event.GetEditor();
    if(eb && eb->IsBuiltinEditor()){
        ed = static_cast<cbEditor*>(eb);
        /* Attach left window again.
           As the right window is destroied, just ignore it */
        leftEditor = ed->GetLeftSplitViewControl();
        pVike->Attach(leftEditor, ed);
        //pVike->DetachBrother(leftEditor);
    }else{
        LOGIT(_T("Not builtin editor"));
        return;
    }
    LOGIT(_T("left is %p, right is %p"), leftEditor, rightEditor);
    event.Skip();
}

#if not defined EVT_EDITOR_SPLIT || not defined EVT_EDITOR_UNSPLIT
    void VikePlugin::OnWindowCreate(wxWindowCreateEvent &event)
    {
        wxWindow *destroied = event.GetWindow();
        wxWindow *controller = destroied->FindWindowByName(_T("SCIwindow"), destroied);
        cbEditor *editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
        if(editor && controller){
            cbStyledTextCtrl *left = editor->GetLeftSplitViewControl();
            cbStyledTextCtrl *right = editor->GetLeftSplitViewControl();
            /* Going to be splitted */
            if(left != nullptr && left == right && left != controller){
                editor->GetSizer()->Clear();

                /* restore left caret */
                ConfigManager* mgr = Manager::Get()->GetConfigManager(_T("editor"));
                int caretStyle = mgr->ReadInt(_T("/caret/style"), wxSCI_CARETSTYLE_LINE);
                left->SetCaretStyle(caretStyle);
                pVike->Detach(left);

                m_UnattachedPair.AddPair(left, (cbStyledTextCtrl *)controller, editor);
            }
            LOGIT(_T("OnWindowCreate left %p, right %p, this is %p"), left, right, controller);
        }
        event.Skip();
    }
#endif // not

void VikePlugin::OnWindowDestroy(wxWindowDestroyEvent& event)
{
    wxWindow *destroied = event.GetWindow();
    cbStyledTextCtrl *controller = nullptr;
    if(destroied->GetName().Cmp(_T("SCIwindow")) == 0){
        controller = (cbStyledTextCtrl *)destroied;
    }

    if(controller){
        #if not defined EVT_EDITOR_SPLIT || not defined EVT_EDITOR_UNSPLIT
            UnattachedPair::Pair *found = m_UnattachedPair.FindPair(controller);
            if(found){
                cbEditor *editor = found->m_pEditor;
                cbStyledTextCtrl *curLeft = editor->GetLeftSplitViewControl();
                cbStyledTextCtrl *curRight = editor->GetRightSplitViewControl();
                LOGIT(_T("curLeft is %p, this is %p"), curLeft, controller);

                /* Unsplit and destroy this window, re-attach the brother */
                if(curLeft != nullptr && curLeft != controller){
                    cbStyledTextCtrl *brother = found->m_pLeft == controller ? found->m_pRight : found->m_pLeft;
                    pVike->Attach(brother, editor);
                }

                /* Any way the splitted window doesn't exist. current editor controller
                   should has been already attached. */
                m_UnattachedPair.DeletePair(found);
            }
        #endif
        /* Detach */
        LOGIT(_T("Detach in OnWindowDestroy %p"), destroied);
        pVike->Detach((cbStyledTextCtrl *)controller);
    }
    event.Skip();
}
