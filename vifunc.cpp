#ifdef __GNUG__
#pragma implementation "vifunc.h"
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
#include <wx/utils.h>

#include "cbstyledtextctrl.h"
#include "vifunc.h"
#include "debugging.h"
ViFunc ViFunc::gViFunc;

VikeWin::VikeWin(wxStatusBar *sb)
    : m_pStatusBar(sb)
{
    LOGIT(_T("new VikeWin created"));
    //init some params
    m_iState = VIKE_START;
    m_iCaretPos = 0;
    m_iDupNumber = 0;
    ChangeMode(NORMAL);
    func = ViFunc::Instance();
}

bool VikeWin::NormalKeyHandler(wxKeyEvent &event)
{
    bool skip = func->NormalKeyHandler(this, event);
    if(!skip){
        if(m_iState == VIKE_END){
            ClearKeyStatus();
            SetState(VIKE_START);
        }else{
            AppendKeyStatus(event.GetKeyCode());
        }
        UpdateStatusBar();
    }
    return skip;
}

bool VikeWin::EspecialKeyHandler(wxKeyEvent &event)
{
    bool skip = func->EspecialKeyHandler(this, event);
    if(!skip){
        if(m_iState == VIKE_END){
            ClearKeyStatus();
            SetState(VIKE_START);
        }else{
            AppendKeyStatus(event.GetKeyCode());
        }
        UpdateStatusBar();
    }
    return skip;
}

ViFunc::ViFunc()
{
    m_bLineCuted = false;
}

bool ViFunc::InsertModeSp(VikeWin *m_pVikeWin, int keyCode, wxScintilla *editor)
{
    bool skip = TRUE;
    if(keyCode == WXK_ESCAPE){
        ViFunc::i_esc(m_pVikeWin, editor);
        skip = FALSE;
    }
    return skip;
}

bool ViFunc::NormalModeSp(VikeWin *m_pVikeWin, int keyCode, int m_iModifier, wxScintilla *editor)
{
    bool skip = TRUE;
    if(keyCode == WXK_ESCAPE){
        ViFunc::n_esc(m_pVikeWin, editor);
        skip = FALSE;
    }else if(keyCode == WXK_BACK){
        ViFunc::n_backspace(m_pVikeWin, editor);
        skip = FALSE;
    }else if(m_iModifier == wxMOD_CONTROL){
        LOGIT(_T("ctrl"));
        /* keyCode doesnot distinguish 'a' and 'A', and they actually
           are the same with CTRL */
        switch(keyCode){
        case 'R':
            LOGIT(_T("ctrl R"));
            n_ctrl_r(m_pVikeWin, editor); return skip;
            skip = FALSE;
            break;
        default:
            skip = FALSE;
            m_pVikeWin->Finish(editor);
            break;
        }
    }
    return skip;
}

