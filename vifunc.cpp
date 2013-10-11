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

const char ViFunc::ShiftKeyTable[128] = {
    0,  // NUL
    1,  // SOH
    2,  // STX
    3,  // ETX
    4,  // EOT
    5,  // ENQ
    6,  // ACK
    7,  // BEL
    8,  // BS
    9,  // HT
    10, // LF
    11, // VT
    12, // FF
    13, // CR
    14, // SO
    15, // SI
    16, // DLE
    17, // DC1
    18, // DC2
    19, // DC3
    20, // DC4
    21, // NAK
    22, // SYN
    23, // TB
    24, // CAN
    25, // EM
    26, // SUB
    27, // ESC
    28, // FS
    29, // GS
    30, // RS
    31, // US
    ' ', // 32
    '!', // 33
    '"', // 34
    '#', // 35
    '$', // 36
    '%', // 37
    '&', // 38
    '"', // 39 ' -> "
    '(', // 40
    ')', // 41
    '*', // 42
    '+', // 43
    '<', // 44 , -> <
    '_', // 45 - -> _
    '>', // 46 . -> >
    '?', // 47 / -> ?
    ')', // 48 0 -> )
    '!', // 49 1 -> !
    '@', // 50 2 -> @
    '#', // 51 3 -> #
    '$', // 52 4 -> $
    '%', // 53 5 -> %
    '^', // 54 6 -> ^
    '&', // 55 7 -> &
    '*', // 56 8 -> *
    '(', // 57 9 -> (
    ':', // 58
    ':', // 59 ; -> :
    '<', // 60
    '+', // 61 = -> +
    '>', // 62
    '?', // 63
    '@', // 64
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    '{', // 91 [ -> {
    '|', // 92 \ -> |
    '}', // 93 ] -> }
    '^', // 94
    '_', // 95
    '~', // 96 ` -> ~
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    '{', // 123
    '|', // 124
    '}', // 125
    '~', // 126
    127, // DEL
};

ViFunc ViFunc::gViFunc;

VikeWin::VikeWin()
{
    LOGIT(_T("new VikeWin created"));
    //init some params
    gMode = NORMAL;

    keyCode = 0;
    keyCodes = 0;
    Number = 0;
    Char=0;

    func = ViFunc::Instance();
}
 int VikeWin::execute(wxKeyEvent &event){
    return func->execute(this, event);
}

