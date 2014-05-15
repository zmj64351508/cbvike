#ifndef __VIFUNC_H__
#define __VIFUNC_H__

#ifdef __GNUG__
#pragma interface "vifunc.h"
#endif

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
        bool NormalKeyHandler(VikeWin *curVike, wxKeyEvent &event);
        bool EspecialKeyHandler(VikeWin *curVike, wxKeyEvent &event);

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
        bool InsertModeSp(VikeWin *m_pVikeWin, int keyCode, wxScintilla *editor);

        /* Normal mode special key handler */
        bool NormalModeSp(VikeWin *m_pVikeWin, int keyCode, int m_iModifier, wxScintilla *editor);

        /* Normal mode normal key handler */
        bool NormalMode(VikeWin *m_pVikeWin, int keyCode, int m_iModifier, wxScintilla *editor);

        /* Handles the commands start with '/' */
        void Search(VikeWin *vike, int keyCode, wxScintilla *editor);

        /* Handles the commands start with ':' */
        void Command(VikeWin *vike, int keyCode, wxScintilla *editor);

        /* Here are all the general functions for different keys */
        //insert mode
        void i_esc(VikeWin* m_pVike, wxScintilla* editor);

        //normal mode
        void n_esc(VikeWin* m_pVike, wxScintilla* editor);
        void n_enter(VikeWin* m_pVike, wxScintilla* editor);
        void n_backspace(VikeWin* m_pVike, wxScintilla* editor);
        void n_number(VikeWin* m_pVike, int number, wxScintilla* editor);
        void n_0(VikeWin* m_pVike, wxScintilla* editor);
        void n_0_end(VikeWin* m_pVike, wxScintilla* editor);

        /* move */
        void n_circumflex(VikeWin* m_pVike, wxScintilla* editor);
        void n_dollar(VikeWin* m_pVike, wxScintilla* editor);
        void n_h(VikeWin* m_pVike, wxScintilla* editor);
        void n_j(VikeWin* m_pVike, wxScintilla* editor);
        void n_k(VikeWin* m_pVike, wxScintilla* editor);
        void n_l(VikeWin* m_pVike, wxScintilla* editor);
        void n_circumflex_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_h_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_j_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_k_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_l_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_dollar_end(VikeWin* m_pVike, wxScintilla* editor);

        void n_ctrl_f(VikeWin* m_pVike, wxScintilla* editor);
        void n_ctrl_b(VikeWin* m_pVike, wxScintilla* editor);

        /* insert and append */
        void n_i(VikeWin* m_pVike, wxScintilla* editor);
        void n_I(VikeWin* m_pVike, wxScintilla* editor);
        void n_a(VikeWin* m_pVike, wxScintilla* editor);
        void n_A(VikeWin* m_pVike, wxScintilla* editor);
        void n_i_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_I_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_a_end(VikeWin *m_pVike, wxScintilla* editor);
        void n_A_end(VikeWin* m_pVike, wxScintilla* editor);

        /* new line */
        void n_o(VikeWin* m_pVike, wxScintilla* editor);
        void n_O(VikeWin* m_pVike, wxScintilla* editor);
        void n_o_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_O_end(VikeWin* m_pVike, wxScintilla* editor);

        /* goto */
        void n_G(VikeWin* m_pVike, wxScintilla* editor);
        void n_G_end(VikeWin *m_pVike, wxScintilla* editor);
        void n_g(VikeWin* m_pVike, wxScintilla* editor);
        void n_gg(VikeWin* m_pVike, wxScintilla* editor);

        /* next and previous word */
        void n_b(VikeWin* m_pVike, wxScintilla* editor);
        void n_b_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_w(VikeWin* m_pVike, wxScintilla* editor);
        void n_w_end(VikeWin *m_pVike, wxScintilla* editor);

        /* delete */
        void n_d(VikeWin* m_pVike, wxScintilla* editor);
        void n_D(VikeWin* m_pVike, wxScintilla* editor);
        void n_dw(VikeWin* m_pVike, wxScintilla* editor);
        void n_ddollar_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_dd(VikeWin* m_pVike, wxScintilla* editor);

        /* change */
        void n_c(VikeWin* m_pVike, wxScintilla* editor);
        void n_cdollar_end(VikeWin *m_pVike, wxScintilla* editor);
        void n_cw(VikeWin* m_pVike, wxScintilla* editor);
        void n_cc(VikeWin* m_pVike, wxScintilla* editor);

        /* replace */
        void n_r(VikeWin* m_pVike, wxScintilla* editor);
        void n_r_any(VikeWin* m_pVike, int keyCode, wxScintilla* editor);

        /* find */
        void n_f(VikeWin* m_pVike, wxScintilla* editor);
        void n_f_any(VikeWin* m_pVike, int keyCode, wxScintilla* editor);
        void n_F(VikeWin* m_pVike, wxScintilla* editor);
        void n_F_any(VikeWin* m_pVike, int keyCode, wxScintilla* editor);
        void n_n(VikeWin* m_pVike, wxScintilla* editor);
        void n_n_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_N(VikeWin* m_pVike, wxScintilla* editor);
        void n_N_end(VikeWin* m_pVike, wxScintilla* editor);

        /* undo and redo */
        void n_u(VikeWin* m_pVike, wxScintilla* editor);
        void n_ctrl_r(VikeWin* m_pVike, wxScintilla* editor);
        void n_u_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_ctrl_r_end(VikeWin* m_pVike, wxScintilla* editor);

        /* copy */
        void n_y(VikeWin* m_pVike, wxScintilla* editor);
        void n_ydollar_end(VikeWin *m_pVike, wxScintilla *editor);
        void n_yw(VikeWin* m_pVike, wxScintilla* editor);
        void n_yy(VikeWin* m_pVike, wxScintilla* editor);

        /* paste */
        void n_p(VikeWin* m_pVike, wxScintilla* editor);
        void n_P(VikeWin* m_pVike, wxScintilla* editor);
        void n_p_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_P_end(VikeWin* m_pVike, wxScintilla* editor);

        /* cut */
        void n_x(VikeWin* m_pVike, wxScintilla* editor);
        void n_X(VikeWin* m_pVike, wxScintilla* editor);
        void n_x_end(VikeWin* m_pVike, wxScintilla* editor);
        void n_X_end(VikeWin* m_pVike, wxScintilla* editor);

        void n_sf_5(VikeWin* m_pVike, wxScintilla* editor);

        void n_tab(VikeWin* m_pVike, wxScintilla* editor);
        void n_tabtab(VikeWin* m_pVike, wxScintilla* editor);
        void n_bktab(VikeWin* m_pVike, wxScintilla* editor);
        void n_bktabbktab(VikeWin* m_pVike, wxScintilla* editor);

        /* searh and command input */
        void n_search(VikeWin* m_pVike, wxScintilla* editor);
        void n_command(VikeWin* m_pVike, wxScintilla* editor);

        void n_ctrl_n(VikeWin* m_pVike, wxScintilla* editor);
        void n_ctrl_p(VikeWin* m_pVike, wxScintilla* editor);
        void n_m(VikeWin* m_pVike, wxScintilla* editor);
        void n_tom(VikeWin* m_pVike, wxScintilla* editor);
        void n_toma(VikeWin* m_pVike, wxScintilla* editor);

        void n_v(VikeWin* m_pVike, wxScintilla* editor);
        //visual mode
        void v_esc(VikeWin* m_pVike, wxScintilla* editor);
        void v_h(VikeWin* m_pVike, wxScintilla* editor);
        void v_j(VikeWin* m_pVike, wxScintilla* editor);
        void v_k(VikeWin* m_pVike, wxScintilla* editor);
        void v_l(VikeWin* m_pVike, wxScintilla* editor);
        void v_e(VikeWin* m_pVike, wxScintilla* editor);
        void v_b(VikeWin* m_pVike, wxScintilla* editor);
        void v_sf_6(VikeWin* m_pVike, wxScintilla* editor);
        void v_0(VikeWin* m_pVike, wxScintilla* editor);
        void v_sf_4(VikeWin* m_pVike, wxScintilla* editor);
        void v_tab(VikeWin* m_pVike, wxScintilla* editor);
        void v_bktab(VikeWin* m_pVike, wxScintilla* editor);
        void v_y(VikeWin* m_pVike, wxScintilla* editor);
        void v_d(VikeWin* m_pVike, wxScintilla* editor);

};

#endif  //__VIFUNC_H__