bool ViFunc::NormalMode(VikeWin *m_pVikeWin, int keyCode, int m_iModifier, wxScintilla *editor)
{
    bool skip = FALSE;

    // check some special state first
    switch(m_pVikeWin->GetState()){
    case VIKE_REPLACE:
        ViFunc::n_r_any(m_pVikeWin, keyCode, editor); return skip;
    case VIKE_FIND_FORWARD:
        ViFunc::n_f_any(m_pVikeWin, keyCode, editor); return skip;
    case VIKE_FIND_BACKWORD:
        ViFunc::n_F_any(m_pVikeWin, keyCode, editor); return skip;
    default:
        break;
    }

    switch(keyCode){
    case '0':
        ViFunc::n_0(m_pVikeWin, editor); break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        ViFunc::n_number(m_pVikeWin, keyCode - '0', editor); break;
    case '^':
        ViFunc::n_circumflex(m_pVikeWin, editor); break;
    case '$':
        ViFunc::n_dollar(m_pVikeWin, editor); break;
    case 'a':
        ViFunc::n_a(m_pVikeWin, editor); break;
    case 'A':
        ViFunc::n_A(m_pVikeWin, editor); break;
    case 'b':
        ViFunc::n_b(m_pVikeWin, editor); break;
    // c cc
    case 'c':
        ViFunc::n_c(m_pVikeWin, editor); break;
    // d dd
    case 'd':
        ViFunc::n_d(m_pVikeWin, editor); break;
    case 'D':
        ViFunc::n_D(m_pVikeWin, editor); break;
    case 'f':
        ViFunc::n_f(m_pVikeWin, editor); break;
    case 'F':
        ViFunc::n_F(m_pVikeWin, editor); break;
    case 'g':
        ViFunc::n_g(m_pVikeWin, editor); break;
    case 'G':
        ViFunc::n_G(m_pVikeWin, editor); break;
    case 'i':
        ViFunc::n_i(m_pVikeWin, editor); break;
    case 'I':
        ViFunc::n_I(m_pVikeWin, editor); break;
    case 'h':
        ViFunc::n_h(m_pVikeWin, editor); break;
    case 'j':
        ViFunc::n_j(m_pVikeWin, editor); break;
    case 'k':
        ViFunc::n_k(m_pVikeWin, editor); break;
    case 'l':
        ViFunc::n_l(m_pVikeWin, editor); break;
    case 'o':
        ViFunc::n_o(m_pVikeWin, editor); break;
    case 'O':
        ViFunc::n_O(m_pVikeWin, editor); break;
    case 'p':
        ViFunc::n_p(m_pVikeWin, editor); break;
    case 'P':
        ViFunc::n_P(m_pVikeWin, editor); break;
    case 'r':
        ViFunc::n_r(m_pVikeWin, editor); break;
    case 'u':
        ViFunc::n_u(m_pVikeWin, editor); break;
    // w dw cw yw
    case 'w':
        ViFunc::n_w(m_pVikeWin, editor); break;
    case 'x':
        ViFunc::n_x(m_pVikeWin, editor); break;
    case 'X':
        ViFunc::n_X(m_pVikeWin, editor); break;
    // y yy
    case 'y':
        ViFunc::n_y(m_pVikeWin, editor); break;
    default:
        m_pVikeWin->Finish(editor);
        break;
    }

    return skip;
}

bool ViFunc::EspecialKeyHandler(VikeWin *curVike, wxKeyEvent &event)
{
    int keyCode = event.GetKeyCode();
    wxScintilla *editor = (wxScintilla *)event.GetEventObject();
    bool skip = TRUE;
    LOGIT(wxT("vifunc::especial_key_press - customer key event,keycode: [%d]"), keyCode);
    switch(curVike->GetMode()){
    case INSERT:
        skip = InsertModeSp(curVike, keyCode, editor);
        break;
    case NORMAL:
        skip = NormalModeSp(curVike, keyCode, event.GetModifiers(), editor);
        break;
    default:
        break;
    }
    return skip;
}

//if return FALSE,not process here,leave next.
//
bool ViFunc::NormalKeyHandler(VikeWin *curVike, wxKeyEvent &event)
{
    int keyCode = event.GetKeyCode();
    wxScintilla *editor = (wxScintilla *)event.GetEventObject();

    LOGIT(wxT("vifunc::execute - customer key event,keycode: [%d]"), keyCode);
    LOGIT(wxT("current mode is %d"), curVike->GetMode());
    LOGIT(wxT("current state is %d"), curVike->GetState());

    bool skip = FALSE;
    switch(curVike->GetMode()){
    case NORMAL:
        skip = NormalMode(curVike, keyCode, event.GetModifiers(), editor);
        break;
    case VISUAL:
    case INSERT:
    default:
        skip = TRUE;
        break;
    }

    return skip;
}

//insert mode
void ViFunc::i_esc(VikeWin *m_pVike, wxScintilla* editor)
{
    m_pVike->ChangeMode(NORMAL);
    if(editor->GetCurrentPos() != editor->PositionFromLine(editor->GetCurrentLine()))
        editor->CharLeft();
    editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
    m_pVike->Finish(editor);
}

