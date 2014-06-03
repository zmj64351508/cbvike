// For compilers that support precompilation, includes "wx/wx.h".
#include "sdk.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if not defined(CB_PRECOMP)
#include "sdk_common.h"
#include "sdk_events.h"
#include "logmanager.h"
#endif

// includes
#include <wx/event.h>

#include "cbvike.h"
#include "vifunc.h"
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
END_EVENT_TABLE()

VikeEvtBinder::VikeEvtBinder(cbVike *vike, cbStyledTextCtrl* controller, cbEditor *editor)
    : m_pVike(vike), m_pTarget(controller), m_pEditor(editor)
{
    VikeStatusBar *bar = nullptr;
    wxArrayPtrVoid *found = vike->FindHandlerFor(editor);
    if(found->Count() == 0){
        LOGIT(_T("new status bar"));
        bar = new VikeStatusBar(editor, wxSB_NORMAL);
    }else{
        LOGIT(_T("old status bar"));
        bar = ((VikeEvtBinder *)found->Item(0))->GetVikeWin()->GetStatusBar();
    }
    m_pVikeWin = new VikeWin(controller, editor, bar);
    m_pTarget->PushEventHandler(this);
    delete found;
}

VikeEvtBinder::~VikeEvtBinder()
{
    LOGIT(_T("Deleting VikeEvtBinder"));
    if ( m_pTarget ){
        bool ret = m_pTarget->RemoveEventHandler(this);
        LOGIT(_T("remove handler ret %d"), ret);
    }

    wxArrayPtrVoid *found = m_pVike->FindHandlerFor(m_pEditor);
    if(found->Count() == 0 || (found->Count() == 1 && found->Item(0) == this)){
        LOGIT(_T("delete status bar"));
        delete m_pVikeWin->GetStatusBar();
    }
    delete found;

    if ( m_pVikeWin ){
        delete m_pVikeWin;
    }
}

void VikeEvtBinder::ReAttach(cbStyledTextCtrl *controller, cbEditor *editor)
{
    assert(m_pTarget != NULL);
    if(m_pTarget == controller){
        m_pEditor = editor;
        m_pVikeWin->LayoutStatusBar(controller, editor);
    }
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
        LOGIT(_T("window is %p, edbase is %p, editor is %p"),event.GetWindow(), edBase, editor);
        m_pVikeWin->UpdateStatusBar();
        m_pVikeWin->UpdateCaret((wxScintilla *)editor);
    }
    event.Skip();
}
/******************* VikeEvtHandler end ***********************/

/******************* cbVike start *****************************/
void cbVike::ReAttach(cbStyledTextCtrl *p, cbEditor *editor)
{
    LOGIT(_T("Re-attach to %p"), p);
    VikeEvtBinder *handler = FindHandlerFor(p);
    if(handler){
        handler->ReAttach(p, editor);
    }
}

void cbVike::Attach(cbStyledTextCtrl *p, cbEditor *editor)
{
    if(!p){
        return;
    }

    // already attached !!!
    if(IsAttachedTo(p)){
        ReAttach(p, editor);
        return;
    }

    // do not attach ourselves to temporary windows !!
    if (p->GetExtraStyle() & wxWS_EX_TRANSIENT)
        return;

    // create a new event handler for this window
    wxEvtHandler *h = new VikeEvtBinder(this, p, editor);
    m_arrHandlers.Add((void*)h);
    LOGIT(_T("Add wxEvtHandler %p for controller %p in editor %p"), h, p, editor);
}

int cbVike::FindHandlerIdxFor(cbStyledTextCtrl *p) const
{
	for(int i=0; i<(int)m_arrHandlers.GetCount(); i++){
		if (((VikeEvtBinder *)m_arrHandlers.Item(i))->IsAttachedTo(p)){
			return i;
		}
	}
	return wxNOT_FOUND;
}

wxArrayPtrVoid *cbVike::FindHandlerFor(cbEditor *p) const
{
    wxArrayPtrVoid *result = new wxArrayPtrVoid();
    for(int i = 0; i < m_arrHandlers.GetCount(); i++){
        if(((VikeEvtBinder*)m_arrHandlers[i])->IsAttachedTo(p)){
            result->Add(m_arrHandlers[i]);
        }
    }
    return result;
}

