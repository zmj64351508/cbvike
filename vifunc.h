#ifndef __VIFUNC_H__
#define __VIFUNC_H__

#include "debugging.h"
#include "vicmd.h"
#include "cbstyledtextctrl.h"

#include <wx/statusbr.h>
#include <wx/hashmap.h>


#define UPPER_CODE(code) (code-'a'+'A')
#define LOWER_CODE(code) (code-'A'+'a')
#define MAKE_ORIGIN_CODE(code) UPPER_CODE(code)
#define MAKE_SHIFT_CODE(code) (code)
#define GET_LINE(code) (code>>8 & 0xffff)
#define GET_KEYCODE(code) (code & 0xff)
#define BOOKMARK_MARKER        2

/*! ViFunc has only one global instance called by VikeWin */
class ViFunc
{
    public:
        static ViFunc* Instance(){
            return &gViFunc;
        }

        //keycode process entry
        bool NormalKeyHandler(VikeWin *vike, wxKeyEvent &event);
        bool EspecialKeyHandler(VikeWin *vike, wxKeyEvent &event);

    private:
        /* not allow construct */
        ViFunc();
        ViFunc(const ViFunc&);
        ViFunc& operator=(const ViFunc&);

        /* singleton instance */
        static ViFunc gViFunc;

        /* temperoray hack for cut line should be a global flag for
           all the open editors */
        /* cut line or cut word */
        bool m_bLineCuted;

        /* Insert mode special key handler */
        bool InsertModeSp(VikeWin *vike, int keyCode, wxScintilla *editor);

        /* Normal mode special key handler */
        bool NormalModeSp(VikeWin *vike, int keyCode, int m_iModifier, wxScintilla *editor);

        /* Normal mode normal key handler */
        bool NormalMode(VikeWin *vike, int keyCode, int m_iModifier, wxScintilla *editor);

        /* Handles the commands start with '/' */
        void Search(VikeWin *vike, int keyCode, wxScintilla *editor);

        /* Handles the commands start with ':' */
        void Command(VikeWin *vike, int keyCode, wxScintilla *editor);

        /* Find the first unpair bracket and return the position */
        int FindUnpairBracket(wxChar bracketStart, bool lookForward, wxScintilla *editor);

        /* Select text wrapped with bracket given, returning the length of character selected if success
           and < 0 when fail */
        int SelectBracket(int bracket, wxScintilla* editor);

        /* Here are all the general functions for different keys */
        //insert mode
        void i_esc(VikeWin* vike, wxScintilla* editor);

        //normal mode
        bool n_esc(VikeWin* vike, wxScintilla* editor);
        void n_enter(VikeWin* vike, wxScintilla* editor);
        void n_backspace(VikeWin* vike, wxScintilla* editor);
        void n_number(VikeWin* vike, int number, wxScintilla* editor);
        void n_0(VikeWin* vike, wxScintilla* editor);
        void n_0_end(VikeWin* vike, wxScintilla* editor);

        /* move */
        void n_circumflex(VikeWin* vike, wxScintilla* editor);
        void n_dollar(VikeWin* vike, wxScintilla* editor);
        void n_h(VikeWin* vike, wxScintilla* editor);
        void n_j(VikeWin* vike, wxScintilla* editor);
        void n_k(VikeWin* vike, wxScintilla* editor);
        void n_l(VikeWin* vike, wxScintilla* editor);
        void n_circumflex_end(VikeWin* vike, wxScintilla* editor);
        void n_h_end(VikeWin* vike, wxScintilla* editor);
        void n_j_end(VikeWin* vike, wxScintilla* editor);
        void n_k_end(VikeWin* vike, wxScintilla* editor);
        void n_l_end(VikeWin* vike, wxScintilla* editor);
        void n_dollar_end(VikeWin* vike, wxScintilla* editor);

