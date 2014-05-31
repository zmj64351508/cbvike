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
#include "cbvike.h"
#include "debugging.h"

/* the instance */
ViFunc ViFunc::gViFunc;

ViFunc::ViFunc()
{
    for(int i = 0; i < VIKE_OPERATOR_END - VIKE_OPERATOR_START - 1; i++){
        m_arrOperatorCommand[i] = &ViFunc::DummyOperatorCommand;
    }
    m_arrOperatorCommand[VIKE_DELETE - VIKE_OPERATOR_START - 1] = &ViFunc::DoDelete;
    m_arrOperatorCommand[VIKE_CHANGE - VIKE_OPERATOR_START - 1] = &ViFunc::DoChange;
    m_arrOperatorCommand[VIKE_YANK   - VIKE_OPERATOR_START - 1] = &ViFunc::DoYank;

    InitNormalCommand();
    m_bLineCuted = false;
}

void ViFunc::InitNormalCommand()
{
    for(int i = 0; i <= ASCII_MAX; i++){
        m_arrCommand[i] = &ViFunc::n_invalid;
    }

    m_arrCommand['0'] = &ViFunc::n_0;
    for(int i = '1'; i <= '9'; i++){
        m_arrCommand[i] = &ViFunc::n_number;
    }
    m_arrCommand['/'] = &ViFunc::n_search;
    m_arrCommand[':'] = &ViFunc::n_command;
    m_arrCommand['^'] = &ViFunc::n_circumflex;
    m_arrCommand['$'] = &ViFunc::n_dollar;
    m_arrCommand['a'] = &ViFunc::n_a;
    m_arrCommand['A'] = &ViFunc::n_A;
    m_arrCommand['b'] = &ViFunc::n_b;
    m_arrCommand['c'] = &ViFunc::n_c;
    m_arrCommand['C'] = &ViFunc::n_C;
    m_arrCommand['d'] = &ViFunc::n_d;
    m_arrCommand['D'] = &ViFunc::n_D;
    m_arrCommand['e'] = &ViFunc::n_e;
    m_arrCommand['f'] = &ViFunc::n_f;
    m_arrCommand['F'] = &ViFunc::n_F;
    m_arrCommand['g'] = &ViFunc::n_g;
    m_arrCommand['G'] = &ViFunc::n_G;
    m_arrCommand['i'] = &ViFunc::n_i;
    m_arrCommand['I'] = &ViFunc::n_I;
    m_arrCommand['h'] = &ViFunc::n_h;
    m_arrCommand['j'] = &ViFunc::n_j;
    m_arrCommand['k'] = &ViFunc::n_k;
    m_arrCommand['l'] = &ViFunc::n_l;
    m_arrCommand['n'] = &ViFunc::n_n;
    m_arrCommand['N'] = &ViFunc::n_N;
    m_arrCommand['o'] = &ViFunc::n_o;
    m_arrCommand['O'] = &ViFunc::n_O;
    m_arrCommand['p'] = &ViFunc::n_p;
    m_arrCommand['P'] = &ViFunc::n_P;
    m_arrCommand['r'] = &ViFunc::n_r;
    m_arrCommand['t'] = &ViFunc::n_t;
    m_arrCommand['T'] = &ViFunc::n_T;
    m_arrCommand['u'] = &ViFunc::n_u;
    m_arrCommand['w'] = &ViFunc::n_w;
    m_arrCommand['x'] = &ViFunc::n_x;
    m_arrCommand['X'] = &ViFunc::n_X;
    m_arrCommand['y'] = &ViFunc::n_y;
    m_arrCommand['Y'] = &ViFunc::n_Y;
    m_arrCommand['z'] = &ViFunc::n_z;
}

bool ViFunc::InsertModeSp(VikeWin *vike, int keyCode, wxScintilla *editor)
{
    bool skip = true;
    if(keyCode == WXK_ESCAPE){
        ViFunc::i_esc(vike, editor);
        skip = false;
    }
    return skip;
}

bool ViFunc::NormalModeSp(VikeWin *vike, int keyCode, int m_iModifier, wxScintilla *editor)
{
    bool skip = false;

    if(keyCode == WXK_ESCAPE){
        skip = n_esc(vike, editor);
    }else if(keyCode == WXK_RETURN){
        n_enter(vike, editor);
    }else if(keyCode == WXK_BACK){
        n_backspace(vike, editor);
    }else if(m_iModifier == wxMOD_CONTROL){
        /* keyCode doesnot distinguish 'a' and 'A', and they actually
           are the same with CTRL */
        switch(keyCode){
        case 'R':
            n_ctrl_r(vike, editor, keyCode); return skip;
            break;
        default:
            vike->Finish(editor);
            skip = true;
            break;
        }
    }else{
        skip = true;
    }
    return skip;
}

void ViFunc::Search(VikeWin *vike, int keyCode, wxScintilla *editor)
{
    vike->GetSearchCmd().AppendCommand(keyCode);
}

void ViFunc::Command(VikeWin *vike, int keyCode, wxScintilla *editor)
{
    vike->GetGeneralCmd().AppendCommand(keyCode);
}

bool ViFunc::NormalMode(VikeWin *vike, int keyCode, int m_iModifier, wxScintilla *editor)
{
    bool skip = FALSE;

    // check some special state first
    switch(vike->GetState()){
    case VIKE_SEARCH:
        Search(vike, keyCode, editor); return skip;
    case VIKE_COMMAND:
        Command(vike, keyCode, editor); return skip;
    case VIKE_REPLACE:
        n_r_any(vike, keyCode, editor); return skip;
    case VIKE_CHANGE_IN:
        n_ci_some(vike, keyCode, editor); return skip;
    case VIKE_FIND_FORWARD:
    case VIKE_FIND_BACKWARD:
    case VIKE_TILL_FORWARD:
    case VIKE_TILL_BACKWARD:
        n_find(vike, editor, vike->GetState(), keyCode); return skip;
    default:
        HandleCommand(vike, editor, keyCode);
        break;
    }
    return skip;
}