void cbVike::Detach(cbEditor *editor)
{
    if(!editor){
        return;
    }

    int idx, i;
    /* remove all the found handlers */
    /* NOTE: I don't know how to remove mutiple items in one iteration */
    for(int j = 0; j < 2; j++){
        for(i = 0; i < m_arrHandlers.GetCount(); i++){
            if(((VikeEvtBinder *)m_arrHandlers[i])->IsAttachedTo(editor)){
                break;
            }
        }
        delete (VikeEvtBinder *)m_arrHandlers[i];
        m_arrHandlers.RemoveAt(i);
    }

    LOGIT(wxT("%d handlers left"), m_arrHandlers.GetCount());
}

VikeEvtBinder *cbVike::FindBrother(cbStyledTextCtrl *controller)
{
    int idx = FindHandlerIdxFor(controller);
    VikeEvtBinder *current, *brother = nullptr;
    if(idx != wxNOT_FOUND){
        cbEditor *editor = ((VikeEvtBinder *)m_arrHandlers[idx])->GetEditor();
        for(int i = 0; i < m_arrHandlers.GetCount(); i++){
            current = (VikeEvtBinder *)m_arrHandlers[i];
            /* find brother for controller*/
            if(current->GetEditor() == editor && current->GetController() != controller){
                brother = (VikeEvtBinder *)m_arrHandlers[i];
                break;
            }
        }
    }
    return brother;
}

void cbVike::DetachBrother(cbStyledTextCtrl *controller)
{
    VikeEvtBinder *toRemove = FindBrother(controller);
    if(toRemove){
        m_arrHandlers.Remove((void *)toRemove);
        delete toRemove;
    }
	LOGIT(wxT("%d handlers left"), m_arrHandlers.GetCount());
}

