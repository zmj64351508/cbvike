#ifndef __VIFUNC_H__
#define __VIFUNC_H__

#ifdef __GNUG__
#pragma interface "vifunc.h"
#endif

#include <wx/hashmap.h>

class ViFunc;
class VikeWin;
typedef void (ViFunc::*vFunc)(VikeWin *, wxScintilla*);

#define MAKE_PAIRS(mode,ekey,lastkeycode,keycode,value) do{ \
        unsigned long int key = calcKey(mode,ekey,lastkeycode,keycode); \
        pairs[key] = value; \
    }while(0)

#define UPPER_CODE(code) (code-'a'+'A')
#define LOWER_CODE(code) (code-'A'+'a')
#define MAKE_SHIFT_CODE(code) (code)
#define GET_LINE(code) (code>>8 & 0xffff)
#define GET_KEYCODE(code) (code & 0xff)
#define BOOKMARK_MARKER        2

WX_DECLARE_HASH_MAP(unsigned long, vFunc, wxIntegerHash, wxIntegerEqual, LongHashMap);
WX_DECLARE_HASH_MAP( wxString, int, wxStringHash, wxStringEqual, StringHashMap);

enum eFKEY{ NONE=0x0, CTRL=0x1, SHIFT=0x2, ALT=0x4 };
enum eMODE{ INSERT, NORMAL, VISUAL };


/* Every window has a instance of VikeWin */
class VikeWin{
    public:
        VikeWin();

        int execute(wxKeyEvent &event);

        /* set caret when startup */
        void SetStartCaret(wxScintilla* p){
            p->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
        }

        void Finish(){
            Number = 0;
            keyCodes = 0;
        }

        void FinishNumber(){
            keyCodes = 0;
        }

        void PrepareNext(){
            keyCodes = keyCode;
        }

        void ShiftPrepareNext(){
            keyCodes = LOWER_CODE(keyCode);
        }

        void ChangeMode(eMODE mode){
            gMode = mode;
        }

        void SetKeyCode(int code){
            keyCode = code;
        }

        int GetKeyCode(){
            return keyCode;
        }

        int GetLastKeyCode(){
            return keyCodes;
        }

        eMODE GetCurrentMode(){
            return gMode;
        }

        void SetNumber(int num){
            Number = num;
        }

        int GetNumber(){
            return Number;
        }

        // the executing function
        ViFunc *func;
        // capslock stat
        int CapsLock;
        // current input character
        char curChar;
        //marker register now set only one (a)
        StringHashMap markers;

    private:
        //current edit mode
        eMODE gMode;
        // Duplicate number
        int Number;
        //current keycode
        int keyCode;
        //store last keycode temporaly
        int keyCodes;
};

/* ViFunc has only one global instance called by VikeWin */
class ViFunc
{
    public:
        static ViFunc* Instance(){
            return &gViFunc;
        }


        //keycode process entry
        bool execute(VikeWin *curVike, wxKeyEvent &event);

    private:
        ViFunc();
        //not allow construct
        ViFunc(const ViFunc&);
        ViFunc& operator=(const ViFunc&);

        //singleton instance
        static ViFunc gViFunc;

        /* temperoray hack for cut line should be a global flag for
           all the open editors */
        // cut line or cut word
        int LineCuted;

        static const char ShiftKeyTable[128];

        //store keycode maps
        LongHashMap pairs;

        unsigned long int calcKey(
                eMODE mode, //vim normal/insert/visual mode
                int ekey, //if shift/ctrl/alt keycodes have pressed
                unsigned char lastkeycode, //record last keycode pressed, like gg(go document home)
                unsigned char keycode //current keycode
                )
        {
            return (mode << 4 | ekey) <<16 | lastkeycode << 8 | keycode;
        }

        wxString getActiveFile()
        {
            cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
            if (!ed)
            {
                return wxT("");
            }

            return ed->GetFilename();
        }

        //insert mode
        void i_esc(VikeWin* vike, wxScintilla* editor);

        //normal mode
        void n_esc(VikeWin* vike, wxScintilla* editor);
        void n_delete(VikeWin *vike, wxScintilla* editor);
        void n_number(VikeWin* vike, wxScintilla* editor);
        void n_0(VikeWin* vike, wxScintilla* editor);