bool ViFunc::EspecialKeyHandler(VikeWin *vike, wxKeyEvent &event)
{
    int keyCode = event.GetKeyCode();
    wxScintilla *editor = (wxScintilla *)event.GetEventObject();
    bool skip = TRUE;
    LOGIT(wxT("vifunc::especial_key_press - customer key event,keycode: [%d]"), keyCode);
    switch(vike->GetMode()){
    case INSERT:
        skip = InsertModeSp(vike, keyCode, editor);
        break;
    case NORMAL:
        skip = NormalModeSp(vike, keyCode, event.GetModifiers(), editor);
        break;
    default:
        break;
    }
    return skip;
}

//if return FALSE,not process here,leave next.
//
bool ViFunc::NormalKeyHandler(VikeWin *vike, wxKeyEvent &event)
{
    int keyCode = event.GetKeyCode();
    wxScintilla *editor = (wxScintilla *)event.GetEventObject();

    LOGIT(wxT("vifunc::execute - customer key event,keycode: [%d]"), keyCode);
    //LOGIT(wxT("current mode is %d"), vike->GetMode());
    //LOGIT(wxT("current state is %d"), vike->GetState());

    bool skip = FALSE;
    switch(vike->GetMode()){
    case NORMAL:
        skip = NormalMode(vike, keyCode, event.GetModifiers(), editor);
        break;
    case VISUAL:
    case INSERT:
    default:
        skip = TRUE;
        break;
    }

    return skip;
}

void ViFunc::HandleCommand(VikeWin *vike, wxScintilla *editor, int keyCode)
{
    /* Command only inlcude ASCII characters */
    if(keyCode < 0 || keyCode > ASCII_MAX){
        vike->Finish(editor);
        vike->SetState(VIKE_INVALID);
        return;
    }
    (this->*m_arrCommand[keyCode])(vike, editor, keyCode);
}

int ViFunc::DoOperatorCommand(VikeWin *vike, int curState, int startPos, int endPos, int flag, wxScintilla *editor)
{
    if(curState >= VIKE_OPERATOR_END || curState <= VIKE_OPERATOR_START){
        return -1;
    }
    return (this->*m_arrOperatorCommand[curState - VIKE_OPERATOR_START - 1])(vike, startPos, endPos, flag, editor);
}

void ViFunc::DoMotionCommand(VikeWin *vike, int dupNum, unsigned int flag, wxScintilla *editor, MotionCommandType motion, int keyCode)
{
    int startPos, endPos;
    /* Motion direction */
    if(flag & MOTION_FORWARD){
        startPos = editor->GetCurrentPos();
    }else{
        endPos = editor->GetCurrentPos();
    }
    editor->BeginUndoAction();
    /* Do dupicated motion */
    //for(int i = 0; i < dupNum; i++){
        //(this->*motion)(vike, keyCode, i, editor);
    //}
    (this->*motion)(vike, dupNum, editor, keyCode);

    if(flag & MOTION_FORWARD){
        endPos = editor->GetCurrentPos();
    }else{
        startPos = editor->GetCurrentPos();
    }

    if(flag & MOTION_INCLUDE_END){
        endPos++;
    }
    LOGIT(_T("start is %d, end is %d"), startPos, endPos);

    /* Motion finish, we can do operation */
    DoOperatorCommand(vike, vike->GetState(), startPos, endPos, flag, editor);
    editor->EndUndoAction();
}

int ViFunc::DummyOperatorCommand(VikeWin *vike, int startPos, int endPos, int flag, wxScintilla *editor)
{
    LOGIT(_T("Not implemented edit command"));
}

int ViFunc::GetDupNum(VikeWin *vike, int maxLevel)
{
    int dupNum = vike->GetDupNumber();
    for(int i = 0; i < (maxLevel - 1) && vike->GetStateCount() > 1; i++){
        vike->PopState();
        dupNum *= vike->GetDupNumber();
    }
    return dupNum;
}

int ViFunc::DoDelete(VikeWin *vike, int startPos, int endPos, int flag, wxScintilla *editor)
{
    int realStart = startPos;
    int realEnd = endPos;
    int startLine = editor->LineFromPosition(startPos);
    int endLine = editor->LineFromPosition(endPos);
    if(flag & MOTION_LINE){
        realStart = editor->PositionFromLine(startLine);
        realEnd = editor->GetLineEndPosition(endLine);
        m_bLineCuted = true;
    }else{
        m_bLineCuted = false;
    }

    /* Real action start */
    editor->SetSelectionStart(realStart);
    editor->SetSelectionEnd(realEnd);
    editor->Cut();

    if(m_bLineCuted){
        editor->LineDelete();
    }
//    /* Start and end not in the same line & Not whole line cut */
//    else if(startLine != endLine){
//        editor->NewLine();
//        editor->LineUp();
//        editor->LineEnd();
//    }
    /* Real action end */
}

int ViFunc::DoChange(VikeWin *vike, int startPos, int endPos, int flag, wxScintilla *editor)
{
    DoDelete(vike, startPos, endPos, flag, editor);
    if(flag & MOTION_LINE){
        editor->NewLine();
        editor->LineUp();
    }
    vike->ChangeMode(INSERT, editor);
}

