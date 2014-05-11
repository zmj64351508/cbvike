#ifndef __VIFUNC_H__
#define __VIFUNC_H__

#ifdef __GNUG__
#pragma interface "vifunc.h"
#endif

#include "debugging.h"

#include <wx/hashmap.h>

#define UPPER_CODE(code) (code-'a'+'A')
#define LOWER_CODE(code) (code-'A'+'a')
#define MAKE_ORIGIN_CODE(code) UPPER_CODE(code)
#define MAKE_SHIFT_CODE(code) (code)
#define GET_LINE(code) (code>>8 & 0xffff)
#define GET_KEYCODE(code) (code & 0xff)
#define BOOKMARK_MARKER        2

enum VikeMode{ INSERT, NORMAL, VISUAL };
enum VikeState {
    VIKE_START,
    VIKE_REPLACE,       // r
    VIKE_FIND_FORWARD,  // f
    VIKE_FIND_BACKWORD, // F
    VIKE_CHANGE,        // c
    VIKE_DELETE,        // d
    VIKE_YANK,          // y
    VIKE_EXTRA,         // g
    VIKE_END,
};

enum VikeStatusBarField{
    STATUS_COMMAND,
    STATUS_KEY,
    STATUS_FIELD_NUM,
};

class ViFunc;

/* Every window has an instance of VikeWin */
class VikeWin{
    public:
        VikeWin(wxStatusBar *sb);

        bool NormalKeyHandler(wxKeyEvent &event);
        bool EspecialKeyHandler(wxKeyEvent &event);

        /* set caret when startup */
        void SetStartCaret(wxScintilla* editor) const
        {
            editor->SetCaretStyle(wxSCI_CARETSTYLE_BLOCK);
        }

        void Finish(wxScintilla *editor)
        {
            m_iDupNumber = 0;
            m_iState = VIKE_END;
            m_iCaretPos = editor->GetCurrentPos();
        }

        int GetUndoPos(){ return m_iCaretPos; }

        void SetModifier(int key){ m_iModifier = key; }
        int GetMonifier() const { return m_iModifier; }

        void ChangeMode(VikeMode new_mode)
        {
            m_iMode = new_mode;
            UpdateStatusBar();
        }

        int GetMode() const { return m_iMode; }

        void AppendKeyStatus(int key_code) { m_arrKey.Add(key_code); }
        void ClearKeyStatus() { m_arrKey.Clear(); }

        void UpdateStatusBar(){
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
            m_pStatusBar->SetStatusText(mode_txt, STATUS_COMMAND);

            wxString txt;
            if(!m_arrKey.IsEmpty()){
                LOGIT(_T("not empty"));
                for(int i = 0; i < m_arrKey.GetCount(); i++){
                    txt.Append(m_arrKey[i]);
                    //LOGIT(_T("%c"), key_array[i]);
                }
            }

            m_pStatusBar->SetStatusText(txt, STATUS_KEY);
        }

        void SetState(VikeState new_state){ m_iState = new_state; }
        int GetState(){ return m_iState; }

        void ShiftAddDupNumber(int num){ m_iDupNumber = m_iDupNumber * 10 + num; }
        bool IsDup(){ return m_iDupNumber != 0; }
        int GetDupNumber(){ return m_iDupNumber == 0 ? 1 : m_iDupNumber; }

        // the executing function
        ViFunc *func;
        // current input character
        char Char;

    private:
        //current edit mode
        VikeMode m_iMode;
        //current state
        VikeState m_iState;
        // Duplicate number
        wxStatusBar *m_pStatusBar;

        wxArrayInt m_arrKey;
        int m_iDupNumber;
        int m_iModifier;
        int m_iCaretPos;
};


/* ViFunc has only one global instance called by VikeWin */
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
        ViFunc();
        //not allow construct
        ViFunc(const ViFunc&);
        ViFunc& operator=(const ViFunc&);

        //singleton instance
        static ViFunc gViFunc;

        /* temperoray hack for cut line should be a global flag for
           all the open editors */
        // cut line or cut word
        bool m_bLineCuted;

        wxString getActiveFile()
        {
            cbEditor *ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
            if (!ed)
            {
                return wxT("");
            }

            return ed->GetFilename();
        }


        bool InsertModeSp(VikeWin *m_pVikeWin, int keyCode, wxScintilla *editor);
        bool NormalModeSp(VikeWin *m_pVikeWin, int keyCode, int m_iModifier, wxScintilla *editor);
        bool NormalMode(VikeWin *m_pVikeWin, int keyCode, int m_iModifier, wxScintilla *editor);

        //insert mode
        void i_esc(VikeWin* m_pVike, wxScintilla* editor);

        //normal mode
        void n_esc(VikeWin* m_pVike, wxScintilla* editor);
        void n_backspace(VikeWin* m_pVike, wxScintilla* editor);
        //void n_delete(VikeWin *vike, wxScintilla* editor);
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
        void n_search(VikeWin* m_pVike, wxScintilla* editor);
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