void cbVike::Detach(cbStyledTextCtrl *p, bool deleteEvtHandler)
{
	if (!p || !IsAttachedTo(p))
		return;		// this is not attached...

    LOGIT(wxT("wxKeyBinder::Detach - detaching from [%s] %p"), p->GetName().c_str(),p);

	// remove the event handler
	int idx = FindHandlerIdxFor(p);
	LOGIT(wxT("idx is %d"), idx);
	VikeEvtBinder *toremove = (VikeEvtBinder*)m_arrHandlers.Item(idx);
	m_arrHandlers.RemoveAt(idx, 1);

	// the wxBinderEvtHandler will remove itself from p's event handlers
	if (deleteEvtHandler) delete toremove;
	LOGIT(wxT("%d handlers left"), m_arrHandlers.GetCount());
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
VikeWin::VikeWin(cbStyledTextCtrl* target, cbEditor *editor, VikeStatusBar *bar)
    : m_searchCmd('/', m_highlight),
      m_generalCmd(':', m_highlight),
      m_pBuiltinStatusBar(bar)
{
    LOGIT(_T("new VikeWin created"));
    //init some params
    PushState(VIKE_START);
    m_iCaretPos = 0;
    func = ViFunc::Instance();

    wxWindow *parent = target->GetParent();

    //parent->Freeze();
    /* Kidnap the sizer of editor */
    LayoutStatusBar(target, editor);
    //parent->Thaw();
    LOGIT(_T("we have resize the window for status bar"));

    /* the last step to change mode */
    ChangeMode(NORMAL, target);
    //UpdateCaret(target);
}

void VikeWin::LayoutStatusBar(cbStyledTextCtrl *controller, cbEditor* editor)
{
    wxWindow *parent = controller->GetParent();
    /* Rebuild status bar */
    wxSizer *sizer = editor->GetSizer();
    sizer->Clear();
    if(parent == editor){
        sizer->Add(controller, 1, wxEXPAND);
    }else{
        sizer->Add(parent, 1, wxEXPAND);
    }
    sizer->Add(m_pBuiltinStatusBar, 0, wxEXPAND);
    editor->Layout();
}

/* Only call with detach */
VikeWin::~VikeWin()
{
    LOGIT(_T("Deleting VikeWin"));
}

void VikeWin::GeneralHandler(int keyCode, bool skip)
{
    int state = GetState();

    if(state == VIKE_INVALID){
        ResetState();
        return;
    }

    /* Clear status on VIKE_END */
    if(state == VIKE_END){
        ClearKeyStatus();
        ResetState();
    }

    /* Not skip, update key status and status bar */
    if(!skip){
        if(state == VIKE_SEARCH || state == VIKE_COMMAND){
            ClearKeyStatus();
        }else if(state != VIKE_END){
            AppendKeyStatus(keyCode);
        }
        UpdateStatusBar();
    }
}

bool VikeWin::OnChar(wxKeyEvent &event)
{
    bool skip = func->NormalKeyHandler(this, event);
    GeneralHandler(event.GetKeyCode(), skip);
    return skip;
}

bool VikeWin::OnKeyDown(wxKeyEvent &event)
{
    bool skip = func->EspecialKeyHandler(this, event);
    GeneralHandler(event.GetKeyCode(), skip);
    return skip;
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
    ClearDupNumber();
    SetState(VIKE_END);
    m_iCaretPos = editor->GetCurrentPos();
}

void VikeWin::ChangeMode(VikeMode new_mode, wxScintilla *editor)
{
    m_iMode = new_mode;
    UpdateCaret(editor);
    UpdateStatusBar();
}

int VikeWin::GetUndoPos()                    { return m_iCaretPos; }
int VikeWin::GetMode() const                 { return m_iMode; }
void VikeWin::AppendKeyStatus(int key_code)  { m_arrKey.Add(key_code); }
void VikeWin::ClearKeyStatus()               { m_arrKey.Clear(); }

VikeSearchCmd &VikeWin::GetSearchCmd()       { return m_searchCmd; }
VikeGeneralCmd &VikeWin::GetGeneralCmd()     { return m_generalCmd; }
VikeHighlight &VikeWin::GetHighlight()       { return m_highlight; }

void VikeWin::SetState(VikeStateEnum newState)
{
    int count = m_state.Count();
    assert(count > 0);
    m_state[count - 1]->state = newState;
}

void VikeWin::PushState(VikeStateEnum newState)
{
    m_state.push_back(new VikeState(newState));
}

void VikeWin::PopState()
{
    if(m_state.Count() > 1){
        delete m_state.Last();
        m_state.pop_back();
    }
}

VikeStateEnum VikeWin::GetState()
{
    return m_state.Last()->state;
}

int VikeWin::GetStateCount()
{
    return m_state.Count();
}

void VikeWin::ResetState()
{
    while(m_state.Count() > 1){
        PopState();
    }
    SetState(VIKE_START);
    ClearDupNumber();
}

/* Shift the duplicate number and add current num to it */
void VikeWin::ShiftAddDupNumber(int num)
{
    VikeState *state = m_state.Last();
    state->dupNum = state->dupNum * 10 + num;
}

/* whether the duplicate number is typed */
bool VikeWin::IsDup()
{
    VikeState *state;
    return m_state.Last()->dupNum != 0;
//    for(int i = 0; i < m_state.Count(); i++){
//        VikeState *state = m_state.Last();
//        if(state->dupNum != 0){
//            return true;
//        }
//    }
//    return false;
}

/* Get the duplicate number */
int VikeWin::GetDupNumber()
{
    VikeState *state = m_state.Last();
    return state->dupNum == 0 ? 1 : state->dupNum;
}

void VikeWin::ClearDupNumber()
{
    VikeState *state = m_state.Last();
    state->dupNum = 0;
}

void VikeWin::UpdateStatusBar()
{
    VikeStatusBar *activeBar;
    activeBar = m_pBuiltinStatusBar;
    if(!activeBar){
        return;
    }

    int state = GetState();
    if(m_iMode == NORMAL && state == VIKE_SEARCH){
        activeBar->SetStatusText(m_searchCmd.GetCommandWithPrefix(), STATUS_COMMAND);
        activeBar->SetStatusText(_T(""), STATUS_KEY);
    }else if(m_iMode == NORMAL && state == VIKE_COMMAND){
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