int ViFunc::DoYank(VikeWin *vike, int startPos, int endPos, int flag, wxScintilla *editor)
{
    int realStart = startPos;
    int realEnd = endPos;

    int startLine = editor->LineFromPosition(startPos);
    int endLine = editor->LineFromPosition(endPos);
    if(flag & MOTION_LINE){
        realStart = editor->PositionFromLine(startLine);
        realEnd = editor->GetLineEndPosition(endLine);
        m_bLineCuted = true;
    }else{
        m_bLineCuted = false;
    }

    /* Real action start */
    editor->SetSelectionVoid(realStart, realEnd);
    editor->Copy();

    /* Goto the start position after yank */
    if(flag & MOTION_FORWARD){
        editor->GotoPos(startPos);
    }else{
        editor->GotoPos(endPos);
    }
    editor->ChooseCaretX();
//    else{
//        if(flag & MOTION_FORWARD){
//            editor->CharLeft();
//        }else{
//            editor->CharRight();
//        }
//    }
    // editor->GotoPos(startPos);
    /* Real action end */
}

//insert mode
void ViFunc::i_esc(VikeWin *vike, wxScintilla* editor)
{
    vike->ChangeMode(NORMAL, editor);
    if(editor->GetCurrentPos() != editor->PositionFromLine(editor->GetCurrentLine()))
        editor->CharLeft();
    vike->Finish(editor);
}

//normal mode
bool ViFunc::n_esc(VikeWin *vike, wxScintilla* editor)
{
    bool skip = false;
    editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
    switch(vike->GetState()){
    case VIKE_SEARCH:
        vike->GetSearchCmd().Store();
        vike->GetSearchCmd().Clear();
        //vike->ResetState();
        vike->SetState(VIKE_END);
        break;
    case VIKE_COMMAND:
        vike->GetGeneralCmd().Store();
        vike->GetGeneralCmd().Clear();
        vike->SetState(VIKE_END);
        break;
    case VIKE_START:
        if(!vike->IsDup() && vike->GetStateCount() == 1){
            skip = true;
        }
        break;
    default:
        break;
    }
    vike->Finish(editor);
    return skip;
}
void ViFunc::n_enter(VikeWin* vike, wxScintilla* editor)
{
    int startPos, endPos, state;
    switch(vike->GetState()){
    case VIKE_SEARCH:
        /* treated like motion command */
        editor->BeginUndoAction();
        startPos = editor->GetCurrentPos();
        vike->GetSearchCmd().doSearch(editor);
        endPos = editor->GetCurrentPos();
        vike->GetSearchCmd().Store();
        vike->GetSearchCmd().Clear();

        vike->PopState();
        state = vike->GetState();
        DoOperatorCommand(vike, state, startPos, endPos, MOTION_CHAR, editor);
        editor->EndUndoAction();
        vike->SetState(VIKE_END);

        break;
    case VIKE_COMMAND:
        vike->GetGeneralCmd().ParseCommand(vike, editor);
        vike->GetGeneralCmd().Store();
        vike->GetGeneralCmd().Clear();
        vike->SetState(VIKE_END);
        break;
    default:
        break;
    }
}
void ViFunc::n_backspace(VikeWin* vike, wxScintilla* editor)
{
    int num = vike->GetDupNumber();
    switch(vike->GetState()){
    case VIKE_SEARCH:
        vike->GetSearchCmd().BackCommand();
        if(vike->GetSearchCmd().IsEmpty()){
            vike->ResetState();
            vike->Finish(editor);
        }
        break;
    case VIKE_COMMAND:
        vike->GetGeneralCmd().BackCommand();
        if(vike->GetGeneralCmd().IsEmpty()){
            vike->Finish(editor);
        }
        break;
    default:
        for(int i = 0; i < num; i++){
            editor->CharLeft();
        }
        vike->Finish(editor);
        break;
    }

}
//void ViFunc::n_delete(VikeWin *vike, wxScintilla* editor)
//{
//    if(editor->GetSelectionStart() == editor->GetSelectionEnd()){
//        editor->CharRight();
//    }
//    editor->DeleteBack();
//    vike->Finish(editor);
//}
void ViFunc::n_number(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    int number = keyCode - '0';
    switch(vike->GetState()){
    case VIKE_DELETE:
    case VIKE_CHANGE:
    case VIKE_YANK:
        vike->PushState(VIKE_START);
        vike->ShiftAddDupNumber(number);
        LOGIT(_T("motion number now is %d"), vike->GetDupNumber());
        break;
    case VIKE_START:
        vike->ShiftAddDupNumber(number);
        LOGIT(_T("number now is %d"), vike->GetDupNumber());
        break;
    default:
        vike->SetState(VIKE_START);
        break;
    }
}
void ViFunc::n_tom(VikeWin *vike, wxScintilla* editor)
{
}
void ViFunc::n_toma(VikeWin *vike, wxScintilla* editor)
{
}
void ViFunc::n_m(VikeWin *vike, wxScintilla* editor)
{
}

#define MAX_COMMAND_LEVEL_CHECK(vike, level) \
do{\
    if(vike->GetStateCount() > level){\
        vike->SetState(VIKE_END);\
        return;\
    }\
}while(0)