ViFunc::ViFunc()
{
    LineCuted = 0;
    CapsLock = wxGetKeyState(WXK_CAPITAL);

    /************* Insert Mode ***************/
    MAKE_PAIRS(INSERT,NONE,0,27,&ViFunc::i_esc);//esc
    MAKE_PAIRS(INSERT,SHIFT,0,27,&ViFunc::i_esc);//esc
    MAKE_PAIRS(INSERT,CTRL,0,'[',&ViFunc::i_esc);//ctrl-[ same as esc

    /************* Normal Mode ***************/
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
    MAKE_PAIRS(NORMAL,NONE,0,WXK_DELETE,&ViFunc::n_delete);//delete

    /* move */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('h'),&ViFunc::n_h);//h
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('l'),&ViFunc::n_l);//l
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('j'),&ViFunc::n_j);//j
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('k'),&ViFunc::n_k);//k
    MAKE_PAIRS(NORMAL,NONE,0,WXK_LEFT,&ViFunc::n_h);//left
    MAKE_PAIRS(NORMAL,NONE,0,WXK_RIGHT,&ViFunc::n_l);//right
    MAKE_PAIRS(NORMAL,NONE,0,WXK_DOWN,&ViFunc::n_j);//down
    MAKE_PAIRS(NORMAL,NONE,0,WXK_UP,&ViFunc::n_k);//up
    MAKE_PAIRS(NORMAL,SHIFT,0,WXK_LEFT,&ViFunc::n_b);//left
    MAKE_PAIRS(NORMAL,SHIFT,0,WXK_RIGHT,&ViFunc::n_w);//right

    /* line move */
    MAKE_PAIRS(NORMAL,SHIFT,0,'6',&ViFunc::n_sf_6); //^
    MAKE_PAIRS(NORMAL,SHIFT,0,'4',&ViFunc::n_sf_4); //$
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_ORIGIN_CODE('g'),&ViFunc::n_G); //G
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('g'),&ViFunc::n_g); //g
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('g'),MAKE_ORIGIN_CODE('g'),&ViFunc::n_gg); //gg

    /* insert and append */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('i'),&ViFunc::n_i);//i
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_ORIGIN_CODE('i'),&ViFunc::n_I);//I
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('a'),&ViFunc::n_a); //a
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_ORIGIN_CODE('a'),&ViFunc::n_A); //A


    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_ORIGIN_CODE('f'),&ViFunc::n_ctrl_f); //ctrl-f
    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_ORIGIN_CODE('b'),&ViFunc::n_ctrl_b); //ctrl-b

    /* new line */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('o'),&ViFunc::n_o); //o
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_ORIGIN_CODE('o'),&ViFunc::n_O); //O

    /* next and previous word */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('w'),&ViFunc::n_w); //w
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('b'),&ViFunc::n_b); //b

    /* replace */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('r'),&ViFunc::n_r); //replace first key
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('r'),0,&ViFunc::n_r_any); //replace sencond key
    MAKE_PAIRS(NORMAL,SHIFT,MAKE_ORIGIN_CODE('r'),0,&ViFunc::n_r_any); //replace sencond key

    /* find */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('f'),&ViFunc::n_f); //find first key
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('f'),0,&ViFunc::n_f_any); //find sencond key
    MAKE_PAIRS(NORMAL,SHIFT,MAKE_ORIGIN_CODE('f'),0,&ViFunc::n_f_any); //find sencond key

    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_ORIGIN_CODE('f'),&ViFunc::n_F); //FIND first key
    MAKE_PAIRS(NORMAL,NONE,MAKE_SHIFT_CODE('f'),0,&ViFunc::n_F_any); //FIND sencond key
    MAKE_PAIRS(NORMAL,SHIFT,MAKE_SHIFT_CODE('f'),0,&ViFunc::n_F_any); //FIND sencond key

    /* delete */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('d'),&ViFunc::n_d); //d
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('d'),MAKE_ORIGIN_CODE('d'),&ViFunc::n_dd); //dd
    MAKE_PAIRS(NORMAL,SHIFT,MAKE_ORIGIN_CODE('d'),'4',&ViFunc::n_dsf_4); //d$
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('d'),MAKE_ORIGIN_CODE('w'),&ViFunc::n_dw); //dw
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_ORIGIN_CODE('d'),&ViFunc::n_dsf_4); //D

    /* change */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('c'),&ViFunc::n_c); //c
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('c'),MAKE_ORIGIN_CODE('4'),&ViFunc::n_csf_4); //c$
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('c'),MAKE_ORIGIN_CODE('w'),&ViFunc::n_cw); //cw
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('c'),MAKE_ORIGIN_CODE('c'),&ViFunc::n_cc); //cc

    /* undo and redo */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('u'),&ViFunc::n_u); //u
    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_ORIGIN_CODE('r'),&ViFunc::n_ctrl_r); //ctrl-r

    /* cut */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('x'),&ViFunc::n_x); //x
    MAKE_PAIRS(NORMAL,SHIFT,0,'5',&ViFunc::n_sf_5); //%
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_ORIGIN_CODE('x'),&ViFunc::n_X); //X

    /* copy */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('y'),&ViFunc::n_y); //y
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('y'),MAKE_ORIGIN_CODE('y'),&ViFunc::n_yy); //yy
    MAKE_PAIRS(NORMAL,NONE,MAKE_ORIGIN_CODE('y'),MAKE_ORIGIN_CODE('w'),&ViFunc::n_yw); //yw

    /* paste */
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('p'),&ViFunc::n_p); //p
    MAKE_PAIRS(NORMAL,SHIFT,0,MAKE_ORIGIN_CODE('p'),&ViFunc::n_P); //P

    MAKE_PAIRS(NORMAL,SHIFT,0,',',&ViFunc::n_bktab); //<
    MAKE_PAIRS(NORMAL,SHIFT,',',',',&ViFunc::n_bktabbktab); //<<
    MAKE_PAIRS(NORMAL,SHIFT,0,'.',&ViFunc::n_tab); //>
    MAKE_PAIRS(NORMAL,SHIFT,'.','.',&ViFunc::n_tabtab); //>>
    MAKE_PAIRS(NORMAL,NONE,0,'/',&ViFunc::n_search); //search / TODO
    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_ORIGIN_CODE('n'),&ViFunc::n_ctrl_n); //complete code ctrl-n TODO
    MAKE_PAIRS(NORMAL,CTRL,0,MAKE_ORIGIN_CODE('p'),&ViFunc::n_ctrl_p); //complete code ctrl-p TODO
    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('m'),&ViFunc::n_m); //m

    MAKE_PAIRS(NORMAL,NONE,0,'~',&ViFunc::n_tom); //`
    MAKE_PAIRS(NORMAL,NONE,'~',MAKE_ORIGIN_CODE('a'),&ViFunc::n_toma); //marker_a

    MAKE_PAIRS(NORMAL,NONE,0,MAKE_ORIGIN_CODE('v'),&ViFunc::n_v); //visual mode
    /************* Visual Mode ***************/
    MAKE_PAIRS(VISUAL,NONE,0,27,&ViFunc::v_esc);//esc
    MAKE_PAIRS(VISUAL,CTRL,0,'[',&ViFunc::v_esc);//ctrl-[ same as esc
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_ORIGIN_CODE('h'),&ViFunc::v_h);//h
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_ORIGIN_CODE('l'),&ViFunc::v_l);//l
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_ORIGIN_CODE('j'),&ViFunc::v_j);//j
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_ORIGIN_CODE('k'),&ViFunc::v_k);//k
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_ORIGIN_CODE('e'),&ViFunc::v_e);//e
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_ORIGIN_CODE('b'),&ViFunc::v_b);//b
    MAKE_PAIRS(VISUAL,SHIFT,0,'4',&ViFunc::v_sf_4);//$
    MAKE_PAIRS(VISUAL,SHIFT,0,'6',&ViFunc::v_sf_6);//^
    MAKE_PAIRS(VISUAL,NONE,0,'0',&ViFunc::v_0);//0
    MAKE_PAIRS(VISUAL,SHIFT,0,',',&ViFunc::v_bktab); //<
    MAKE_PAIRS(VISUAL,SHIFT,0,'.',&ViFunc::v_tab); //>
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_ORIGIN_CODE('y'),&ViFunc::v_y);//y
    MAKE_PAIRS(VISUAL,NONE,0,MAKE_ORIGIN_CODE('d'),&ViFunc::v_d);//d
}