//normal mode
void ViFunc::n_esc(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
    m_pVike->Finish(editor);
}
void ViFunc::n_backspace(VikeWin* m_pVike, wxScintilla* editor)
{
    int num = m_pVike->GetDupNumber();
    for(int i = 0; i < num; i++){
        editor->CharLeft();
    }
    m_pVike->Finish(editor);
}
//void ViFunc::n_delete(VikeWin *vike, wxScintilla* editor)
//{
//    if(editor->GetSelectionStart() == editor->GetSelectionEnd()){
//        editor->CharRight();
//    }
//    editor->DeleteBack();
//    vike->Finish(editor);
//}
void ViFunc::n_number(VikeWin *m_pVike, int number, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        m_pVike->ShiftAddDupNumber(number);
        LOGIT(_T("number now is %d"), m_pVike->GetDupNumber());
        break;
    default:
        m_pVike->SetState(VIKE_START);
        break;
    }
}
void ViFunc::n_tom(VikeWin *m_pVike, wxScintilla* editor)
{
}
void ViFunc::n_toma(VikeWin *m_pVike, wxScintilla* editor)
{
}
void ViFunc::n_m(VikeWin *m_pVike, wxScintilla* editor)
{
}

#define SINGLE_COMMAND(m_pVike, editor, command) \
do{\
    switch(m_pVike->GetState()){\
    case VIKE_START:\
        command(m_pVike, editor);\
    default:\
        m_pVike->SetState(VIKE_END);\
    }\
}while(0)
void ViFunc::n_circumflex(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_circumflex_end);
}
void ViFunc::n_dollar(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        n_dollar_end(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    case VIKE_CHANGE:
        n_cdollar_end(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    case VIKE_DELETE:
        n_ddollar_end(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    case VIKE_YANK:
        n_ydollar_end(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    default:
        m_pVike->SetState(VIKE_END);
    }
}
void ViFunc::n_ctrl_r(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_ctrl_r_end);
}
void ViFunc::n_a(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_a_end);
}
void ViFunc::n_b(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_b_end);
}
void ViFunc::n_A(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_A_end);
}
void ViFunc::n_D(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_ddollar_end);
}
void ViFunc::n_h(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_h_end);
}
void ViFunc::n_j(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_j_end);
}
void ViFunc::n_k(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_k_end);
}
void ViFunc::n_l(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_l_end);
}
void ViFunc::n_i(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_i_end);
}
void ViFunc::n_I(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_I_end);
}
void ViFunc::n_o(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_o_end);
}
void ViFunc::n_O(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_O_end);
}
void ViFunc::n_u(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_u_end);
}
void ViFunc::n_x(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_x_end);
}
void ViFunc::n_X(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_X_end);
}
void ViFunc::n_p(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_p_end);
}
void ViFunc::n_P(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_P_end);
}
void ViFunc::n_G(VikeWin *m_pVike, wxScintilla* editor)
{
    SINGLE_COMMAND(m_pVike, editor, n_G_end);
}
void ViFunc::n_a_end(VikeWin *m_pVike, wxScintilla* editor)
{
    if(editor->GetCurrentPos() != editor->GetLineEndPosition(editor->GetCurrentLine())){
        editor->CharRight();
    }
    n_i_end(m_pVike, editor);
    m_pVike->Finish(editor);
}
void ViFunc::n_A_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->LineEnd();
    n_i_end(m_pVike, editor);
    m_pVike->Finish(editor);
}
void ViFunc::n_h_end(VikeWin *m_pVike, wxScintilla* editor)
{
    if(editor->GetCurrentPos() != editor->PositionFromLine(editor->GetCurrentLine()))
        editor->CharLeft();
    m_pVike->Finish(editor);
}
void ViFunc::n_j_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->LineDown();
    m_pVike->Finish(editor);
}
void ViFunc::n_k_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->LineUp();
    m_pVike->Finish(editor);
}
void ViFunc::n_i_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->BeginUndoAction();
    m_pVike->ChangeMode(INSERT);
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
    editor->EndUndoAction();
    m_pVike->Finish(editor);
}
void ViFunc::n_I_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->VCHome();
    m_pVike->ChangeMode(INSERT);
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
    m_pVike->Finish(editor);
}
void ViFunc::n_l_end(VikeWin *m_pVike, wxScintilla* editor)
{
    if(editor->GetCurrentPos() != editor->GetLineEndPosition(editor->GetCurrentLine())){
        editor->CharRight();
    }
    m_pVike->Finish(editor);
}
void ViFunc::n_0(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        if(m_pVike->IsDup()){
            ViFunc::n_number(m_pVike, 0, editor);
        }else{
            ViFunc::n_0_end(m_pVike, editor);
        }
        break;
    default:
        m_pVike->SetState(VIKE_END);
    }
}
void ViFunc::n_0_end(VikeWin *m_pVike, wxScintilla* editor)
{
    if(m_pVike->IsDup()) {
        m_pVike->ShiftAddDupNumber(0);
    } else {
        editor->Home();
    }
}
void ViFunc::n_circumflex_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->VCHome();
    m_pVike->Finish(editor);
}
void ViFunc::n_dollar_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->LineEnd();
    m_pVike->Finish(editor);
}
void ViFunc::n_G_end(VikeWin *m_pVike, wxScintilla* editor)
{
    if(m_pVike->IsDup()) {
        int num = m_pVike->GetDupNumber();
        editor->GotoLine(num - 1);
    } else {
        editor->DocumentEnd();
    }
    m_pVike->Finish(editor);
}
void ViFunc::n_bktab(VikeWin *m_pVike, wxScintilla* editor)
{
}
void ViFunc::n_bktabbktab(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->BackTab();
    m_pVike->Finish(editor);
}
void ViFunc::n_tab(VikeWin *m_pVike, wxScintilla* editor)
{
}
void ViFunc::n_tabtab(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->Tab();
    m_pVike->Finish(editor);
}
void ViFunc::n_search(VikeWin *m_pVike, wxScintilla* editor)
{
    m_pVike->Finish(editor);
}
void ViFunc::n_ctrl_n(VikeWin *m_pVike, wxScintilla* editor)
{
    m_pVike->Finish(editor);
}
void ViFunc::n_ctrl_p(VikeWin *m_pVike, wxScintilla* editor)
{
    m_pVike->Finish(editor);
}
void ViFunc::n_o_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->BeginUndoAction();
    editor->LineEnd();
    editor->NewLine();
    n_i_end(m_pVike, editor);
    editor->EndUndoAction();
    m_pVike->Finish(editor);
}
void ViFunc::n_O_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->BeginUndoAction();
    editor->VCHome();
    editor->NewLine();
    editor->LineUp();
    n_i_end(m_pVike, editor);
    editor->EndUndoAction();
    m_pVike->Finish(editor);
}
void ViFunc::n_g(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        m_pVike->SetState(VIKE_EXTRA);
        break;
    case VIKE_EXTRA:
        ViFunc::n_gg(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    default:
        m_pVike->SetState(VIKE_END);
        break;
    }
}
void ViFunc::n_gg(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->DocumentStart();
    m_pVike->Finish(editor);
}
void ViFunc::n_y(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        m_pVike->SetState(VIKE_YANK);
        break;
    case VIKE_YANK:
        ViFunc::n_yy(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    default:
        m_pVike->SetState(VIKE_END);
    }
}
void ViFunc::n_ydollar_end(VikeWin *m_pVike, wxScintilla *editor)
{
    int num = m_pVike->GetDupNumber();
    int curPos = editor->GetCurrentPos();

    editor->SetSelectionStart(curPos);
    int start_line = editor->GetCurrentLine();
    int end_pos = editor->GetLineEndPosition(start_line + num - 1);
    editor->SetSelectionEnd(end_pos);
    editor->Copy();
    editor->SetEmptySelection(curPos);

    m_bLineCuted = false;
    m_pVike->Finish(editor);
}
void ViFunc::n_yy(VikeWin *m_pVike, wxScintilla* editor)
{
    int num = m_pVike->GetDupNumber();
    int curPos = editor->GetCurrentPos();

    int start_line = editor->GetCurrentLine();
    editor->SetSelectionStart(editor->PositionFromLine(start_line));
    int end_pos = editor->GetLineEndPosition(start_line + num - 1);
    editor->SetSelectionEnd(end_pos);
    editor->Copy();
    editor->SetEmptySelection(curPos);

    m_bLineCuted = true;
    m_pVike->Finish(editor);
}
void ViFunc::n_yw(VikeWin *m_pVike, wxScintilla* editor)
{
    m_bLineCuted = false;
    int num = m_pVike->GetDupNumber();
    num = num > 0 ? num : 1;
    int curPos = editor->GetCurrentPos();
    for(;num > 0; num--){
        editor->WordRight();
    }
    editor->SetSelectionStart(curPos);
    editor->Copy();
    editor->GotoPos(curPos);
    m_pVike->Finish(editor);
}
void ViFunc::n_p_end(VikeWin *m_pVike, wxScintilla* editor)
{
    int line;
    int curPos;

    editor->BeginUndoAction();
    if(m_bLineCuted){
        editor->LineEnd();
        editor->NewLine();
        line = editor->GetCurrentLine();
        //editor->DelLineLeft();
        editor->SetLineIndentation(line, 0);
    }else{
        n_l(m_pVike, editor);
        curPos = editor->GetCurrentPos();


    }

    editor->Paste();

    if(m_bLineCuted){
        editor->GotoLine(line);
        editor->VCHome();
    }else{
        editor->GotoPos(curPos);
    }

    editor->EndUndoAction();
    m_pVike->Finish(editor);
}
void ViFunc::n_P_end(VikeWin *m_pVike, wxScintilla* editor)
{
    int line;
    int curPos;

    editor->BeginUndoAction();
    if(m_bLineCuted){
        editor->LineUp();
        editor->LineEnd();
        editor->NewLine();
        line = editor->GetCurrentLine();
        //editor->DelLineLeft();
        editor->SetLineIndentation(line, 0);
    }else{
        curPos = editor->GetCurrentPos();
    }

    editor->Paste();

    if(m_bLineCuted){
        editor->GotoLine(line);
        editor->VCHome();
    }else{
        editor->GotoPos(curPos);
    }

    editor->EndUndoAction();
    m_pVike->Finish(editor);
}
void ViFunc::n_ctrl_f(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->PageDown();
    m_pVike->Finish(editor);
}

