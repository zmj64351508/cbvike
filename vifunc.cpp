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



VikeWin::VikeWin()
{
    LOGIT(_T("VikeWin"));
    //init some params
    gMode = NORMAL;

    keyCode = 0;
    keyCodes = 0;
    Number = 0;
    CapsLock = wxGetKeyState(WXK_CAPITAL);
    LineCuted = 0;
    curChar=0;

    func = ViFunc::Instance();
}
 int VikeWin::execute(wxKeyEvent &event){
    return func->execute(this, event);
}

ViFunc::ViFunc()
{
    //insert mode
    MAKE_PAIRS(INSERT,NONE,0,27,&ViFunc::i_esc);//esc
    MAKE_PAIRS(INSERT,SHIFT,0,27,&ViFunc::i_esc);//esc
    MAKE_PAIRS(INSERT,CTRL,0,'[',&ViFunc::i_esc);//ctrl-[ same as esc

    //normal mode
    MAKE_PAIRS(NORMAL,NONE,0,27,&ViFunc::n_esc);//esc
    MAKE_PAIRS(NORMAL,CTRL,0,'[',&ViFunc::n_esc);//ctrl-[ same as esc
    MAKE_PAIRS(NORMAL,NONE,0,'0',&ViFunc::n_0); //0
    MAKE_PAIRS(NORMAL,NONE,0,'1',&ViFunc::n_number);//1
    MAKE_PAIRS(NORMAL,NONE,0,'2',&ViFunc::n_number);//2
    MAKE_PAIRS(NORMAL,NONE,0,'3',&ViFunc::n_number);//3
    MAKE_PAIRS(NORMAL,NONE,0,'4',&ViFunc::n_number);//4
    MAKE_PAIRS(NORMAL,NONE,0,'5',&ViFunc::n_number);//5
    MAKE_PAIRS(NORMAL,NONE,0,'6',&ViFunc::n_number);//6
    MAKE_PAIRS(NORMAL,NONE,0,'7',&ViFunc::n_number);//7
    MAKE_PAIRS(NORMAL,NONE,0,'8',&ViFunc::n_number);//8
    MAKE_PAIRS(NORMAL,NONE,0,'9',&ViFunc::n_number);//9

    /* move */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('h'),&ViFunc::n_h);//h
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('l'),&ViFunc::n_l);//l
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('j'),&ViFunc::n_j);//j
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('k'),&ViFunc::n_k);//k
    MAKE_PAIRS(NORMAL,NONE,0,WXK_LEFT,&ViFunc::n_h);//left
    MAKE_PAIRS(NORMAL,NONE,0,WXK_RIGHT,&ViFunc::n_l);//right
    MAKE_PAIRS(NORMAL,NONE,0,WXK_DOWN,&ViFunc::n_j);//down
    MAKE_PAIRS(NORMAL,NONE,0,WXK_UP,&ViFunc::n_k);//up
    MAKE_PAIRS(NORMAL,SHIFT,0,WXK_LEFT,&ViFunc::n_b);//left
    MAKE_PAIRS(NORMAL,SHIFT,0,WXK_RIGHT,&ViFunc::n_w);//right

    /* line move */
    MAKE_PAIRS(NORMAL,SHIFT,0,'6',&ViFunc::n_sf_6); //^
    MAKE_PAIRS(NORMAL,SHIFT,0,'4',&ViFunc::n_sf_4); //$
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_CODE('g'),&ViFunc::n_G); //G
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('g'),&ViFunc::n_g); //g
    MAKE_PAIRS(NORMAL,NONE,MAKE_CODE('g'),MAKE_CODE('g'),&ViFunc::n_gg); //gg

    /* insert and append */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('i'),&ViFunc::n_i);//i
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_CODE('i'),&ViFunc::n_I);//I
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('a'),&ViFunc::n_a); //a
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_CODE('a'),&ViFunc::n_A); //A


    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_CODE('f'),&ViFunc::n_ctrl_f); //ctrl-f
    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_CODE('b'),&ViFunc::n_ctrl_b); //ctrl-b

    /* new line */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('o'),&ViFunc::n_o); //o
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_CODE('o'),&ViFunc::n_O); //O

    /* next and previous word */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('w'),&ViFunc::n_w); //w
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('b'),&ViFunc::n_b); //b

    /* replace */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('r'),&ViFunc::n_r); //replace first key
    MAKE_PAIRS(NORMAL,NONE,MAKE_CODE('r'),0,&ViFunc::n_r_any); //replace sencond key
    MAKE_PAIRS(NORMAL,SHIFT,MAKE_CODE('r'),0,&ViFunc::n_r_any); //replace sencond key

    /* delete */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('d'),&ViFunc::n_d); //d
    MAKE_PAIRS(NORMAL,NONE,MAKE_CODE('d'),MAKE_CODE('d'),&ViFunc::n_dd); //dd
    MAKE_PAIRS(NORMAL,SHIFT,MAKE_CODE('d'),'4',&ViFunc::n_dsf_4); //d$
    MAKE_PAIRS(NORMAL,NONE,MAKE_CODE('d'),MAKE_CODE('w'),&ViFunc::n_dw); //dw

    /* change */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('c'),&ViFunc::n_c); //c
    MAKE_PAIRS(NORMAL,NONE,MAKE_CODE('c'),MAKE_CODE('4'),&ViFunc::n_csf_4); //c$
    MAKE_PAIRS(NORMAL,NONE,MAKE_CODE('c'),MAKE_CODE('w'),&ViFunc::n_cw); //cw
    MAKE_PAIRS(NORMAL,NONE,MAKE_CODE('c'),MAKE_CODE('c'),&ViFunc::n_cc); //cc

    /* undo and redo */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('u'),&ViFunc::n_u); //u
    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_CODE('r'),&ViFunc::n_ctrl_r); //ctrl-r

    /* cut */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('x'),&ViFunc::n_x); //x
    MAKE_PAIRS(NORMAL,SHIFT,0,'5',&ViFunc::n_sf_5); //%
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_CODE('x'),&ViFunc::n_X); //X

    /* copy */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('y'),&ViFunc::n_y); //y
    MAKE_PAIRS(NORMAL,NONE,MAKE_CODE('y'),MAKE_CODE('y'),&ViFunc::n_yy); //yy
    MAKE_PAIRS(NORMAL,NONE,MAKE_CODE('y'),MAKE_CODE('w'),&ViFunc::n_yw); //yw

    /* paste */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('p'),&ViFunc::n_p); //p
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_CODE('p'),&ViFunc::n_P); //P


    MAKE_PAIRS(NORMAL,SHIFT,0,',',&ViFunc::n_bktab); //<
    MAKE_PAIRS(NORMAL,SHIFT,',',',',&ViFunc::n_bktabbktab); //<<
    MAKE_PAIRS(NORMAL,SHIFT,0,'.',&ViFunc::n_tab); //>
    MAKE_PAIRS(NORMAL,SHIFT,'.','.',&ViFunc::n_tabtab); //>>
    MAKE_PAIRS(NORMAL,NONE,0,'/',&ViFunc::n_search); //search / TODO
    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_CODE('n'),&ViFunc::n_ctrl_n); //complete code ctrl-n TODO
    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_CODE('p'),&ViFunc::n_ctrl_p); //complete code ctrl-p TODO
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('m'),&ViFunc::n_m); //m

    MAKE_PAIRS(NORMAL,NONE,0,'~',&ViFunc::n_tom); //`
    MAKE_PAIRS(NORMAL,NONE,'~',MAKE_CODE('a'),&ViFunc::n_toma); //marker_a

    MAKE_PAIRS(NORMAL,NONE,0,MAKE_CODE('v'),&ViFunc::n_v); //visual mode
    //visual mode
    MAKE_PAIRS(VISUAL,NONE,0,27,&ViFunc::v_esc);//esc
    MAKE_PAIRS(VISUAL,CTRL,0,'[',&ViFunc::v_esc);//ctrl-[ same as esc
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_CODE('h'),&ViFunc::v_h);//h
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_CODE('l'),&ViFunc::v_l);//l
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_CODE('j'),&ViFunc::v_j);//j
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_CODE('k'),&ViFunc::v_k);//k
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_CODE('e'),&ViFunc::v_e);//e
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_CODE('b'),&ViFunc::v_b);//b
    MAKE_PAIRS(VISUAL,SHIFT,0,'4',&ViFunc::v_sf_4);//$
    MAKE_PAIRS(VISUAL,SHIFT,0,'6',&ViFunc::v_sf_6);//^
    MAKE_PAIRS(VISUAL,NONE,0,'0',&ViFunc::v_0);//0
    MAKE_PAIRS(VISUAL,SHIFT,0,',',&ViFunc::v_bktab); //<
    MAKE_PAIRS(VISUAL,SHIFT,0,'.',&ViFunc::v_tab); //>
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_CODE('y'),&ViFunc::v_y);//y
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_CODE('d'),&ViFunc::v_d);//d
}