        /* move */
        void n_h(VikeWin* vike, wxScintilla* editor);
        void n_j(VikeWin* vike, wxScintilla* editor);
        void n_k(VikeWin* vike, wxScintilla* editor);
        void n_l(VikeWin* vike, wxScintilla* editor);
        void n_sf_6(VikeWin* vike, wxScintilla* editor);
        void n_sf_4(VikeWin* vike, wxScintilla* editor);

        void n_ctrl_f(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_b(VikeWin* vike, wxScintilla* editor);

        /* insert and append */
        void n_i(VikeWin* vike, wxScintilla* editor);
        void n_I(VikeWin* vike, wxScintilla* editor);
        void n_a(VikeWin* vike, wxScintilla* editor);
        void n_A(VikeWin* vike, wxScintilla* editor);

        /* new line */
        void n_o(VikeWin* vike, wxScintilla* editor);
        void n_O(VikeWin* vike, wxScintilla* editor);

        /* goto */
        void n_G(VikeWin* vike, wxScintilla* editor);
        void n_g(VikeWin* vike, wxScintilla* editor);
        void n_gg(VikeWin* vike, wxScintilla* editor);

        /* next and previous word */
        void n_b(VikeWin* vike, wxScintilla* editor);
        void n_w(VikeWin* vike, wxScintilla* editor);

        /* delete */
        void n_d(VikeWin* vike, wxScintilla* editor);
        void n_dw(VikeWin* vike, wxScintilla* editor);
        void n_dsf_4(VikeWin* vike, wxScintilla* editor);
        void n_dd(VikeWin* vike, wxScintilla* editor);

        /* change */
        void n_c(VikeWin* vike, wxScintilla* editor);
        void n_csf_4(VikeWin *vike, wxScintilla* editor);
        void n_cw(VikeWin* vike, wxScintilla* editor);
        void n_cc(VikeWin* vike, wxScintilla* editor);

        /* replace */
        void n_r(VikeWin* vike, wxScintilla* editor);
        void n_r_any(VikeWin* vike, wxScintilla* editor);

        /* find */
        void n_f(VikeWin* vike, wxScintilla* editor);
        void n_f_any(VikeWin* vike, wxScintilla* editor);
        void n_F(VikeWin* vike, wxScintilla* editor);
        void n_F_any(VikeWin* vike, wxScintilla* editor);

        /* undo and redo */
        void n_u(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_r(VikeWin* vike, wxScintilla* editor);

        /* copy */
        void n_y(VikeWin* vike, wxScintilla* editor);
        void n_yw(VikeWin* vike, wxScintilla* editor);
        void n_yy(VikeWin* vike, wxScintilla* editor);

        /* paste */
        void n_p(VikeWin* vike, wxScintilla* editor);
        void n_P(VikeWin* vike, wxScintilla* editor);

        /* cut */
        void n_x(VikeWin* vike, wxScintilla* editor);
        void n_X(VikeWin* vike, wxScintilla* editor);

        void n_sf_5(VikeWin* vike, wxScintilla* editor);

        void n_tab(VikeWin* vike, wxScintilla* editor);
        void n_tabtab(VikeWin* vike, wxScintilla* editor);
        void n_bktab(VikeWin* vike, wxScintilla* editor);
        void n_bktabbktab(VikeWin* vike, wxScintilla* editor);
        void n_search(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_n(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_p(VikeWin* vike, wxScintilla* editor);
        void n_m(VikeWin* vike, wxScintilla* editor);
        void n_tom(VikeWin* vike, wxScintilla* editor);
        void n_toma(VikeWin* vike, wxScintilla* editor);

        void n_v(VikeWin* vike, wxScintilla* editor);
        //visual mode
        void v_esc(VikeWin* vike, wxScintilla* editor);
        void v_h(VikeWin* vike, wxScintilla* editor);
        void v_j(VikeWin* vike, wxScintilla* editor);
        void v_k(VikeWin* vike, wxScintilla* editor);
        void v_l(VikeWin* vike, wxScintilla* editor);
        void v_e(VikeWin* vike, wxScintilla* editor);
        void v_b(VikeWin* vike, wxScintilla* editor);
        void v_sf_6(VikeWin* vike, wxScintilla* editor);
        void v_0(VikeWin* vike, wxScintilla* editor);
        void v_sf_4(VikeWin* vike, wxScintilla* editor);
        void v_tab(VikeWin* vike, wxScintilla* editor);
        void v_bktab(VikeWin* vike, wxScintilla* editor);
        void v_y(VikeWin* vike, wxScintilla* editor);
        void v_d(VikeWin* vike, wxScintilla* editor);

};



#endif  //__VIFUNC_H__