#define SINGLE_COMMAND(vike, editor, command, level, curkey) \
do{\
    MAX_COMMAND_LEVEL_CHECK(vike, level);\
    int dupNum = GetDupNum(vike, level);\
    switch(vike->GetState()){\
    case VIKE_START:\
        command(vike, dupNum, editor, curkey);\
    default:\
        vike->SetState(VIKE_END);\
    }\
}while(0)
/****** Motion Commands *******/
void ViFunc::n_circumflex(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    //SINGLE_COMMAND(vike, editor, n_circumflex_end, 2, keyCode);
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    DoMotionCommand(vike, 1, MOTION_CHAR|MOTION_BACKWARD, editor, &ViFunc::n_circumflex_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_find(VikeWin *vike, wxScintilla *editor, VikeStateEnum state, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);

    switch(state){
    case VIKE_FIND_FORWARD:
        DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_FORWARD|MOTION_INCLUDE_END, editor, &ViFunc::n_f_any, keyCode); break;
    case VIKE_FIND_BACKWARD:
        DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_BACKWARD|MOTION_INCLUDE_END, editor, &ViFunc::n_F_any, keyCode); break;
    case VIKE_TILL_FORWARD:
        DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_FORWARD|MOTION_INCLUDE_END, editor, &ViFunc::n_t_any, keyCode); break;
    case VIKE_TILL_BACKWARD:
        DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_BACKWARD|MOTION_INCLUDE_END, editor, &ViFunc::n_F_any, keyCode); break;
    default:
        assert(false); break;
    }

    vike->Finish(editor);
}

void ViFunc::n_dollar(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_FORWARD, editor, &ViFunc::n_dollar_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_0(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    //MAX_COMMAND_LEVEL_CHECK(vike, 2);
    switch(vike->GetState()){
    case VIKE_START:
        if(vike->IsDup()){
            ViFunc::n_number(vike, editor, keyCode);
            vike->SetState(VIKE_START);
        }else{
            DoMotionCommand(vike, 1, MOTION_CHAR|MOTION_BACKWARD, editor, &ViFunc::n_0_end, keyCode);
            vike->Finish(editor);
        }
        break;
    case VIKE_DELETE:
        /* duplicated number is not meaningful in this case, but it may change the result of n_0_end */
        vike->ClearDupNumber();
        DoMotionCommand(vike, 1, MOTION_CHAR|MOTION_BACKWARD, editor, &ViFunc::n_0_end, keyCode);
        vike->Finish(editor);
        break;
    default:
        vike->SetState(VIKE_END);
    }
}

void ViFunc::n_b(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_BACKWARD, editor, &ViFunc::n_b_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_w(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_FORWARD, editor, &ViFunc::n_w_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_e(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_FORWARD|MOTION_INCLUDE_END, editor, &ViFunc::n_e_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_h(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_BACKWARD, editor, &ViFunc::n_h_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_j(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_LINE|MOTION_FORWARD|MOTION_INCLUDE_END, editor, &ViFunc::n_j_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_k(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_LINE|MOTION_BACKWARD, editor, &ViFunc::n_k_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_l(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_FORWARD, editor, &ViFunc::n_l_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_n(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_FORWARD, editor, &ViFunc::n_n_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_N(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);
    DoMotionCommand(vike, dupNum, MOTION_CHAR|MOTION_BACKWARD, editor, &ViFunc::n_N_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_g(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    switch(vike->GetState()){
    case VIKE_DELETE:
    case VIKE_YANK:
    case VIKE_CHANGE:
        vike->PushState(VIKE_GO);
        break;
    case VIKE_START:
        vike->SetState(VIKE_GO);
        break;
    case VIKE_GO:
        vike->PopState();
        DoMotionCommand(vike, 1, MOTION_CHAR|MOTION_BACKWARD, editor, &ViFunc::n_gg, keyCode);
        vike->SetState(VIKE_END);
        break;
    default:
        vike->SetState(VIKE_END);
        break;
    }
}

/* 2d3G == d6G == 6dG */
void ViFunc::n_G(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = vike->GetDupNumber();
    bool isDup = vike->IsDup();
    /* check the previous state */
    if(vike->GetStateCount() > 1){
        vike->PopState();
        isDup |= vike->IsDup();
        dupNum *= vike->GetDupNumber();
    }
    int curLine = editor->GetCurrentLine();
    unsigned int flag = MOTION_LINE;

    if(isDup && dupNum >= curLine){
        flag |= MOTION_FORWARD;
    }else if(isDup && dupNum < curLine){
        flag |= MOTION_BACKWARD;
    }else{
        flag |= MOTION_FORWARD;
        dupNum = 0;
    }
    DoMotionCommand(vike, dupNum, flag, editor, &ViFunc::n_G_end, keyCode);
    vike->Finish(editor);
}

void ViFunc::n_f(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    switch(vike->GetState()){
    case VIKE_CHANGE:
    case VIKE_DELETE:
    case VIKE_YANK:
        vike->PushState(VIKE_FIND_FORWARD);
        break;
    case VIKE_START:
        vike->SetState(VIKE_FIND_FORWARD);
        break;
    default:
        vike->SetState(VIKE_END);
        break;
    }
}

void ViFunc::n_F(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    switch(vike->GetState()){
    case VIKE_CHANGE:
    case VIKE_DELETE:
    case VIKE_YANK:
        vike->PushState(VIKE_FIND_BACKWARD);
        break;
    case VIKE_START:
        vike->SetState(VIKE_FIND_BACKWARD);
        break;
    default:
        vike->SetState(VIKE_END);
        break;
    }
}

void ViFunc::n_t(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum;
    switch(vike->GetState()){
    case VIKE_CHANGE:
    case VIKE_DELETE:
    case VIKE_YANK:
        vike->PushState(VIKE_TILL_FORWARD);
        break;
    case VIKE_START:
        vike->SetState(VIKE_TILL_FORWARD);
        break;
    case VIKE_Z:
        MAX_COMMAND_LEVEL_CHECK(vike, 1);
        dupNum = GetDupNum(vike, 1);
        n_zt(vike, dupNum, editor, keyCode);
        vike->Finish(editor);
        break;
    default:
        vike->SetState(VIKE_END);
        break;
    }
}

void ViFunc::n_T(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    switch(vike->GetState()){
    case VIKE_CHANGE:
    case VIKE_DELETE:
    case VIKE_YANK:
        vike->PushState(VIKE_TILL_BACKWARD);
        break;
    case VIKE_START:
        vike->SetState(VIKE_TILL_BACKWARD);
        break;
    default:
        vike->SetState(VIKE_END);
        break;
    }
}

void ViFunc::n_search(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    vike->PushState(VIKE_SEARCH);
    //vike->Finish(editor);
}

/****** Common Commands *******/
void ViFunc::n_a(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_a_end, 1, keyCode);
}

void ViFunc::n_A(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_A_end, 1, keyCode);
}

void ViFunc::n_i(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    //SINGLE_COMMAND(vike, editor, n_i_end);
    MAX_COMMAND_LEVEL_CHECK(vike, 1);
    switch(vike->GetState()){
    case VIKE_START:
        n_i_end(vike, 1, editor, keyCode);
        vike->SetState(VIKE_END);
        break;
    case VIKE_CHANGE:
        n_ci(vike, editor);
        vike->SetState(VIKE_CHANGE_IN);
        break;
    default:
        vike->SetState(VIKE_END);
        break;
    }
}

void ViFunc::n_I(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_I_end, 1, keyCode);
}

void ViFunc::n_o(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_o_end, 1, keyCode);
}
void ViFunc::n_O(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_O_end, 1, keyCode);
}

void ViFunc::n_D(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 1);
    switch(vike->GetState()){
    case VIKE_START:
        vike->SetState(VIKE_DELETE);
        n_dollar(vike, editor, keyCode);
    default:
        vike->SetState(VIKE_END);
    }
}

void ViFunc::n_Y(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 1);
    int dupNum = GetDupNum(vike, 1);
    switch(vike->GetState()){
    case VIKE_START:
        vike->SetState(VIKE_YANK);
        n_yy(vike, dupNum, editor, keyCode);
    default:
        vike->SetState(VIKE_END);
    }
}

void ViFunc::n_C(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 1);
    switch(vike->GetState()){
    case VIKE_START:
        vike->SetState(VIKE_CHANGE);
        n_dollar(vike, editor, keyCode);
    default:
        vike->SetState(VIKE_END);
    }
}

void ViFunc::n_x(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_x_end, 1, keyCode);
}

void ViFunc::n_X(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_X_end, 1, keyCode);
}

