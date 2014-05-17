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

#include "vicmd.h"
#include "cbvike.h"
#include "vifunc.h"

/***************** VikeCommand start *********************/
VikeCommand::VikeCommand(wxChar prefix, VikeHighlight &hl)
    : m_highlight(hl),
      m_cPrefix(prefix)
{
    Clear();
}

void VikeCommand::AppendCommand(wxChar cmdChar)
{
    m_sCommand.Append(cmdChar);
}

void VikeCommand::BackCommand()
{
    m_sCommand.RemoveLast();
}

bool VikeCommand::IsEmpty()
{
    return m_sCommand.IsEmpty();
}

wxChar VikeCommand::GetPrefix()
{
    return m_cPrefix;
}

wxString VikeCommand::GetCommand()
{
    this->SetPrefix();
    return m_sCommand.Mid(1);
}

wxString &VikeCommand::GetCommandWithPrefix()
{
    this->SetPrefix();
    return m_sCommand;
}

void VikeCommand::Clear()
{
    m_sCommand.Clear();
    m_sCommand.Prepend(m_cPrefix);
}

void VikeCommand::Store()
{
}

void VikeCommand::SetPrefix()
{
    if(m_sCommand.Length() == 0 ||
       (m_sCommand.Length() > 0 && m_sCommand[0] != m_cPrefix))
    {
        m_sCommand.Prepend(m_cPrefix);
    }
}
/***************** VikeCommand end *********************/

/***************** VikeSearchCommand start *****************/
VikeSearchCmd::VikeSearchCmd(wxChar prefix, VikeHighlight &hl)
    : VikeCommand(prefix, hl),
      m_iLast(0),
      m_iMaxIndicator(VIKE_INDIC_START)
{}

void VikeSearchCmd::doSearch(wxScintilla *editor)
{
    int curPos = editor->GetCurrentPos();
    bool found = StoreFind(0, editor->GetLength(), editor);
    if(found){
        editor->GotoPos(NextPos(curPos));
    }
}

int VikeSearchCmd::NextPos(int curPos)
{
    return NeighborPos(curPos, true);
}

int VikeSearchCmd::PrevPos(int curPos)
{
    return NeighborPos(curPos, false);
}

bool VikeSearchCmd::StoreFind(int start, int end, wxScintilla *editor)
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

int VikeSearchCmd::NeighborPos(int curPos, bool lookForward)
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

void VikeSearchCmd::ClearState()
{
    m_iFindLen = 0;
    m_iLast = 0;
    m_iMaxIndicator = VIKE_INDIC_START;
    m_arrFind.Clear();
}
/***************** VikeSearchCommand end *****************/

/***************** VikeGeneralCommand start *****************/
VikeGeneralCmd::VikeGeneralCmd(wxChar prefix, VikeHighlight &hl)
    :VikeCommand(prefix, hl)
{
    m_pCmdFunc = VikeCmdFunc::Instance();
}

void VikeGeneralCmd::ParseCommand(VikeWin *vike, wxScintilla *editor)
{
    wxString cmd = GetCommand();
    int len = 10;
    wxString **argv = new wxString*[len];
    VikeCmdHandler handler = m_pCmdFunc->GetHandler(cmd);
    if(handler){
        handler(1, argv, vike, editor);
    }
}
/***************** VikeGeneralCommand end *****************/

/***************** VikeCmdFunc start *****************/
/* the instance */
VikeCmdFunc VikeCmdFunc::m_pInstance;

VikeCmdFunc::VikeCmdFunc()
{
    m_cmdMap[_T("nohl")] = &nohl;
}

VikeCmdFunc::VikeCmdFunc(const VikeCmdFunc&)
{
}

VikeCmdHandler VikeCmdFunc::GetHandler(const wxString &cmd)
{
    return m_cmdMap[cmd];
}

/* All the commands */
int VikeCmdFunc::nohl(int argc, wxString *argv[], VikeWin *vike, wxScintilla *editor)
{
    vike->GetHighlight().Hide(editor);
}
/***************** VikeCmdFunc end *****************/
