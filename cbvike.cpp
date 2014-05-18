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
IMPLEMENT_CLASS(VikeEvtBinder, wxEvtHandler)

/******************* VikeEvtHandler start ***********************/
// event table for cbVike
BEGIN_EVENT_TABLE(VikeEvtBinder, wxEvtHandler)
    // wxEVT_KEY_DOWN received some special key like ESC, BACKSPCE etc.
    EVT_KEY_DOWN(VikeEvtBinder::OnKeyDown)
    // wxEVT_CHAR recevied any ASCII characters
    EVT_CHAR(VikeEvtBinder::OnChar)
    EVT_SET_FOCUS(VikeEvtBinder::OnFocus)

#if defined( __WXMSW__	)		// supported only on Win32
#if wxUSE_HOTKEY                // enabled?
#if wxCHECK_VERSION(2, 5, 1)	// and from wxWidgets 2.5.1

    // I don't think this is needed because wxEVT_HOTKEY are generated
    // only in some special cases...
    EVT_HOTKEY(wxID_ANY, VikeEvtBinder::OnChar)
#endif
#endif
#endif
END_EVENT_TABLE()

// window names allowed for which vike may attach()
wxArrayString cbVike::usableWindows;                    //+v0.4.4

VikeEvtBinder::VikeEvtBinder(cbVike *vike, wxWindow *tg)
    : m_pVike(vike), m_pTarget(tg)
{
    m_pVikeWin = new VikeWin(tg, vike->GetStatusBar());
    m_pVikeWin->SetStartCaret((wxScintilla*)tg);
    m_pTarget->PushEventHandler(this);
}

VikeEvtBinder::~VikeEvtBinder()
{
    if ( m_pTarget ) m_pTarget->RemoveEventHandler(this);
    if ( m_pVikeWin ) delete m_pVikeWin;
}

void VikeEvtBinder::OnChar(wxKeyEvent &p)
{
    m_pVike->OnChar(m_pVikeWin, p);
}

void VikeEvtBinder::OnKeyDown(wxKeyEvent &p)
{
    m_pVike->OnKeyDown(m_pVikeWin, p);
}

void VikeEvtBinder::OnFocus(wxFocusEvent &event)
{
    LOGIT(_("on focus"));
    cbEditor *edBase = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    wxScintilla *editor = NULL;
    if(edBase){
        editor = (wxScintilla *)edBase->GetControl();
        m_pVikeWin->UpdateStatusBar();
        m_pVikeWin->UpdateCaret((wxScintilla *)editor);
    }
    event.Skip();
}
/******************* VikeEvtHandler end ***********************/