void ViFunc::n_p(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_p_end, 1, keyCode);
}

void ViFunc::n_P(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_P_end, 1, keyCode);
}

void ViFunc::n_u(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_u_end, 1, keyCode);
}

void ViFunc::n_r(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 1);
    switch(vike->GetState()){
    case VIKE_START:
        vike->SetState(VIKE_REPLACE);
        break;
    default:
        vike->SetState(VIKE_END);
        break;
    }
}

void ViFunc::n_ctrl_r(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    SINGLE_COMMAND(vike, editor, n_ctrl_r_end, 1, keyCode);
}

void ViFunc::n_z(VikeWin *vike, wxScintilla *editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 1);
    int dupNum = GetDupNum(vike, 1);
    switch(vike->GetState()){
    case VIKE_START:
        vike->SetState(VIKE_Z);
        break;
    case VIKE_Z:
        n_zz(vike, dupNum, editor, keyCode);
        vike->Finish(editor);
        break;
    default:
        vike->Finish(editor);
        vike->SetState(VIKE_INVALID);
    }
}

/****** Operator Commands *******/
void ViFunc::n_y(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);

    /* case : d2d */
    if(vike->GetStateCount() == 2 && vike->GetLastState() == VIKE_YANK){
        n_yy(vike, dupNum, editor, keyCode);
        vike->SetState(VIKE_END);
        return;
    }

    switch(vike->GetState()){
    case VIKE_START:
        vike->SetState(VIKE_YANK);
        break;
    case VIKE_YANK:
        ViFunc::n_yy(vike, dupNum, editor, keyCode);
        vike->SetState(VIKE_END);
        break;
    default:
        vike->SetState(VIKE_END);
    }
}
void ViFunc::n_d(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);

    /* case : d2d */
    if(vike->GetStateCount() == 2 && vike->GetLastState() == VIKE_DELETE){
        n_dd(vike, dupNum, editor, keyCode);
        vike->SetState(VIKE_END);
        return;
    }

    switch(vike->GetState()){
    case VIKE_START:
        vike->SetState(VIKE_DELETE);
        break;
    case VIKE_DELETE:
        ViFunc::n_dd(vike, dupNum, editor, keyCode);
        vike->SetState(VIKE_END);
        break;
    default:
        vike->SetState(VIKE_END);
        break;
    }
}
void ViFunc::n_c(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    MAX_COMMAND_LEVEL_CHECK(vike, 2);
    int dupNum = GetDupNum(vike, 2);

    /* case : c2c */
    if(vike->GetStateCount() == 2 && vike->GetLastState() == VIKE_CHANGE){
        n_cc(vike, dupNum, editor, keyCode);
        vike->SetState(VIKE_END);
        return;
    }

    switch(vike->GetState()){
    case VIKE_START:
        vike->SetState(VIKE_CHANGE);
        break;
    case VIKE_CHANGE:
        ViFunc::n_cc(vike, dupNum, editor, keyCode);
        vike->SetState(VIKE_END);
        break;
    default:
        vike->SetState(VIKE_END);
        break;
    }
}

/****** Extra Commands *******/
void ViFunc::n_command(VikeWin *vike, wxScintilla* editor, int keyCode)
{
    vike->SetState(VIKE_COMMAND);
}