//if return FALSE,not process here,leave next.
//
bool ViFunc::execute(VikeWin *curVike, wxKeyEvent &event)
{
    int fkey = NONE;
    curVike->keyCode = event.GetKeyCode();
    LOGIT(wxT("vifunc::execute - customer key event,keycode: [%d]"), curVike->keyCode);

    /* CapsLock */
    if(curVike->keyCode == 311){
        curVike->CapsLock = !curVike->CapsLock;
        return FALSE;
    }

    /* Generate fkey */
    if(event.m_controlDown) {
        fkey |= CTRL;
    } else if(event.m_shiftDown) {
        if(curVike->CapsLock){
            fkey &= ~SHIFT;
        }else{
            fkey |= SHIFT;
        }
    } else {
        if(curVike->CapsLock){
            fkey |= SHIFT;
        }else{
            fkey = NONE;
        }
    }

    /* Pressing Shift but no other key pressed */
    if(curVike->keyCode == WXK_SHIFT && event.m_shiftDown) return TRUE;

    unsigned long key;
    /* If entered r then ignore next key */
    if(GET_KEYCODE(curVike->keyCodes) == 'R'){
        key = calcKey(curVike->gMode,fkey,GET_KEYCODE(curVike->keyCodes),0);
    }else{
        key = calcKey(curVike->gMode,fkey,GET_KEYCODE(curVike->keyCodes),GET_KEYCODE(curVike->keyCode));
    }

    /* Generate ASCII charactor by key code and SHIFT */
    if(fkey & SHIFT){
        curVike->curChar = ShiftKeyTable[curVike->keyCode];
    }else{
        if(curVike->keyCode >= 'A' && curVike->keyCode <= 'Z'){
            curVike->curChar = curVike->keyCode-'A'+'a';
        }else{
            curVike->curChar = curVike->keyCode;
        }
    }

    LOGIT(L"calc key is 0x%x\n", key);
    /* found key operation */
    if(pairs[key])
    {
        (this->*(pairs[key]))(curVike, (wxScintilla*)event.GetEventObject());
        return TRUE;
    }
    /* insert mode */
    else if(curVike->gMode == INSERT)
    {
        return FALSE;
    }
    /* unknow key operation */
    else
    {
        curVike->keyCodes = 0;
        return TRUE;
    }
}


