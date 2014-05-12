#ifndef __VIFUNC_H__
#define __VIFUNC_H__

#ifdef __GNUG__
#pragma interface "vifunc.h"
#endif

#include "debugging.h"

#include <wx/hashmap.h>

#define VIKE_INDIC_START 20

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
    VIKE_SEARCH,        // /
    VIKE_COMMAND,       // :
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

class VikeHighlight{
    public:
        VikeHighlight(): m_iIndicator(VIKE_INDIC_START)
        {
            m_colour = wxColor(255, 0, 0);
        }

        void Hide(wxScintilla *editor)
        {
            editor->SetIndicatorCurrent(m_iIndicator);
            editor->IndicatorClearRange(0, editor->GetLength());
        }

        void Show(wxScintilla *editor)
        {
            editor->SetIndicatorCurrent(m_iIndicator);
            editor->IndicatorSetStyle(m_iIndicator, wxSCI_INDIC_HIGHLIGHT);
            editor->IndicatorSetForeground(m_iIndicator, m_colour);
        }

        void SetColour(wxColour colour)
        {
            m_colour = colour;
        }

        void SetPos(int pos, int len, wxScintilla *editor)
        {
            editor->SetIndicatorCurrent(m_iIndicator);
            editor->IndicatorFillRange(pos, len);
        }

    private:
        int m_iIndicator;
        wxColour m_colour;
};

class VikeCommand{
    public:
        //VikeCommand(void){}
        VikeCommand(wxChar prefix, VikeHighlight &hl)
        : m_highlight(hl),
          m_cPrefix(prefix)
        {
            Clear();
        }

        void AppendCommand(wxChar cmdChar){ m_sCommand.Append(cmdChar); }
        void BackCommand(){ m_sCommand.RemoveLast(); }
        bool IsEmpty() { return m_sCommand.IsEmpty(); }
        wxChar GetPrefix() { return m_cPrefix; }

        wxString GetCommand(){
            this->SetPrefix();
            return m_sCommand.Mid(1);
        }

        wxString &GetCommandWithPrefix(){
            this->SetPrefix();
            return m_sCommand;
        }

        void Clear(){
            m_sCommand.Clear();
            m_sCommand.Prepend(m_cPrefix);
        }

        void Store(){}

    protected:
        VikeHighlight& m_highlight;

    private:
        void SetPrefix(){
            if(m_sCommand.Length() == 0 ||
               (m_sCommand.Length() > 0 && m_sCommand[0] != m_cPrefix))
            {
                m_sCommand.Prepend(m_cPrefix);
            }
        }

        wxChar m_cPrefix;
        wxString m_sCommand;
};

class VikeSearchCmd: public VikeCommand{
    public:
        //VikeSearchCmd(void) :VikeCommand(){}
        VikeSearchCmd(wxChar prefix, VikeHighlight &hl)
            : VikeCommand(prefix, hl),
              m_iLast(0),
              m_iMaxIndicator(VIKE_INDIC_START)
        {}

        // main search routine
        void doSearch(wxScintilla *editor){
            int curPos = editor->GetCurrentPos();
            bool found = StoreFind(0, editor->GetLength(), editor);
            if(found){
                editor->GotoPos(NextPos(curPos));
            }
        }

        int NextPos(int curPos)
        {
            return NeighborPos(curPos, true);
        }

        int PrevPos(int curPos)
        {
            return NeighborPos(curPos, false);
        }

    private:
        // Store all the found position
        bool StoreFind(int start, int end, wxScintilla *editor)
        {
            // Clear
            m_highlight.Hide(editor);

            bool found = false;
            int len;
            ClearState();
            int pos = start;
            do{
                pos = editor->FindText(pos + 1, end, this->GetCommand(), wxSCI_FIND_REGEXP|wxSCI_FIND_MATCHCASE, &len);
                if(pos >= 0){
                    found = true;
                    m_arrFind.push_back(pos);
                    // highlight
                    m_highlight.SetPos(pos, len, editor);
                }
                //LOGIT(_T("pos is %d"), pos);
            }while(pos >= 0);
            m_highlight.Show(editor);
            LOGIT(_T("find string length: %d"), len);
            return found;
        }

        int NeighborPos(int curPos, bool lookForward)
        {
            if(m_arrFind.GetCount() == 0){
                return curPos;
            }

            // Get next or previous position directly through m_iLast
            int step = lookForward ? 1 : -1;
            if(m_iLast == curPos){
                m_iLast = (m_iLast + step) % m_arrFind.GetCount();
                return m_arrFind[m_iLast];
            }

            // Find next or previous position by searching in m_arrFind
            int pos = curPos;
            if(lookForward){
                 for(int i = 0; i < m_arrFind.GetCount(); i++){
                    if(m_arrFind[i] > curPos){
                        pos = m_arrFind[i];
                        m_iLast = i;
                        break;
                    }
                }
            }else{
                for(int i = m_arrFind.GetCount() - 1; i >= 0; i--){
                    if(m_arrFind[i] < curPos){
                        pos = m_arrFind[i];
                        m_iLast = i;
                        break;
                    }
                }
            }
            // nothing find after curPos
            if(pos == curPos){
                pos = lookForward ? m_arrFind[0] : m_arrFind.Last();
            }
            return pos;
        }
        void ClearState()
        {
            m_iFindLen = 0;
            m_iLast = 0;
            m_iMaxIndicator = VIKE_INDIC_START;
            m_arrFind.Clear();
        }