/****** Commands Implementation *******/
void ViFunc::n_a_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    if(editor->GetCurrentPos() != editor->GetLineEndPosition(editor->GetCurrentLine())){
        editor->CharRight();
    }
    vike->ChangeMode(INSERT, editor);
    vike->Finish(editor);
}
void ViFunc::n_A_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->LineEnd();
    vike->ChangeMode(INSERT, editor);
    vike->Finish(editor);
}
void ViFunc::n_h_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        if(editor->GetCurrentPos() != editor->PositionFromLine(editor->GetCurrentLine())){
            editor->CharLeft();
        }
    }
}
void ViFunc::n_j_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        editor->LineDown();
    }
}
void ViFunc::n_k_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        editor->LineUp();
    }
}
void ViFunc::n_l_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        if(editor->GetCurrentPos() != editor->GetLineEndPosition(editor->GetCurrentLine())){
            editor->CharRight();
        }
    }
    //vike->Finish(editor);
}
void ViFunc::n_i_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->BeginUndoAction();
    vike->ChangeMode(INSERT, editor);
    editor->EndUndoAction();
    vike->Finish(editor);
}
void ViFunc::n_I_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->VCHome();
    vike->ChangeMode(INSERT, editor);
    vike->Finish(editor);
}
void ViFunc::n_0_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    if(vike->IsDup()) {
        vike->ShiftAddDupNumber(0);
    } else {
        editor->Home();
    }
}
void ViFunc::n_circumflex_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->VCHome();
}
void ViFunc::n_dollar_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->LineEnd();
    for(int i = 0; i < dupNum - 1; i++){
        editor->LineDown();
        editor->LineEnd();
    }
}
void ViFunc::n_n_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    int pos;
    for(int i = 0; i < dupNum; i++){
        pos = editor->GetCurrentPos();
        editor->GotoPos(vike->GetSearchCmd().NextPos(pos));
    }
}
void ViFunc::n_N_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    int pos;
    for(int i = 0; i < dupNum; i++){
        pos = editor->GetCurrentPos();
        editor->GotoPos(vike->GetSearchCmd().PrevPos(pos));
    }
}
void ViFunc::n_G_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    if(dupNum > 0) {
        editor->GotoLine(dupNum - 1);
    } else {
        editor->DocumentEnd();
    }
}
void ViFunc::n_bktab(VikeWin *vike, wxScintilla* editor)
{
}
void ViFunc::n_bktabbktab(VikeWin *vike, wxScintilla* editor)
{
    editor->BackTab();
    vike->Finish(editor);
}
void ViFunc::n_tab(VikeWin *vike, wxScintilla* editor)
{
}
void ViFunc::n_tabtab(VikeWin *vike, wxScintilla* editor)
{
    editor->Tab();
    vike->Finish(editor);
}

void ViFunc::n_ctrl_n(VikeWin *vike, wxScintilla* editor)
{
    vike->Finish(editor);
}
void ViFunc::n_ctrl_p(VikeWin *vike, wxScintilla* editor)
{
    vike->Finish(editor);
}
void ViFunc::n_o_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->BeginUndoAction();
    editor->LineEnd();
    editor->NewLine();
    vike->ChangeMode(INSERT, editor);
    editor->EndUndoAction();
    vike->Finish(editor);
}
void ViFunc::n_O_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->BeginUndoAction();
    editor->VCHome();
    editor->NewLine();
    editor->LineUp();
    vike->ChangeMode(INSERT, editor);
    editor->EndUndoAction();
    vike->Finish(editor);
}
void ViFunc::n_gg(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->DocumentStart();
}
void ViFunc::n_yy(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    int curPos = editor->GetCurrentPos();

    int start_line = editor->GetCurrentLine();
    editor->SetSelectionStart(editor->PositionFromLine(start_line));
    int end_pos = editor->GetLineEndPosition(start_line + dupNum - 1);
    editor->SetSelectionEnd(end_pos);
    editor->Copy();
    editor->SetEmptySelection(curPos);

    m_bLineCuted = true;
    vike->Finish(editor);
}
void ViFunc::n_zz(VikeWin *vike, int dupNum, wxScintilla *editor, int keyCode)
{
    if(vike->IsDup() && dupNum > 0){
        editor->GotoLine(dupNum - 1);
    }
    editor->VerticalCentreCaret();
    vike->Finish(editor);
}
void ViFunc::n_zt(VikeWin *vike, int dupNum, wxScintilla *editor, int keyCode)
{
    if(vike->IsDup() && dupNum > 0){
        editor->GotoLine(dupNum - 1);
    }
    int curLine = editor->GetCurrentLine();
    int firstLine = editor->GetFirstVisibleLine();
    editor->LineScroll(0, curLine - firstLine);
    vike->Finish(editor);
}
void ViFunc::n_p_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
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
        n_l(vike, editor, keyCode);
        curPos = editor->GetCurrentPos();
    }

    editor->Paste();

    if(m_bLineCuted){
        editor->GotoLine(line);
        editor->VCHome();
    }else{
        editor->GotoPos(curPos);
        /* stick caret x position at curPos when move up and down */
        editor->ChooseCaretX();
    }
    editor->EndUndoAction();

    vike->Finish(editor);
}
void ViFunc::n_P_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    int line;
    int curPos;

    editor->BeginUndoAction();
    if(m_bLineCuted){
        editor->VCHome();
        editor->NewLine();
        editor->LineUp();
        editor->DelLineLeft();
        line = editor->GetCurrentLine();
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
    vike->Finish(editor);
}
void ViFunc::n_ctrl_f(VikeWin *vike, wxScintilla* editor)
{
    editor->PageDown();
    vike->Finish(editor);
}