//insert mode
void ViFunc::i_esc(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    vike->gMode = NORMAL;
    if(editor->GetCurrentPos() != editor->PositionFromLine(editor->GetCurrentLine()))
        editor->CharLeft();
    editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
}


//normal mode
void ViFunc::n_esc(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
}
void ViFunc::n_number(VikeWin *vike, wxScintilla* editor)
{
    vike->Number = vike->Number*10 + vike->keyCode - '0';
    LOGIT(_T("number now is %d"), vike->Number);
    vike->keyCodes = 0;
}
void ViFunc::n_tom(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = vike->keyCode;
}
void ViFunc::n_toma(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    wxString filename = getActiveFile();
    int markerid = vike->markers[filename];
    if(markerid) {
        int line = editor->MarkerLineFromHandle(markerid);
        editor->GotoLine(line);
    }
}
void ViFunc::n_m(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = vike->keyCode;
}
void ViFunc::n_a(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
//    wxString filename = getActiveFile();
//    int markerid = markers[filename];
//    if(markerid) editor->MarkerDeleteHandle(markerid);
//    markers[filename] = editor->MarkerAdd(editor->GetCurrentLine(),BOOKMARK_MARKER);
    vike->gMode = INSERT;
    if(editor->GetCurrentPos() != editor->GetLineEndPosition(editor->GetCurrentLine())){
        editor->CharRight();
    }
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
}
void ViFunc::n_A(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    vike->gMode = INSERT;
    editor->LineEnd();
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
}
void ViFunc::n_h(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    if(editor->GetCurrentPos() != editor->PositionFromLine(editor->GetCurrentLine()))
        editor->CharLeft();
}
void ViFunc::n_j(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->LineDown();
}
void ViFunc::n_k(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->LineUp();
}
void ViFunc::n_i(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    vike->gMode = INSERT;
    //editor->SetCaretForeground(wxColour(0x00, 0x00, 0x00));
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
}
void ViFunc::n_I(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->VCHome();
    vike->gMode = INSERT;
    //editor->SetCaretForeground(wxColour(0x00, 0x00, 0x00));
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
}
void ViFunc::n_l(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    if(editor->GetCurrentPos() != editor->GetLineEndPosition(editor->GetCurrentLine())){
        editor->CharRight();
    }
}
void ViFunc::n_0(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    if(vike->Number > 0) {
        vike->Number = vike->Number*10 + vike->keyCode - '0';
    } else {
        editor->Home();
    }
}
void ViFunc::n_sf_6(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->VCHome();
}
void ViFunc::n_sf_4(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->LineEnd();
}
void ViFunc::n_G(VikeWin *vike, wxScintilla* editor)
{
    if(vike->Number > 0) {
        editor->GotoLine(vike->Number-1);
    } else {
        editor->DocumentEnd();
    }
    vike->Number = 0;
    vike->keyCodes = 0;
}
void ViFunc::n_g(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = vike->keyCode;
}
void ViFunc::n_bktab(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = vike->keyCode;
}
void ViFunc::n_bktabbktab(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->BackTab();
}
void ViFunc::n_tab(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = vike->keyCode;
}
void ViFunc::n_tabtab(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->Tab();
}
void ViFunc::n_search(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
}
void ViFunc::n_ctrl_n(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
}
void ViFunc::n_ctrl_p(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
}
void ViFunc::n_o(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->LineEnd();
    editor->NewLine();
    n_i(vike, editor);
}
void ViFunc::n_O(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->VCHome();
    editor->NewLine();
    editor->LineUp();
    n_i(vike, editor);
}
void ViFunc::n_gg(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->DocumentStart();
}
void ViFunc::n_y(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = vike->keyCode;
}
void ViFunc::n_yy(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    vike->LineCuted = 1;
    editor->LineCopy();
}
void ViFunc::n_yw(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    vike->LineCuted = 0;
    int start_pos = editor->GetCurrentPos();
    int end_pos = editor->WordEndPosition(start_pos, TRUE);
    editor->CopyRange(start_pos, end_pos);
}
void ViFunc::n_p(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    int line;
    if(vike->LineCuted){
        editor->LineEnd();
        editor->NewLine();
        editor->Home();
        line = editor->GetCurrentLine();
    }
    editor->Paste();
    if(vike->LineCuted){
        editor->GotoLine(line);
        editor->VCHome();
    }
}
void ViFunc::n_P(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->LineUp();
    n_p(vike, editor);
}
void ViFunc::n_ctrl_f(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->PageDown();
}