/******************* cbVike start *****************************/
void cbVike::CreateStatusBar()
{
    m_pStatusBar = new VikeStatusBar(Manager::Get()->GetAppWindow(),  wxID_ANY, wxSB_NORMAL, wxString((wxChar*)"cbVike"));
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

void cbVike::DestoryStatusBar()
{
    CodeBlocksDockEvent evt(cbEVT_REMOVE_DOCK_WINDOW);
    evt.name = _T("Vike status");
    evt.title = _("Vike status");
    evt.pWindow = m_pStatusBar;
    Manager::Get()->ProcessEvent(evt);
    delete m_pStatusBar;
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
    wxEvtHandler *h = new VikeEvtBinder(this, p);
    m_arrHandlers.Add((void*)h);
}

int cbVike::FindHandlerIdxFor(wxWindow *p) const
{
	for (int i=0; i<(int)m_arrHandlers.GetCount(); i++)
		if (((VikeEvtBinder *)m_arrHandlers.Item(i))->IsAttachedTo(p))
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
	LOGIT(wxT("idx is %d, %d left"), idx);
	VikeEvtBinder *toremove = (VikeEvtBinder*)m_arrHandlers.Item(idx);
	m_arrHandlers.RemoveAt(idx, 1);

	// the wxBinderEvtHandler will remove itself from p's event handlers
	if (deleteEvtHandler) delete toremove;
	LOGIT(wxT("%d handlers left"), m_arrHandlers.GetCount());
}

wxWindow* cbVike::FindWindowRecursively(const wxWindow* parent, const wxWindow* handle)
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

wxWindow* cbVike::winExists(wxWindow *parent)
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
        VikeEvtBinder* pHdlr = (VikeEvtBinder*)m_arrHandlers.Item(i);
        LOGIT(_T("pHdlr is %p"), pHdlr);
        pwin = pHdlr->GetTargetWnd();     //+v0.4
        LOGIT(_T("pwin is %p"), pwin);
        if  (!winExists( pwin ) )
        {   //+v0.4.9
            // tell dtor not to crash by using RemoveEventHander()
            pHdlr->SetWndInvalid(0);
            LOGIT( _T("WxKeyBinder:DetachAll:window NOT found %p <----------"), pwin); //+v0.4.6
        }
        if (pHdlr->GetTargetWnd()){
            LOGIT( _T("WxKeyBinder:DetachAll:Deleteing EvtHdlr for [%s] %p"), pwin->GetLabel().GetData(), pwin);     //+v0.4
        }
        delete pHdlr;
	 }

    LOGIT(_T("clear m_arrHandlers"));
	// and clear the array
	m_arrHandlers.Clear();

}//DetachAll

void cbVike::OnChar(VikeWin *vikeWin, wxKeyEvent &event)
{
    //begin trap the keycode
    bool skip = vikeWin->OnChar(event);
    if(skip){
        event.Skip();
    }
}

void cbVike::OnKeyDown(VikeWin *vikeWin, wxKeyEvent &event)
{
    //begin trap the keycode
    bool skip = vikeWin->OnKeyDown(event);
    if(skip){
        event.Skip();
    }
}

void cbVike::OnFocus(wxFocusEvent &event)
{

}

/******************* cbVike end *****************************/

/******************* VikeWin start **************************/
VikeWin::VikeWin(wxWindow *target, VikeStatusBar *sb)
    : m_searchCmd('/', m_highlight),
      m_generalCmd(':', m_highlight),
      m_pStatusBar(sb)
      //m_highlight(),
{
    LOGIT(_T("new VikeWin created"));
    //init some params
    m_iState = VIKE_START;
    m_iCaretPos = 0;
    m_iDupNumber = 0;
    func = ViFunc::Instance();

    cbEditor *editor = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
//    wxWindow *parent = target->GetParent();
//    cbAuiNotebook *notebook = Manager::Get()->GetEditorManager()->GetNotebook();
//    wxWindow *curPage = notebook->GetPage(notebook->GetSelection());
//    LOGIT(_T("parent is %p, curPage is %p, editor is %p, contoller is %p, notebook is %p"), parent, curPage, editor, editor->GetControl(), notebook);

    m_pBuiltinStatusBar = new VikeStatusBar(editor, wxSB_NORMAL);
    m_pBuiltinStatusBar->SetSize(100, 25);;
    editor->Freeze();
    m_pSizer = new wxBoxSizer(wxVERTICAL);
    m_pSizer->Add(editor->GetControl(), 1, wxEXPAND);
    m_pSizer->Add(m_pBuiltinStatusBar, 0, wxEXPAND);
    editor->SetSizer(m_pSizer, false);
    editor->Layout();
    editor->Thaw();

    /* the last step to change mode */
    ChangeMode(NORMAL);
}

bool VikeWin::OnChar(wxKeyEvent &event)
{
    bool skip = func->NormalKeyHandler(this, event);
    if(!skip){
        if(m_iState == VIKE_END){
            ClearKeyStatus();
            SetState(VIKE_START);
        }else if(m_iState == VIKE_SEARCH || m_iState == VIKE_COMMAND){
            ClearKeyStatus();
        }else{
            AppendKeyStatus(event.GetKeyCode());
        }
        UpdateStatusBar();
    }
    return skip;
}

