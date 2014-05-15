#ifndef __CBVIKE_H__
#define __CBVIKE_H__

#if defined(__GNUG__)
	#pragma interface "cbvike.h"
#endif

#include "cbplugin.h"
#include "cbvike.h"

#define VERSION "0.9.1 2011/12/17"

class wxWindow;
class wxLogWindow;

/*! The plugin, handling the general plugin action like attaching, releasing, etc. */
class VikePlugin : public cbPlugin
{
	public:
		VikePlugin();
		virtual ~VikePlugin();

        /**virtual**/
		void BuildMenu(wxMenuBar* menuBar){
            //delete system editor hotkeys that conflict with vim
            delAccer(menuBar,_("&Search"),_("Find..."));
            delAccer(menuBar,_("&Search"),_("Replace..."));
            delAccer(menuBar,_("&Edit"),_("Bookmarks"),_("Toggle bookmark"));
        }
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
		bool BuildToolBar(wxToolBar* toolBar){return FALSE;}
        /****/

		void OnAttach();
		void OnRelease(bool appShutDown);

      #if LOGGING
        //for logging
        wxLogWindow* pMyLog;
      #endif

    private:
        bool m_bBound;
        void OnEditorOpen(CodeBlocksEvent& event);
        void OnEditorClose(CodeBlocksEvent& event);
        void AttachEditor(wxWindow* pEditor);
        void DetachEditor(wxWindow* pWindow, bool deleteEvtHandler = true);

        void OnAppStartupDone(CodeBlocksEvent& event);
        void OnAppStartShutdown(CodeBlocksEvent& event);

        void OnWindowCreateEvent(wxEvent& event);
        void OnWindowDestroyEvent(wxEvent& event);

        void delAccer(wxMenuBar* menuBar,wxString l1_menu,wxString l2_menu)
        {
            int pos = menuBar->FindMenu(l1_menu);
            if (pos != wxNOT_FOUND)
            {
                wxMenu* fm = menuBar->GetMenu(pos);
                int id = fm->FindItem(l2_menu);
                wxMenuItem* mn = fm->FindItem(id);
                if(mn != NULL) mn->SetItemLabel(mn->GetItemLabelText());
            }
        }

        void delAccer(wxMenuBar* menuBar,wxString l1_menu,wxString l2_menu,wxString l3_menu)
        {
            int pos = menuBar->FindMenu(l1_menu);
            if (pos != wxNOT_FOUND)
            {
                wxMenu* fm = menuBar->GetMenu(pos);
                int id = fm->FindItem(l2_menu);
                wxMenuItem* mn = fm->FindItem(id);
                if (mn != NULL && (fm = mn->GetSubMenu()))
                {
                    id = fm->FindItem(l3_menu);
                    mn = fm->FindItem(id);
                    if(mn != NULL) mn->SetItemLabel(mn->GetItemLabelText());
                }
            }
        }

        wxWindow*       pcbWindow;
        cbVike *pVike;
};//class cbVike

#endif // __CBVIKE_H__