//if return FALSE,not process here,leave next.
//
bool ViFunc::execute(VikeWin *curVike, wxKeyEvent &event)
{
    int fkey = NONE;
    int keyCode = event.GetKeyCode();
    curVike->SetKeyCode(keyCode);
    LOGIT(wxT("vifunc::execute - customer key event,keycode: [%d]"), keyCode);

    /* CapsLock */
    if(keyCode == 311){
        CapsLock = !CapsLock;
        return FALSE;
    }

    /* Generate fkey */
    fkey = GenerateFunctionalKey(event);

    /* Pressing Shift but no other key pressed */
    if(keyCode == WXK_SHIFT && event.m_shiftDown) return TRUE;

    unsigned long key;
    eMODE curMode = curVike->GetCurrentMode();
    int lastKey = curVike->GetLastKeyCode();

    /* If entered r or f then accept all the next key */
    if(GET_KEYCODE(lastKey) == MAKE_ORIGIN_CODE('r') ||
       GET_KEYCODE(lastKey) == MAKE_ORIGIN_CODE('f') ||
       GET_KEYCODE(lastKey) == MAKE_SHIFT_CODE('f'))
    {
        key = calcKey(curMode,fkey,GET_KEYCODE(lastKey),0);
    }else{
        key = calcKey(curMode,fkey,GET_KEYCODE(lastKey),GET_KEYCODE(keyCode));
    }

    /* Generate ASCII charactor by key code and SHIFT */
    curVike->Char = MakeChar(fkey, keyCode);

    LOGIT(L"calc key is 0x%x\n", key);
    /* found key operation */
    if(pairs[key]){
        (this->*(pairs[key]))(curVike, (wxScintilla*)event.GetEventObject());
        return TRUE;
    }
    /* insert mode */
    else if(curMode == INSERT){
        return FALSE;
    }
    /* unknow key operation */
    else{
        curVike->Finish();
        return TRUE;
    }
}


