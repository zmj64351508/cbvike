#ifndef __VICMD_H__
#define __VICMD_H__

#include "cbstyledtextctrl.h"
#include "debugging.h"

#define VIKE_INDIC_START 20

class VikeHighlight;
class VikeWin;
class VikeCmdFunc;

/*! Command base */
class VikeCommand{
    public:
        //VikeCommand(void){}
        VikeCommand(wxChar prefix, VikeHighlight &hl);

        /* Append character to the command */
        void AppendCommand(wxChar cmdChar);

        /* Backspace a character from the command */
        void BackCommand();

        /* Whether the command is empty */
        bool IsEmpty();

        /* Get the prefix of the command */
        wxChar GetPrefix();

        /* Get the command without prefix */
        wxString GetCommand();

        /* Get the command with prefix */
        wxString &GetCommandWithPrefix();

        /* Clear the command */
        void Clear();

        /* Store the command in the buffer */
        void Store();

    protected:
        VikeHighlight& m_highlight;

    private:
        void SetPrefix();

        wxChar m_cPrefix;
        wxString m_sCommand;
};

/*! Search command start with '/' */
class VikeSearchCmd: public VikeCommand{
    public:
        VikeSearchCmd(wxChar prefix, VikeHighlight &hl);

        /* Main search routine */
        void doSearch(wxScintilla *editor);

        /* Return the position of next search result */
        int NextPos(int curPos);

        /* Return the position of previous search result */
        int PrevPos(int curPos);

    private:
        /* Store all the found position */
        bool StoreFind(int start, int end, wxScintilla *editor);

        /* Get the position of next or previous result */
        int NeighborPos(int curPos, bool lookForward);

        /* Clear search state */
        void ClearState();

        int m_iLast;
        int m_iFindLen;
        int m_iMaxIndicator;
        wxArrayInt m_arrFind;
};

/*! General Command start with ':' */
class VikeGeneralCmd:public VikeCommand{
    public:
        VikeGeneralCmd(wxChar prefix, VikeHighlight &hl);

        /* Parse and run a command */
        void ParseCommand(VikeWin *vike, wxScintilla *editor);

    private:
        VikeCmdFunc* m_pCmdFunc;
};

/* the command handler function */
typedef int (*VikeCmdHandler)(int argc, wxString *argv[], VikeWin *vike, wxScintilla *editor);
WX_DECLARE_STRING_HASH_MAP( VikeCmdHandler, VikeCmdMap);

/*! Stores all the command function */
class VikeCmdFunc{
    public:
        /* Singleton */
        static VikeCmdFunc* Instance()
        {
            return &m_pInstance;
        }

        /* Get the command handler through the command string */
        VikeCmdHandler GetHandler(const wxString &cmd);

    private:
        static VikeCmdFunc m_pInstance;
        VikeCmdMap m_cmdMap;

        VikeCmdFunc();
        VikeCmdFunc(const VikeCmdFunc&);
        VikeCmdFunc& operator=(const VikeCmdFunc&);

        /* All the commands */
        /* no hightlight */
        static int nohl(int argc, wxString *argv[], VikeWin *vike, wxScintilla *editor);
};

#endif // __VICMD_H__
