#ifndef __VIFUNC_H__
#define __VIFUNC_H__

#include "debugging.h"
#include "vicmd.h"
#include "cbvike.h"
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


#define MOTION_LINE (1ul << 0) // motion for line like j k
#define MOTION_CHAR (0)        // motion for char like h l
#define MOTION_FORWARD (1ul << 1) // motion to forward like w
#define MOTION_BACKWARD (0)     // motion to backward like b
#define MOTION_INCLUDE_END (1ul << 2) // do command include motion end position like e

#define ASCII_MAX 255

/*! ViFunc has only one global instance called by VikeWin */
class ViFunc
{
    typedef void (ViFunc::*CommandType)(VikeWin *vike, wxScintilla *editor, int keyCode);
    typedef int (ViFunc::*EditCommandType)(VikeWin *vike, int startPos, int endPos, int flag, wxScintilla *editor);
    typedef void (ViFunc::*MotionCommandType)(VikeWin *vike, int dupNum, wxScintilla *editor, int keyCode);
    public:
        static ViFunc* Instance(){
            return &gViFunc;
        }

        /* Keycode process entry */
        bool NormalKeyHandler(VikeWin *vike, wxKeyEvent &event);
        bool EspecialKeyHandler(VikeWin *vike, wxKeyEvent &event);

    private:
        /* Store edit commands */
        EditCommandType m_arrOperatorCommand[VIKE_OPERATOR_END - VIKE_OPERATOR_START - 1];
        CommandType m_arrCommand[ASCII_MAX + 1];

        /* singleton instance */
        static ViFunc gViFunc;

        /* cut line or cut word */
        bool m_bLineCuted;

    private:
        /* not allow construct */
        ViFunc();
        ViFunc(const ViFunc&);
        ViFunc& operator=(const ViFunc&);

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

        void InitNormalCommand();
        void HandleCommand(VikeWin *vike, wxScintilla *editor, int keyCode);
        void n_invalid(VikeWin *vike, wxScintilla *editor, int keyCode)
        {
            vike->Finish(editor);
            vike->SetState(VIKE_INVALID);
        }

        /* the real handler for normal commands */
        int DoOperatorCommand(VikeWin *vike, int curState, int startPos, int endPos, int flag, wxScintilla *editor);
        void DoMotionCommand(VikeWin *vike, int dupNum, unsigned int flag, wxScintilla *editor, MotionCommandType motion, int keyCode = -1);

        /* Get duplicate number, maxLevel indicates the max position the commands can exist.
           eg. 12d2w -- in this command the maxLevel of 'w' is 2 while which of d is 1
           Generally speaking, the maxLevel of Motion Commands are often 2, and Edit Commands are 1 */
        int GetDupNum(VikeWin *vike, int maxLevel);

        /* handlers for the edit commands, called by DoEditCommad */
        int DoDelete(VikeWin *vike, int startPos, int endPos, int flag, wxScintilla *editor);
        int DoChange(VikeWin *vike, int startPos, int endPos, int flag, wxScintilla *editor);
        int DoYank(VikeWin *vike, int startPos, int endPos, int flag, wxScintilla *editor);
        int DummyOperatorCommand(VikeWin *vike, int startPos, int endPos, int flag, wxScintilla *editor);

        /************ Some help functions ***************/
        /* Find the first unpair bracket and return the position */
        int FindUnpairBracket(wxChar bracketStart, bool lookForward, wxScintilla *editor);

        /* Select text wrapped with bracket given, returning the length of character selected if success
           and < 0 when fail */
        int SelectBracket(int bracket, wxScintilla* editor);

        /* Find next character match position on current line, return >= 0 is success */
        int GotoCharCurrentLine(wxChar toFind, wxScintilla *editor, bool lookForward);

        /******** Here are all the general functions for different keys ********/
        //insert mode
        void i_esc(VikeWin* vike, wxScintilla* editor);

        //normal mode
        /****** special commands *****/
        bool n_esc(VikeWin* vike, wxScintilla* editor);
        void n_enter(VikeWin* vike, wxScintilla* editor);
        void n_backspace(VikeWin* vike, wxScintilla* editor);
        void n_number(VikeWin* vike, wxScintilla* editor, int keyCode);

        /****** motion comands *******/
        /* common move */
        void n_0(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_circumflex(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_dollar(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_h(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_j(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_k(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_l(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_0_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_h_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_j_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_k_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_l_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_dollar_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_circumflex_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /* goto */
        void n_G(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_g(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_G_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_gg(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /* next and previous word */
        void n_b(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_w(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_e(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_b_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_w_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_e_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /* find */
        void n_f(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_F(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_t(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_T(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_find(VikeWin *vike, wxScintilla *editor, VikeStateEnum state, int keyCode);
        void n_f_any(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_F_any(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_t_any(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_T_any(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /* next and previous */
        void n_n(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_N(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_n_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_N_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /****** operator comands *******/
        void n_d(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_c(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_y(VikeWin* vike, wxScintilla* editor, int keyCode);

        /****** other comands *******/
        /* insert and append */
        void n_i(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_I(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_a(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_A(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_i_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_I_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_a_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_A_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /* delete */
        void n_D(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_dd(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /* change */
        void n_C(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_ci(VikeWin* vike, wxScintilla* editor);
        void n_ci_some(VikeWin* vike, int keyCode, wxScintilla* editor);
        void n_cc(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /* copy */
        void n_Y(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_yy(VikeWin *vike, int dupNum, wxScintilla *editor, int keyCode);

        /* new line */
        void n_o(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_O(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_o_end(VikeWin *vike, int dupNum, wxScintilla *editor, int keyCode);
        void n_O_end(VikeWin *vike, int dupNum, wxScintilla *editor, int keyCode);

        /* replace */
        void n_r(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_r_any(VikeWin* vike, int keyCode, wxScintilla* editor);

        /* undo and redo */
        void n_u(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_ctrl_r(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_u_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_ctrl_r_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /* paste */
        void n_p(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_P(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_p_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_P_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        /* cut */
        void n_x(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_X(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_x_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);
        void n_X_end(VikeWin* vike, int dupNum, wxScintilla* editor, int keyCode);

        void n_z(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_zz(VikeWin *vike, int dupNum, wxScintilla *editor, int keyCode);
        void n_zt(VikeWin *vike, int dupNum, wxScintilla *editor, int keyCode);

        /******* extra commands ***********/
        void n_search(VikeWin* vike, wxScintilla* editor, int keyCode);
        void n_command(VikeWin* vike, wxScintilla* editor, int keyCode);

        /****** unused(developing) *******/
        void n_ctrl_f(VikeWin* vike, wxScintilla* editor);
        void n_ctrl_b(VikeWin* vike, wxScintilla* editor);
        void n_sf_5(VikeWin* vike, wxScintilla* editor);
        void n_tab(VikeWin* vike, wxScintilla* editor);
        void n_tabtab(VikeWin* vike, wxScintilla* editor);
        void n_bktab(VikeWin* vike, wxScintilla* editor);
        void n_bktabbktab(VikeWin* vike, wxScintilla* editor);
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