bool VikeWin::OnKeyDown(wxKeyEvent &event)
{
    bool skip = func->EspecialKeyHandler(this, event);
    if(!skip){
        if(m_iState == VIKE_END){
            ClearKeyStatus();
            SetState(VIKE_START);
        }else if(m_iState == VIKE_SEARCH || m_iState == VIKE_COMMAND){
            ClearKeyStatus();
        }else{
            AppendKeyStatus(event.GetKeyCode());
        }
        UpdateStatusBar();
    }
    return skip;
}

/* set caret when startup */
void VikeWin::SetStartCaret(wxScintilla* editor) const
{
    editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
}

void VikeWin::UpdateCaret(wxScintilla *editor)
{
    if(m_iMode == INSERT){
        editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
    }else{
        editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
    }
}

void VikeWin::Finish(wxScintilla *editor)
{
    m_iDupNumber = 0;
    m_iState = VIKE_END;
    m_iCaretPos = editor->GetCurrentPos();
}

void VikeWin::ChangeMode(VikeMode new_mode)
{
    m_iMode = new_mode;
    UpdateStatusBar();
}

int VikeWin::GetUndoPos()                    { return m_iCaretPos; }
int VikeWin::GetMode() const                 { return m_iMode; }
void VikeWin::AppendKeyStatus(int key_code)  { m_arrKey.Add(key_code); }
void VikeWin::ClearKeyStatus()               { m_arrKey.Clear(); }
void VikeWin::SetState(VikeState new_state)  { m_iState = new_state; }
int VikeWin::GetState()                      { return m_iState; }
void VikeWin::ShiftAddDupNumber(int num)     { m_iDupNumber = m_iDupNumber * 10 + num; }
bool VikeWin::IsDup()                        { return m_iDupNumber != 0; }
int VikeWin::GetDupNumber()                  { return m_iDupNumber == 0 ? 1 : m_iDupNumber; }
VikeSearchCmd &VikeWin::GetSearchCmd()       { return m_searchCmd; }
VikeGeneralCmd &VikeWin::GetGeneralCmd()     { return m_generalCmd; }
VikeHighlight &VikeWin::GetHighlight()       { return m_highlight; }

void VikeWin::UpdateStatusBar()
{
    VikeStatusBar *activeBar;
    //activeBar = m_pStatusBar;
    activeBar = m_pBuiltinStatusBar;
    if(!activeBar){
        return;
    }

    if(m_iMode == NORMAL && m_iState == VIKE_SEARCH){
        activeBar->SetStatusText(m_searchCmd.GetCommandWithPrefix(), STATUS_COMMAND);
        activeBar->SetStatusText(_T(""), STATUS_KEY);
    }else if(m_iMode == NORMAL && m_iState == VIKE_COMMAND){
        activeBar->SetStatusText(m_generalCmd.GetCommandWithPrefix(), STATUS_COMMAND);
        activeBar->SetStatusText(_T(""), STATUS_KEY);
    }else{
        const wxChar *mode_txt = NULL;
        switch(m_iMode){
        case NORMAL:
            mode_txt = _T("-- NORMAL --"); break;
        case INSERT:
            mode_txt = _T("-- INSERT --"); break;
        case VISUAL:
            mode_txt = _T("-- VISIUAL --"); break;
        default:
            break;
        }
        activeBar->SetStatusText(mode_txt, STATUS_COMMAND);

        wxString keyState;
        for(int i = 0; i < m_arrKey.GetCount(); i++){
            keyState.append((wxChar)m_arrKey[i]);
        }
        if(m_arrKey.IsEmpty()){
            activeBar->SetStatusText(_T(""), STATUS_KEY);
        }else{
            activeBar->SetStatusText(keyState, STATUS_KEY);
        }
    }
}

void VikeWin::ClearCmd()
{
    m_searchCmd.Clear();
    m_generalCmd.Clear();
}
/******************* VikeWin end ****************************/