void ViFunc::n_v(VikeWin *vike, wxScintilla* editor)
{
    vike->gMode = VISUAL;
}
void ViFunc::n_dsf_4(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->DelLineRight();
}
void ViFunc::n_ctrl_b(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->PageUp();
}
void ViFunc::n_b(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->WordLeft();
}
void ViFunc::n_w(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->WordRight();
}
void ViFunc::n_dw(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->DelWordRight();
}
void ViFunc::n_dd(VikeWin *vike, wxScintilla* editor)
{
    int number = vike->Number > 0 ? vike->Number : 1;
    vike->Number = 0;

    editor->Home();
    editor->SetSelectionStart(editor->GetCurrentPos());
    int start_line = editor->GetCurrentLine();
    int end_pos = editor->GetLineEndPosition(start_line + number - 1);
    editor->SetSelectionEnd(end_pos);
    editor->Cut();
    editor->DeleteBack();
    editor->CharRight();

    vike->keyCodes = 0;
    vike->LineCuted = 1;
}
void ViFunc::n_d(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = vike->keyCode;
}
void ViFunc::n_c(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = vike->keyCode;
}
void ViFunc::n_csf_4(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    n_dsf_4(vike, editor);
    n_i(vike, editor);
}
void ViFunc::n_cw(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    n_dw(vike, editor);
    n_i(vike, editor);
}
void ViFunc::n_cc(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    n_dd(vike, editor);
    n_O(vike, editor);
}
void ViFunc::n_r(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = vike->keyCode;
}
void ViFunc::n_r_any(VikeWin *vike, wxScintilla* editor)
{
    LOGIT(_T("r_any key %c\n"), vike->keyCode);
    vike->keyCodes = 0;
    editor->CharRightExtend();
    wxString *replace = new wxString((wxChar)vike->curChar);
    editor->ReplaceSelection(*replace);
    editor->CharLeft();
    delete replace;
}
void ViFunc::n_u(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->Undo();
}
void ViFunc::n_sf_5(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    int curpos = editor->GetCurrentPos();
    int bracepos = editor->BraceMatch(curpos);
    if(bracepos != -1)
    {
        editor->SetCurrentPos(bracepos);
        curpos < bracepos ? editor->SetSelectionStart(editor->GetSelectionEnd()) : editor->SetSelectionEnd(editor->GetSelectionStart());
    }
}
void ViFunc::n_x(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->CharRight();
    editor->DeleteBack();
}
void ViFunc::n_X(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->DeleteBack();
}
void ViFunc::n_ctrl_r(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->Redo();
}
//////////////////////////////////////////////////////////////////////