        void n_ctrl_f(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_b(VikeWin* vike, wxScintilla* editor);

        /* insert and append */
        void n_i(VikeWin* vike, wxScintilla* editor);
        void n_I(VikeWin* vike, wxScintilla* editor);
        void n_a(VikeWin* vike, wxScintilla* editor);
        void n_A(VikeWin* vike, wxScintilla* editor);
        void n_i_end(VikeWin* vike, wxScintilla* editor);
        void n_I_end(VikeWin* vike, wxScintilla* editor);
        void n_a_end(VikeWin *vike, wxScintilla* editor);
        void n_A_end(VikeWin* vike, wxScintilla* editor);

        /* new line */
        void n_o(VikeWin* vike, wxScintilla* editor);
        void n_O(VikeWin* vike, wxScintilla* editor);
        void n_o_end(VikeWin* vike, wxScintilla* editor);
        void n_O_end(VikeWin* vike, wxScintilla* editor);

        /* goto */
        void n_G(VikeWin* vike, wxScintilla* editor);
        void n_G_end(VikeWin *vike, wxScintilla* editor);
        void n_g(VikeWin* vike, wxScintilla* editor);
        void n_gg(VikeWin* vike, wxScintilla* editor);

        /* next and previous word */
        void n_b(VikeWin* vike, wxScintilla* editor);
        void n_b_end(VikeWin* vike, wxScintilla* editor);
        void n_w(VikeWin* vike, wxScintilla* editor);
        void n_w_end(VikeWin *vike, wxScintilla* editor);

        /* delete */
        void n_d(VikeWin* vike, wxScintilla* editor);
        void n_D(VikeWin* vike, wxScintilla* editor);
        void n_dw(VikeWin* vike, wxScintilla* editor);
        void n_ddollar_end(VikeWin* vike, wxScintilla* editor);
        void n_dd(VikeWin* vike, wxScintilla* editor);

        /* change */
        void n_c(VikeWin* vike, wxScintilla* editor);
        void n_C(VikeWin* vike, wxScintilla* editor);
        void n_cdollar_end(VikeWin *vike, wxScintilla* editor);
        void n_cw(VikeWin* vike, wxScintilla* editor);
        void n_cc(VikeWin* vike, wxScintilla* editor);
        void n_ci(VikeWin* vike, wxScintilla* editor);
        void n_ci_some(VikeWin* vike, int keyCode, wxScintilla* editor);

        /* replace */
        void n_r(VikeWin* vike, wxScintilla* editor);
        void n_r_any(VikeWin* vike, int keyCode, wxScintilla* editor);

        /* find */
        void n_f(VikeWin* vike, wxScintilla* editor);
        void n_f_any(VikeWin* vike, int keyCode, wxScintilla* editor);
        void n_F(VikeWin* vike, wxScintilla* editor);
        void n_F_any(VikeWin* vike, int keyCode, wxScintilla* editor);
        void n_n(VikeWin* vike, wxScintilla* editor);
        void n_n_end(VikeWin* vike, wxScintilla* editor);
        void n_N(VikeWin* vike, wxScintilla* editor);
        void n_N_end(VikeWin* vike, wxScintilla* editor);

        /* undo and redo */
        void n_u(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_r(VikeWin* vike, wxScintilla* editor);
        void n_u_end(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_r_end(VikeWin* vike, wxScintilla* editor);

        /* copy */
        void n_y(VikeWin* vike, wxScintilla* editor);
        void n_ydollar_end(VikeWin *vike, wxScintilla *editor);
        void n_yw(VikeWin* vike, wxScintilla* editor);
        void n_yy(VikeWin* vike, wxScintilla* editor);

        /* paste */
        void n_p(VikeWin* vike, wxScintilla* editor);
        void n_P(VikeWin* vike, wxScintilla* editor);
        void n_p_end(VikeWin* vike, wxScintilla* editor);
        void n_P_end(VikeWin* vike, wxScintilla* editor);

        /* cut */
        void n_x(VikeWin* vike, wxScintilla* editor);
        void n_X(VikeWin* vike, wxScintilla* editor);
        void n_x_end(VikeWin* vike, wxScintilla* editor);
        void n_X_end(VikeWin* vike, wxScintilla* editor);

        void n_sf_5(VikeWin* vike, wxScintilla* editor);

        void n_tab(VikeWin* vike, wxScintilla* editor);
        void n_tabtab(VikeWin* vike, wxScintilla* editor);
        void n_bktab(VikeWin* vike, wxScintilla* editor);
        void n_bktabbktab(VikeWin* vike, wxScintilla* editor);

        /* searh and command input */
        void n_search(VikeWin* vike, wxScintilla* editor);
        void n_command(VikeWin* vike, wxScintilla* editor);

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