//insert mode
void ViFunc::i_esc(VikeWin *vike, wxScintilla* editor)
{
    vike->ChangeMode(NORMAL);
    if(editor->GetCurrentPos() != editor->PositionFromLine(editor->GetCurrentLine()))
        editor->CharLeft();
    editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
    vike->Finish();
}

//normal mode
void ViFunc::n_esc(VikeWin *vike, wxScintilla* editor)
{
    editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
    vike->Finish();
}
void ViFunc::n_delete(VikeWin *vike, wxScintilla* editor)
{
    if(editor->GetSelectionStart() == editor->GetSelectionEnd()){
        editor->CharRight();
    }
    editor->DeleteBack();
    vike->Finish();
}
void ViFunc::n_number(VikeWin *vike, wxScintilla* editor)
{
    int num = vike->GetNumber() * 10 + vike->GetKeyCode() - '0';
    vike->SetNumber(num);
    vike->FinishNumber();
    LOGIT(_T("number now is %d"), num);
}
void ViFunc::n_tom(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_toma(VikeWin *vike, wxScintilla* editor)
{
    wxString filename = getActiveFile();
    int markerid = vike->markers[filename];
    if(markerid) {
        int line = editor->MarkerLineFromHandle(markerid);
        editor->GotoLine(line);
    }
    vike->Finish();
}
void ViFunc::n_m(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_a(VikeWin *vike, wxScintilla* editor)
{
//    wxString filename = getActiveFile();
//    int markerid = markers[filename];
//    if(markerid) editor->MarkerDeleteHandle(markerid);
//    markers[filename] = editor->MarkerAdd(editor->GetCurrentLine(),BOOKMARK_MARKER);
    vike->ChangeMode(INSERT);
    if(editor->GetCurrentPos() != editor->GetLineEndPosition(editor->GetCurrentLine())){
        editor->CharRight();
    }
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
    vike->Finish();
}
void ViFunc::n_A(VikeWin *vike, wxScintilla* editor)
{
    vike->ChangeMode(INSERT);
    editor->LineEnd();
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
    vike->Finish();
}
void ViFunc::n_h(VikeWin *vike, wxScintilla* editor)
{
    if(editor->GetCurrentPos() != editor->PositionFromLine(editor->GetCurrentLine()))
        editor->CharLeft();
    vike->Finish();
}
void ViFunc::n_j(VikeWin *vike, wxScintilla* editor)
{
    editor->LineDown();
    vike->Finish();
}
void ViFunc::n_k(VikeWin *vike, wxScintilla* editor)
{
    editor->LineUp();
    vike->Finish();
}
void ViFunc::n_i(VikeWin *vike, wxScintilla* editor)
{
    vike->ChangeMode(INSERT);
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
    vike->Finish();
}
void ViFunc::n_I(VikeWin *vike, wxScintilla* editor)
{
    editor->VCHome();
    vike->ChangeMode(INSERT);
    editor->SetCaretStyle(wxSCI_CARETSTYLE_LINE);
    vike->Finish();
}
void ViFunc::n_l(VikeWin *vike, wxScintilla* editor)
{
    if(editor->GetCurrentPos() != editor->GetLineEndPosition(editor->GetCurrentLine())){
        editor->CharRight();
    }
    vike->Finish();
}
void ViFunc::n_0(VikeWin *vike, wxScintilla* editor)
{
    int num = vike->GetNumber();
    if(num > 0) {
        num = num * 10 + vike->GetKeyCode() - '0';
        vike->SetNumber(num);
    } else {
        editor->Home();
    }
    vike->FinishNumber();
}
void ViFunc::n_sf_6(VikeWin *vike, wxScintilla* editor)
{
    editor->VCHome();
    vike->Finish();
}
void ViFunc::n_sf_4(VikeWin *vike, wxScintilla* editor)
{
    editor->LineEnd();
    vike->Finish();
}
void ViFunc::n_G(VikeWin *vike, wxScintilla* editor)
{
    int num = vike->GetNumber();
    if(num > 0) {
        editor->GotoLine(num - 1);
    } else {
        editor->DocumentEnd();
    }
    vike->Finish();
}
void ViFunc::n_g(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_bktab(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_bktabbktab(VikeWin *vike, wxScintilla* editor)
{
    editor->BackTab();
    vike->Finish();
}
void ViFunc::n_tab(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_tabtab(VikeWin *vike, wxScintilla* editor)
{
    editor->Tab();
    vike->Finish();
}
void ViFunc::n_search(VikeWin *vike, wxScintilla* editor)
{
    vike->Finish();
}
void ViFunc::n_ctrl_n(VikeWin *vike, wxScintilla* editor)
{
    vike->Finish();
}
void ViFunc::n_ctrl_p(VikeWin *vike, wxScintilla* editor)
{
    vike->Finish();
}
void ViFunc::n_o(VikeWin *vike, wxScintilla* editor)
{
    editor->LineEnd();
    editor->NewLine();
    n_i(vike, editor);
    vike->Finish();
}
void ViFunc::n_O(VikeWin *vike, wxScintilla* editor)
{
    editor->VCHome();
    editor->NewLine();
    editor->LineUp();
    n_i(vike, editor);
    vike->Finish();
}
void ViFunc::n_gg(VikeWin *vike, wxScintilla* editor)
{
    editor->DocumentStart();
    vike->Finish();
}
void ViFunc::n_y(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_yy(VikeWin *vike, wxScintilla* editor)
{
    int num = vike->GetNumber();
    num = num > 0 ? num : 1;
    int curPos = editor->GetCurrentPos();

    editor->Home();
    editor->SetSelectionStart(editor->GetCurrentPos());
    int start_line = editor->GetCurrentLine();
    int end_pos = editor->GetLineEndPosition(start_line + num - 1);
    editor->SetSelectionEnd(end_pos);
    editor->Copy();

    editor->GotoPos(curPos);
    LineCuted = 1;
    vike->Finish();
}
void ViFunc::n_yw(VikeWin *vike, wxScintilla* editor)
{
    LineCuted = 0;
    int num = vike->GetNumber();
    num = num > 0 ? num : 1;
    int curPos = editor->GetCurrentPos();
    for(;num > 0; num--){
        editor->WordRight();
    }
    editor->SetSelectionStart(curPos);
    editor->Copy();
    editor->GotoPos(curPos);
    vike->Finish();
}
void ViFunc::n_p(VikeWin *vike, wxScintilla* editor)
{
    int line;
    int curPos;

    if(LineCuted){
        editor->LineEnd();
        editor->NewLine();
        editor->Home();
        line = editor->GetCurrentLine();
    }else{
        n_l(vike, editor);
        curPos = editor->GetCurrentPos();
    }

    editor->Paste();

    if(LineCuted){
        editor->GotoLine(line);
        editor->VCHome();
    }else{
        editor->GotoPos(curPos);
    }

    vike->Finish();
}
void ViFunc::n_P(VikeWin *vike, wxScintilla* editor)
{
    if(LineCuted){
        editor->LineUp();
    }else{
        n_h(vike, editor);
    }
    n_p(vike, editor);
    vike->Finish();
}
void ViFunc::n_ctrl_f(VikeWin *vike, wxScintilla* editor)
{
    editor->PageDown();
    vike->Finish();
}

void ViFunc::n_v(VikeWin *vike, wxScintilla* editor)
{
    vike->ChangeMode(VISUAL);
    editor->SetCaretForeground(wxColour(0xFF, 0x00, 0x00));
    vike->Finish();
}
void ViFunc::n_dsf_4(VikeWin *vike, wxScintilla* editor)
{
    editor->DelLineRight();
    vike->Finish();
}
void ViFunc::n_ctrl_b(VikeWin *vike, wxScintilla* editor)
{
    editor->PageUp();
    vike->Finish();
}
void ViFunc::n_b(VikeWin *vike, wxScintilla* editor)
{
    editor->WordLeft();
    vike->Finish();
}
void ViFunc::n_w(VikeWin *vike, wxScintilla* editor)
{
    editor->WordRight();
    vike->Finish();
}
void ViFunc::n_dw(VikeWin *vike, wxScintilla* editor)
{
    int num = vike->GetNumber();
    num = num > 0 ? num : 1;

    int curPos = editor->GetCurrentPos();
    for(;num > 0; num--){
        editor->WordRight();
    }
    editor->SetSelectionStart(curPos);
    editor->Cut();
    editor->SetCurrentPos(curPos);

    vike->Finish();
}
void ViFunc::n_dd(VikeWin *vike, wxScintilla* editor)
{
    int num = vike->GetNumber();
    num = num > 0 ? num : 1;

    editor->Home();
    editor->SetSelectionStart(editor->GetCurrentPos());
    int start_line = editor->GetCurrentLine();
    int end_pos = editor->GetLineEndPosition(start_line + num - 1);
    editor->SetSelectionEnd(end_pos);
    editor->Cut();
    editor->DeleteBack();
    editor->CharRight();

    LineCuted = 1;
    vike->Finish();
}
void ViFunc::n_d(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_c(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_csf_4(VikeWin *vike, wxScintilla* editor)
{
    n_dsf_4(vike, editor);
    n_i(vike, editor);
    vike->Finish();
}
void ViFunc::n_cw(VikeWin *vike, wxScintilla* editor)
{
    n_dw(vike, editor);
    n_i(vike, editor);
    vike->Finish();
}
void ViFunc::n_cc(VikeWin *vike, wxScintilla* editor)
{
    n_dd(vike, editor);
    n_O(vike, editor);
    vike->Finish();
}
void ViFunc::n_r(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_r_any(VikeWin *vike, wxScintilla* editor)
{
    LOGIT(_T("r_any key %c\n"), vike->GetKeyCode());
    editor->CharRightExtend();
    wxString replace((wxChar)vike->Char);
    editor->ReplaceSelection(replace);
    editor->CharLeft();
    delete replace;

    vike->Finish();
}
void ViFunc::n_f(VikeWin *vike, wxScintilla* editor)
{
    vike->PrepareNext();
}
void ViFunc::n_f_any(VikeWin *vike, wxScintilla* editor)
{
    LOGIT(_T("f_any key %c\n"), vike->Char);
    wxString toFind((wxChar)vike->Char);

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
    vike->Finish();
}
void ViFunc::n_F(VikeWin *vike, wxScintilla* editor)
{
    vike->ShiftPrepareNext();
}
void ViFunc::n_F_any(VikeWin *vike, wxScintilla* editor)
{
    LOGIT(_T("F_any key %c\n"), vike->Char);
    wxString *toFind = new wxString((wxChar)vike->Char);

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
    delete toFind;
    vike->Finish();
}
void ViFunc::n_u(VikeWin *vike, wxScintilla* editor)
{
    editor->Undo();
    vike->Finish();
}
void ViFunc::n_sf_5(VikeWin *vike, wxScintilla* editor)
{
    int curpos = editor->GetCurrentPos();
    int bracepos = editor->BraceMatch(curpos);
    if(bracepos != -1)
    {
        editor->SetCurrentPos(bracepos);
        curpos < bracepos ? editor->SetSelectionStart(editor->GetSelectionEnd()) : editor->SetSelectionEnd(editor->GetSelectionStart());
    }
    vike->Finish();
}
void ViFunc::n_x(VikeWin *vike, wxScintilla* editor)
{
    editor->CharRight();
    editor->DeleteBack();
    vike->Finish();
}
void ViFunc::n_X(VikeWin *vike, wxScintilla* editor)
{
    editor->DeleteBack();
    vike->Finish();
}
void ViFunc::n_ctrl_r(VikeWin *vike, wxScintilla* editor)
{
    editor->Redo();
    vike->Finish();
}
//////////////////////////////////////////////////////////////////////

//visual mode
void ViFunc::v_esc(VikeWin *vike, wxScintilla* editor)
{
    vike->ChangeMode(NORMAL);
    //restore select extend
    editor->SetCurrentPos(editor->GetSelectionStart());
    editor->SetCaretForeground(wxColour(0x00, 0x00, 0x00));
    vike->Finish();
}
void ViFunc::v_h(VikeWin *vike, wxScintilla* editor)
{
    editor->CharLeftExtend();
    vike->Finish();
}
void ViFunc::v_j(VikeWin *vike, wxScintilla* editor)
{
    editor->LineDownExtend();
    vike->Finish();
}
void ViFunc::v_k(VikeWin *vike, wxScintilla* editor)
{
    editor->LineUpExtend();
    vike->Finish();
}
void ViFunc::v_l(VikeWin *vike, wxScintilla* editor)
{
    editor->CharRightExtend();
    vike->Finish();
}
void ViFunc::v_e(VikeWin *vike, wxScintilla* editor)
{
    editor->WordRightExtend();
    vike->Finish();
}
void ViFunc::v_b(VikeWin *vike, wxScintilla* editor)
{
    editor->WordLeftExtend();
    vike->Finish();
}
void ViFunc::v_sf_4(VikeWin *vike, wxScintilla* editor)
{
    editor->LineEndExtend();
    vike->Finish();
}
void ViFunc::v_sf_6(VikeWin *vike, wxScintilla* editor)
{
    editor->VCHomeExtend();
    vike->Finish();
}
void ViFunc::v_0(VikeWin *vike, wxScintilla* editor)
{
    editor->HomeExtend();
    vike->Finish();
}
void ViFunc::v_bktab(VikeWin *vike, wxScintilla* editor)
{
    editor->BackTab();
    vike->Finish();
}
void ViFunc::v_tab(VikeWin *vike, wxScintilla* editor)
{
    editor->Tab();
    vike->Finish();
}
void ViFunc::v_y(VikeWin *vike, wxScintilla* editor)
{
    editor->Copy();
    editor->SetCurrentPos(editor->GetSelectionStart());
    vike->ChangeMode(NORMAL);
    vike->Finish();
}
void ViFunc::v_d(VikeWin *vike, wxScintilla* editor)
{
    editor->DeleteBack();
    vike->ChangeMode(NORMAL);
    vike->Finish();
}
//////////////////////////////////////////////////////////////////////