//visual mode
void ViFunc::v_esc(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    vike->gMode = NORMAL;
    //restore select extend
    editor->SetCurrentPos(editor->GetSelectionStart());
    editor->SetCaretForeground(wxColour(0xFF, 0x00, 0x00));
}
void ViFunc::v_h(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->CharLeftExtend();
}
void ViFunc::v_j(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->LineDownExtend();
}
void ViFunc::v_k(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->LineUpExtend();
}
void ViFunc::v_l(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->CharRightExtend();
}
void ViFunc::v_e(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->WordRightExtend();
}
void ViFunc::v_b(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->WordLeftExtend();
}
void ViFunc::v_sf_4(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->LineEndExtend();
}
void ViFunc::v_sf_6(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->VCHomeExtend();
}
void ViFunc::v_0(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->HomeExtend();
}
void ViFunc::v_bktab(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->BackTab();
}
void ViFunc::v_tab(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->Tab();
}
void ViFunc::v_y(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->Copy();
    editor->SetCurrentPos(editor->GetSelectionStart());
    vike->gMode = NORMAL;
}
void ViFunc::v_d(VikeWin *vike, wxScintilla* editor)
{
    vike->keyCodes = 0;
    editor->DeleteBack();
    vike->gMode = NORMAL;
}
//////////////////////////////////////////////////////////////////////