void ViFunc::n_v(VikeWin *vike, wxScintilla* editor)
{
    //vike->ChangeMode(VISUAL);
    //editor->SetCaretForeground(wxColour(0xFF, 0x00, 0x00));
    vike->Finish(editor);
}
void ViFunc::n_ctrl_b(VikeWin *vike, wxScintilla* editor)
{
    editor->PageUp();
    vike->Finish(editor);
}
void ViFunc::n_b_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        editor->WordLeft();
    }
}
void ViFunc::n_w_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        editor->WordRight();
    }
}
void ViFunc::n_e_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    int curPos;
    for(int i = 0; i < dupNum; i++){
        curPos = editor->GetCurrentPos();
        editor->CharRight();
        editor->WordRightEnd();
        /* scintilla think the end of word is the next character of the last one in the word */
        if(curPos != editor->GetCurrentPos()){
            editor->CharLeft();
        }
    }
}
void ViFunc::n_dd(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->BeginUndoAction();
    int startLine = editor->GetCurrentLine();
    int startPos = editor->PositionFromLine(startLine);
    int endPos = editor->GetLineEndPosition(startLine + dupNum - 1);
    DoDelete(vike, startPos, endPos, MOTION_LINE, editor);
    editor->EndUndoAction();

    vike->Finish(editor);
}
void ViFunc::n_cc(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    editor->BeginUndoAction();
    n_dd(vike, dupNum, editor, 'c');
    n_O_end(vike, 1, editor, 'c');
    editor->EndUndoAction();
    vike->Finish(editor);
}
void ViFunc::n_ci(VikeWin *vike, wxScintilla* editor)
{
}

static bool IsBracketStart(wxChar c)
{
    if(c == '(' || c == '{' || c == '[' || c == '<' || c == '\'' || c == '"'){
        return true;
    }else{
        return false;
    }
}

static bool IsBracketEnd(wxChar c)
{
    if(c == ')' || c == '}' || c == ']' || c == '>' || c == '\'' || c == '"'){
        return true;
    }else{
        return false;
    }
}

static wxChar BracketPair(wxChar c)
{
    wxChar pair = -1;
    switch(c){
    /* bracket start to end*/
    case '(':
        pair = ')'; break;
    case '{':
        pair = '}'; break;
    case '[':
        pair = ']'; break;
    case '<':
        pair = '>'; break;
    /* bracket end to start */
    case ')':
        pair = '('; break;
    case '}':
        pair = '{'; break;
    case ']':
        pair = '['; break;
    case '>':
        pair = '<'; break;
    /* same with start and end */
    case '\'':
        pair = '\''; break;
    case '"':
        pair = '"'; break;
    default:
        pair = -1;
        break;
    }
    return pair;
}

int ViFunc::FindUnpairBracket(wxChar bracketStart, bool lookForward, wxScintilla *editor)
{
    /* NOTE: We don't check the bracketStart. So make sure bracketStart is ( or < or { , etc. */
    wxChar bracketEnd = BracketPair(bracketStart);
    wxChar curChar;
    int unMatch = 0;

    /* Check current character first */
    int curPos = editor->GetCurrentPos();
    curChar = editor->GetCharAt(curPos);
    if(!lookForward && curChar == bracketStart){
        return curPos;
    }else if(lookForward && curChar == bracketEnd){
        return curPos;
    }

    /* Lookforward or lookbackward to find the first unpair bracket */
    for(lookForward ? curPos = editor->GetCurrentPos() + 1 : curPos = editor->GetCurrentPos() - 1;
        lookForward ? curPos < editor->GetLength()        : curPos >= 0;
        lookForward ? curPos++                            : curPos--)
    {
        curChar = editor->GetCharAt(curPos);
        if(lookForward ? curChar == bracketEnd : curChar == bracketStart){
            unMatch += 1;
        }else if(lookForward ? curChar == bracketStart : curChar == bracketEnd){
            unMatch -= 1;
        }
        if(unMatch > 0){
            break;
        }
    }

    /* not found */
    if(curPos == editor->GetLength() || curPos < 0){
        curPos = -1;
    }

    return curPos;
}

int ViFunc::SelectBracket(int keyCode, wxScintilla* editor)
{
    /* not handle any character that is not bracket */
    bool isStart = IsBracketStart(keyCode);
    bool isEnd = IsBracketEnd(keyCode);
    if(!isStart && !isEnd){
        return -1;
    }

    wxChar bracketStart;
    if(isStart){
        bracketStart = (wxChar)keyCode;
    }else{
        bracketStart = BracketPair(keyCode);
    }

    int start = FindUnpairBracket(bracketStart, false, editor);
    int end = FindUnpairBracket(bracketStart, true, editor);

    if(start >= 0 && end >= 0){
        /* start with the next postion of the bracket start */
        start += 1;
        editor->SetSelectionStart(start);
        editor->SetSelectionEnd(end);
        return end - start;
    }

    return -1;
}

void ViFunc::n_ci_some(VikeWin *vike, int keyCode, wxScintilla* editor)
{
    editor->BeginUndoAction();
    int curPos = editor->GetCurrentPos();
    int len = SelectBracket(keyCode, editor);
    if(len >= 0){
        /* TODO: Is there any way don't need to check length is 0 or not? */
        if(len > 0){
            editor->DeleteBack();
        }
        vike->ChangeMode(INSERT, editor);
    }else{
        /* if not find anything return to origin position */
        editor->GotoPos(curPos);
    }
    editor->EndUndoAction();
    vike->Finish(editor);
}