void ViFunc::n_v(VikeWin *m_pVike, wxScintilla* editor)
{
    m_pVike->ChangeMode(VISUAL);
    editor->SetCaretForeground(wxColour(0xFF, 0x00, 0x00));
    m_pVike->Finish(editor);
}
void ViFunc::n_ctrl_b(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->PageUp();
    m_pVike->Finish(editor);
}
void ViFunc::n_b_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->WordLeft();
    m_pVike->Finish(editor);
}
void ViFunc::n_w_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->WordRight();
    m_pVike->Finish(editor);
}
void ViFunc::n_ddollar_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->BeginUndoAction();
    int num = m_pVike->GetDupNumber();

    int curPos = editor->GetCurrentPos();
    int curLine = editor->GetCurrentLine();
    int lineEnd = editor->GetLineEndPosition(curLine + num - 1);
    editor->SetSelectionStart(curPos);
    editor->SetSelectionEnd(lineEnd);
    editor->Cut();
    editor->EndUndoAction();

    m_bLineCuted = false;
    m_pVike->Finish(editor);
}
void ViFunc::n_dw(VikeWin *m_pVike, wxScintilla* editor)
{
    int num = m_pVike->GetDupNumber();
    num = num > 0 ? num : 1;

    int curPos = editor->GetCurrentPos();
    for(;num > 0; num--){
        editor->WordRight();
    }
    editor->SetSelectionStart(curPos);
    editor->Cut();
    editor->SetCurrentPos(curPos);
    m_bLineCuted = false;

    m_pVike->Finish(editor);
}
void ViFunc::n_dd(VikeWin *m_pVike, wxScintilla* editor)
{
    int num = m_pVike->GetDupNumber();
    num = num > 0 ? num : 1;

    editor->Home();
    editor->SetSelectionStart(editor->GetCurrentPos());
    int start_line = editor->GetCurrentLine();
    int end_pos = editor->GetLineEndPosition(start_line + num - 1);
    editor->SetSelectionEnd(end_pos);
    editor->Cut();
    editor->DeleteBack();
    editor->CharRight();

    m_bLineCuted = true;
    m_pVike->Finish(editor);
}
void ViFunc::n_d(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        m_pVike->SetState(VIKE_DELETE);
        break;
    case VIKE_DELETE:
        ViFunc::n_dd(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    default:
        m_pVike->SetState(VIKE_END);
        break;
    }
}
void ViFunc::n_c(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        m_pVike->SetState(VIKE_CHANGE);
        break;
    case VIKE_CHANGE:
        ViFunc::n_cc(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    default:
        m_pVike->SetState(VIKE_END);
        break;
    }
}
void ViFunc::n_w(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        ViFunc::n_w_end(m_pVike, editor);
        break;
    case VIKE_CHANGE:
        ViFunc::n_cw(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    case VIKE_DELETE:
        ViFunc::n_dw(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    case VIKE_YANK:
        ViFunc::n_yw(m_pVike, editor);
        m_pVike->SetState(VIKE_END);
        break;
    default:
        m_pVike->SetState(VIKE_END);
        break;
    }
}
void ViFunc::n_cdollar_end(VikeWin *m_pVike, wxScintilla* editor)
{
    n_ddollar_end(m_pVike, editor);
    n_i_end(m_pVike, editor);
    m_pVike->Finish(editor);
}
void ViFunc::n_cw(VikeWin *m_pVike, wxScintilla* editor)
{
    n_dw(m_pVike, editor);
    n_i_end(m_pVike, editor);
    m_pVike->Finish(editor);
}
void ViFunc::n_cc(VikeWin *m_pVike, wxScintilla* editor)
{
    n_dd(m_pVike, editor);
    n_O(m_pVike, editor);
    m_pVike->Finish(editor);
}
void ViFunc::n_r(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        m_pVike->SetState(VIKE_REPLACE);
        break;
    default:
        m_pVike->SetState(VIKE_END);
        break;
    }
}
void ViFunc::n_r_any(VikeWin *m_pVike, int keyCode, wxScintilla* editor)
{
    LOGIT(_T("r_any key %c\n"), keyCode);
    editor->CharRightExtend();
    wxString replace((wxChar)keyCode);
    editor->ReplaceSelection(replace);
    editor->CharLeft();
    delete replace;
    m_pVike->Finish(editor);
}
void ViFunc::n_f(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        m_pVike->SetState(VIKE_FIND_FORWARD);
        break;
    default:
        m_pVike->SetState(VIKE_END);
        break;
    }
}
void ViFunc::n_f_any(VikeWin *m_pVike, int keyCode, wxScintilla* editor)
{
    LOGIT(_T("f_any key %c\n"), keyCode);
    wxString toFind((wxChar)keyCode);

    int curPos = editor->GetCurrentPos() + 1;
    int endPos = editor->GetLineEndPosition(editor->GetCurrentLine());
    int lengthFound, findPos = -1;
    /* no need to find */
    if(curPos >= endPos){
        goto out;
    }
    findPos = editor->FindText(curPos, endPos, toFind, wxSCI_FIND_MATCHCASE, &lengthFound);
    if(findPos > 0){
        editor->GotoPos(findPos);
    }
    delete toFind;
out:
    m_pVike->Finish(editor);
}
void ViFunc::n_F(VikeWin *m_pVike, wxScintilla* editor)
{
    switch(m_pVike->GetState()){
    case VIKE_START:
        m_pVike->SetState(VIKE_FIND_BACKWORD);
        break;
    default:
        m_pVike->SetState(VIKE_END);
        break;
    }
}
void ViFunc::n_F_any(VikeWin *m_pVike, int keyCode, wxScintilla* editor)
{
    LOGIT(_T("F_any key %c\n"), keyCode);
    wxString toFind((wxChar)keyCode);

    int curPos = editor->GetCurrentPos();
    int startPos = editor->PositionFromLine(editor->GetCurrentLine());
    /* find the previous one */
    int lengthFound, findPos = -1;
    while(1){
        startPos = editor->FindText(startPos, curPos, *toFind, wxSCI_FIND_MATCHCASE, &lengthFound);
        if(startPos < 0){
            break;
        }
        findPos = startPos++;
    }
    if(findPos > 0){
        editor->GotoPos(findPos);
    }
    m_pVike->Finish(editor);
}
void ViFunc::n_u_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->Undo();
    //editor->SetCurrentPos(m_pVike->GetUndoPos());
    m_pVike->Finish(editor);
}
void ViFunc::n_sf_5(VikeWin *m_pVike, wxScintilla* editor)
{
    int curpos = editor->GetCurrentPos();
    int bracepos = editor->BraceMatch(curpos);
    if(bracepos != -1)
    {
        editor->SetCurrentPos(bracepos);
        curpos < bracepos ? editor->SetSelectionStart(editor->GetSelectionEnd()) : editor->SetSelectionEnd(editor->GetSelectionStart());
    }
    m_pVike->Finish(editor);
}
void ViFunc::n_x_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->CharRight();
    editor->DeleteBack();
    m_pVike->Finish(editor);
}
void ViFunc::n_X_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->DeleteBack();
    m_pVike->Finish(editor);
}
void ViFunc::n_ctrl_r_end(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->Redo();
    m_pVike->Finish(editor);
}
//////////////////////////////////////////////////////////////////////

