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
        unsigned long key = calcKey(mode,ekey,lastkeycode,keycode); \
        pairs[key] = value; \
    }while(0)

#define MAKE_CODE(code) (code-'a'+65)
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

        // the executing function
        ViFunc *func;
        // capslock stat
        int CapsLock;
        // cut line or cut word
        int LineCuted;
        // current input character
        char curChar;
        //current edit mode
        eMODE gMode;
        // Duplicate number
        int Number;
        //current keycode
        int keyCode;
        //store last keycode temporaly
        int keyCodes;
        //marker register now set only one (a)
        StringHashMap markers;


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

        char ShiftKeyTable[0xFF] = {
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

        //store keycode maps
        LongHashMap pairs;

        unsigned long calcKey(
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
        void n_number(VikeWin* vike, wxScintilla* editor);
        void n_h(VikeWin* vike, wxScintilla* editor);
        void n_j(VikeWin* vike, wxScintilla* editor);
        void n_k(VikeWin* vike, wxScintilla* editor);
        void n_l(VikeWin* vike, wxScintilla* editor);
        void n_i(VikeWin* vike, wxScintilla* editor);
        void n_I(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_f(VikeWin* vike, wxScintilla* editor);
        void n_0(VikeWin* vike, wxScintilla* editor);
        void n_sf_6(VikeWin* vike, wxScintilla* editor);
        void n_sf_4(VikeWin* vike, wxScintilla* editor);
        void n_dsf_4(VikeWin* vike, wxScintilla* editor);
        void n_o(VikeWin* vike, wxScintilla* editor);
        void n_O(VikeWin* vike, wxScintilla* editor);
        void n_G(VikeWin* vike, wxScintilla* editor);
        void n_g(VikeWin* vike, wxScintilla* editor);
        void n_gg(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_b(VikeWin* vike, wxScintilla* editor);
        void n_b(VikeWin* vike, wxScintilla* editor);
        void n_w(VikeWin* vike, wxScintilla* editor);
        void n_dw(VikeWin* vike, wxScintilla* editor);
        void n_d(VikeWin* vike, wxScintilla* editor);
        void n_dd(VikeWin* vike, wxScintilla* editor);
        void n_c(VikeWin* vike, wxScintilla* editor);
        void n_csf_4(VikeWin *vike, wxScintilla* editor);
        void n_cw(VikeWin* vike, wxScintilla* editor);
        void n_cc(VikeWin* vike, wxScintilla* editor);
        void n_r(VikeWin* vike, wxScintilla* editor);
        void n_r_any(VikeWin* vike, wxScintilla* editor);

        void n_u(VikeWin* vike, wxScintilla* editor);
        void n_y(VikeWin* vike, wxScintilla* editor);
        void n_yw(VikeWin* vike, wxScintilla* editor);
        void n_yy(VikeWin* vike, wxScintilla* editor);
        void n_p(VikeWin* vike, wxScintilla* editor);
        void n_P(VikeWin* vike, wxScintilla* editor);
        void n_sf_5(VikeWin* vike, wxScintilla* editor);
        void n_x(VikeWin* vike, wxScintilla* editor);
        void n_X(VikeWin* vike, wxScintilla* editor);
        void n_v(VikeWin* vike, wxScintilla* editor);
        void n_tab(VikeWin* vike, wxScintilla* editor);
        void n_tabtab(VikeWin* vike, wxScintilla* editor);
        void n_bktab(VikeWin* vike, wxScintilla* editor);
        void n_bktabbktab(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_r(VikeWin* vike, wxScintilla* editor);
        void n_search(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_n(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_p(VikeWin* vike, wxScintilla* editor);
        void n_m(VikeWin* vike, wxScintilla* editor);
        void n_a(VikeWin* vike, wxScintilla* editor);
        void n_A(VikeWin* vike, wxScintilla* editor);
        void n_tom(VikeWin* vike, wxScintilla* editor);
        void n_toma(VikeWin* vike, wxScintilla* editor);

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