void ViFunc::n_r_any(VikeWin *vike, int keyCode, wxScintilla* editor)
{
    LOGIT(_T("r_any key %c\n"), keyCode);
    editor->CharRightExtend();
    wxString replace((wxChar)keyCode);
    editor->ReplaceSelection(replace);
    editor->CharLeft();
    delete replace;
    vike->Finish(editor);
}
int ViFunc::GotoCharCurrentLine(wxChar toFind, wxScintilla *editor, bool lookForward)
{
    wxString tf(toFind);
    int findPos = -1;
    int lineEnd = editor->GetLineEndPosition(editor->GetCurrentLine());
    int lineStart = editor->PositionFromLine(editor->GetCurrentLine());
    int originPos = editor->GetCurrentPos();

    if(lookForward){
        editor->CharRight();
        editor->SearchAnchor();
        findPos = editor->SearchNext(wxSCI_FIND_MATCHCASE, tf);
        if(findPos > lineEnd){
            findPos = -1;
        }
    }else{
        editor->CharLeft();
        editor->SearchAnchor();
        findPos = editor->SearchPrev(wxSCI_FIND_MATCHCASE, tf);
        if(findPos < lineStart){
            findPos = -1;
        }
    }

    /* Ensure we go to the proper position */
    if(findPos >= 0){
        editor->GotoPos(findPos);
    }else{
        editor->GotoPos(originPos);
    }

    return findPos;
}
void ViFunc::n_f_any(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    LOGIT(_T("f_any key %c\n"), keyCode);
    for(int i = 0; i < dupNum; i++){
        GotoCharCurrentLine(keyCode, editor, true/*forward*/);
    }
}
void ViFunc::n_F_any(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    LOGIT(_T("F_any key %c\n"), keyCode);
    for(int i = 0; i < dupNum; i++){
        GotoCharCurrentLine(keyCode, editor, false/*backward*/);
    }
}

void ViFunc::n_t_any(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    LOGIT(_T("t_any key %c\n"), keyCode);
    int curPos = editor->GetCurrentPos();
    int lineEnd = editor->GetLineEndPosition(editor->GetCurrentLine());
    /* no need to find */
    if(curPos == lineEnd){
        return;
    }

    for(int i = 0; i < dupNum; i++){
        editor->CharRight();
        int pos = GotoCharCurrentLine(keyCode, editor, true/*forward*/);
        if(pos > 0){
            editor->CharLeft();
        }else{
            editor->GotoPos(curPos);
            break;
        }
    }
}
void ViFunc::n_T_any(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    LOGIT(_T("T_any key %c\n"), keyCode);
    int pos = GotoCharCurrentLine(keyCode, editor, false/*forward*/);
    if(pos > 0){
        editor->CharRight();
    }
}
void ViFunc::n_u_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        editor->Undo();
    }
    //editor->SetCurrentPos(vike->GetUndoPos());
    vike->Finish(editor);
}
void ViFunc::n_x_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        editor->CharRight();
        editor->DeleteBack();
    }
    vike->Finish(editor);
}
void ViFunc::n_X_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        editor->DeleteBack();
    }
    vike->Finish(editor);
}
void ViFunc::n_ctrl_r_end(VikeWin *vike, int dupNum, wxScintilla* editor, int keyCode)
{
    for(int i = 0; i < dupNum; i++){
        editor->Redo();
    }
    vike->Finish(editor);
}
//////////////////////////////////////////////////////////////////////

//visual mode
void ViFunc::v_esc(VikeWin *vike, wxScintilla* editor)
{
    //vike->ChangeMode(NORMAL);
    //restore select extend
    //editor->SetCurrentPos(editor->GetSelectionStart());
    //editor->SetCaretForeground(wxColour(0x00, 0x00, 0x00));
    vike->Finish(editor);
}
void ViFunc::v_h(VikeWin *vike, wxScintilla* editor)
{
    editor->CharLeftExtend();
    vike->Finish(editor);
}
void ViFunc::v_j(VikeWin *vike, wxScintilla* editor)
{
    editor->LineDownExtend();
    vike->Finish(editor);
}
void ViFunc::v_k(VikeWin *vike, wxScintilla* editor)
{
    editor->LineUpExtend();
    vike->Finish(editor);
}
void ViFunc::v_l(VikeWin *vike, wxScintilla* editor)
{
    editor->CharRightExtend();
    vike->Finish(editor);
}
void ViFunc::v_e(VikeWin *vike, wxScintilla* editor)
{
    editor->WordRightExtend();
    vike->Finish(editor);
}
void ViFunc::v_b(VikeWin *vike, wxScintilla* editor)
{
    editor->WordLeftExtend();
    vike->Finish(editor);
}
void ViFunc::v_sf_4(VikeWin *vike, wxScintilla* editor)
{
    editor->LineEndExtend();
    vike->Finish(editor);
}
void ViFunc::v_sf_6(VikeWin *vike, wxScintilla* editor)
{
    editor->VCHomeExtend();
    vike->Finish(editor);
}
void ViFunc::v_0(VikeWin *vike, wxScintilla* editor)
{
    editor->HomeExtend();
    vike->Finish(editor);
}
void ViFunc::v_bktab(VikeWin *vike, wxScintilla* editor)
{
    editor->BackTab();
    vike->Finish(editor);
}
void ViFunc::v_tab(VikeWin *vike, wxScintilla* editor)
{
    editor->Tab();
    vike->Finish(editor);
}
void ViFunc::v_y(VikeWin *vike, wxScintilla* editor)
{
    editor->Copy();
    editor->SetCurrentPos(editor->GetSelectionStart());
    //vike->ChangeMode(NORMAL);
    vike->Finish(editor);
}
void ViFunc::v_d(VikeWin *vike, wxScintilla* editor)
{
    editor->DeleteBack();
    //vike->ChangeMode(NORMAL);
    vike->Finish(editor);
}
//////////////////////////////////////////////////////////////////////