//visual mode
void ViFunc::v_esc(VikeWin *m_pVike, wxScintilla* editor)
{
    m_pVike->ChangeMode(NORMAL);
    //restore select extend
    editor->SetCurrentPos(editor->GetSelectionStart());
    editor->SetCaretForeground(wxColour(0x00, 0x00, 0x00));
    m_pVike->Finish(editor);
}
void ViFunc::v_h(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->CharLeftExtend();
    m_pVike->Finish(editor);
}
void ViFunc::v_j(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->LineDownExtend();
    m_pVike->Finish(editor);
}
void ViFunc::v_k(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->LineUpExtend();
    m_pVike->Finish(editor);
}
void ViFunc::v_l(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->CharRightExtend();
    m_pVike->Finish(editor);
}
void ViFunc::v_e(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->WordRightExtend();
    m_pVike->Finish(editor);
}
void ViFunc::v_b(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->WordLeftExtend();
    m_pVike->Finish(editor);
}
void ViFunc::v_sf_4(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->LineEndExtend();
    m_pVike->Finish(editor);
}
void ViFunc::v_sf_6(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->VCHomeExtend();
    m_pVike->Finish(editor);
}
void ViFunc::v_0(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->HomeExtend();
    m_pVike->Finish(editor);
}
void ViFunc::v_bktab(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->BackTab();
    m_pVike->Finish(editor);
}
void ViFunc::v_tab(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->Tab();
    m_pVike->Finish(editor);
}
void ViFunc::v_y(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->Copy();
    editor->SetCurrentPos(editor->GetSelectionStart());
    m_pVike->ChangeMode(NORMAL);
    m_pVike->Finish(editor);
}
void ViFunc::v_d(VikeWin *m_pVike, wxScintilla* editor)
{
    editor->DeleteBack();
    m_pVike->ChangeMode(NORMAL);
    m_pVike->Finish(editor);
}
//////////////////////////////////////////////////////////////////////