        int m_iLast;
        int m_iFindLen;
        int m_iMaxIndicator;
        wxArrayInt m_arrFind;
};

class VikeWin;
typedef int (*VikeCmdHandler)(int argc, wxString *argv[], VikeWin *vike, wxScintilla *editor);
WX_DECLARE_STRING_HASH_MAP( VikeCmdHandler, VikeCmdMap);

class VikeCmdFunc{
    private:
        VikeCmdFunc()
        {
            m_cmdMap[_T("nohl")] = &nohl;
        };
        VikeCmdFunc(const VikeCmdFunc&){};
        VikeCmdFunc& operator=(const VikeCmdFunc&);
        static VikeCmdFunc m_pInstance;
        VikeCmdMap m_cmdMap;

        static int nohl(int argc, wxString *argv[], VikeWin *vike, wxScintilla *editor);

    public:
        static VikeCmdFunc* Instance()
        {
            return &m_pInstance;
        }

        VikeCmdHandler GetHandler(const wxString &cmd)
        {
            return m_cmdMap[cmd];
        }


};

class VikeGeneralCmd:public VikeCommand{
    public:
        VikeGeneralCmd(wxChar prefix, VikeHighlight &hl)
            :VikeCommand(prefix, hl)
        {
            m_pCmdFunc = VikeCmdFunc::Instance();
        }

        void ParseCommand(VikeWin *vike, wxScintilla *editor)
        {
            wxString cmd = GetCommand();
            int len = 10;
            wxString **argv = new wxString*[len];
            VikeCmdHandler handler = m_pCmdFunc->GetHandler(cmd);
            if(handler){
                handler(1, argv, vike, editor);
            }
        }

    private:
        VikeCmdFunc* m_pCmdFunc;

};

/* Every window has an instance of VikeWin */
class VikeWin{
    public:
        VikeWin(wxStatusBar *sb);

        bool OnChar(wxKeyEvent &event);
        bool OnKeyDown(wxKeyEvent &event);

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

        int GetUndoPos()            { return m_iCaretPos; }

        void SetModifier(int key)   { m_iModifier = key; }
        int GetMonifier() const     { return m_iModifier; }

        void ChangeMode(VikeMode new_mode)
        {
            m_iMode = new_mode;
            UpdateStatusBar();
        }

        int GetMode() const                 { return m_iMode; }

        void AppendKeyStatus(int key_code)  { m_arrKey.Add(key_code); }
        void ClearKeyStatus()               { m_arrKey.Clear(); }

        void UpdateStatusBar(){
            if(m_iMode == NORMAL && m_iState == VIKE_SEARCH){
                m_pStatusBar->SetStatusText(m_searchCmd.GetCommandWithPrefix(), STATUS_COMMAND);
                m_pStatusBar->SetStatusText(_T(""), STATUS_KEY);
            }else if(m_iMode == NORMAL && m_iState == VIKE_COMMAND){
                m_pStatusBar->SetStatusText(m_generalCmd.GetCommandWithPrefix(), STATUS_COMMAND);
                m_pStatusBar->SetStatusText(_T(""), STATUS_KEY);
            }else{
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

                if(m_arrKey.IsEmpty()){
                    m_pStatusBar->SetStatusText(_T(""), STATUS_KEY);
                }else{
                    m_pStatusBar->PushStatusText((wxChar)m_arrKey.Last(), STATUS_KEY);
                }
            }
        }

        void SetState(VikeState new_state)  { m_iState = new_state; }
        int GetState()                      { return m_iState; }

        void ShiftAddDupNumber(int num)     { m_iDupNumber = m_iDupNumber * 10 + num; }
        bool IsDup()                        { return m_iDupNumber != 0; }
        int GetDupNumber()                  { return m_iDupNumber == 0 ? 1 : m_iDupNumber; }

        VikeSearchCmd &GetSearchCmd()       { return m_searchCmd; }
        VikeGeneralCmd &GetGeneralCmd()     { return m_generalCmd; }
        VikeHighlight &GetHighlight()       { return m_highlight; }
        void ClearCmd()
        {
            m_searchCmd.Clear();
            //m_generalCmd.Clear();
        }
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

        VikeSearchCmd m_searchCmd;
        VikeGeneralCmd m_generalCmd;

        VikeHighlight m_highlight;

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
        bool SearchSp(VikeWin *vike, int keyCode, wxScintilla *editor);
        void Search(VikeWin *vike, int keyCode, wxScintilla *editor);
        void Command(VikeWin *vike, int keyCode, wxScintilla *editor);


        //insert mode
        void i_esc(VikeWin* m_pVike, wxScintilla* editor);

        //normal mode
        void n_esc(VikeWin* m_pVike, wxScintilla* editor);
        void n_enter(VikeWin* m_pVike, wxScintilla* editor);
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
