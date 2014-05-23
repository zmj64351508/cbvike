#ifndef __CBVIKE_H__
#define __CBVIKE_H__

#include "cbplugin.h"
#include "cbvike.h"

//#define VERSION "0.2"

class wxWindow;
class wxLogWindow;

#if not defined EVT_EDITOR_SPLIT || not defined EVT_EDITOR_UNSPLIT
    class UnattachedPair
    {
        public:
            struct Pair{
                Pair(cbStyledTextCtrl *left, cbStyledTextCtrl *right, cbEditor *editor)
                    :m_pLeft(left), m_pRight(right), m_pEditor(editor)
                {}
                cbStyledTextCtrl *m_pLeft;
                cbStyledTextCtrl *m_pRight;
                cbEditor *m_pEditor;
            };

        public:
            void AddPair(cbStyledTextCtrl *left, cbStyledTextCtrl *right, cbEditor *editor)
            {
                if(FindPair(left) == nullptr){
                    m_arr.Add(new Pair(left, right, editor));
                }
            }

            Pair *FindPair(cbStyledTextCtrl *leftOrRight)
            {
                Pair *cur;
                for(int i = 0; i < m_arr.Count(); i++){
                    cur = (Pair*)m_arr[i];
                    if(cur->m_pLeft == leftOrRight || cur->m_pRight == leftOrRight){
                        return cur;
                    }
                }
                return nullptr;
            }

            void DeletePair(cbStyledTextCtrl *leftOrRight)
            {
                Pair *result = FindPair(leftOrRight);
                if(result){
                    m_arr.Remove(result);
                }
            }

            void DeletePair(Pair *toDelete)
            {
                m_arr.Remove(toDelete);
            }

        private:
            wxArrayPtrVoid m_arr;
    };
#endif


/*! The plugin, handling the general plugin action like attaching, releasing, etc. */
class VikePlugin : public cbPlugin
{
	public:
		VikePlugin();
		virtual ~VikePlugin();

        /**virtual**/
		void BuildMenu(wxMenuBar* menuBar);
		void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
		bool BuildToolBar(wxToolBar* toolBar){return FALSE;}
        /****/

		void OnAttach();
		void OnRelease(bool appShutDown);

      #ifdef LOGGING
        //for logging
        wxLogWindow* pMyLog;
      #endif

    private:
        void OnEditorOpen(CodeBlocksEvent& event);
        void OnEditorClose(CodeBlocksEvent& event);
        void OnEditorSplit(CodeBlocksEvent& event);
        void OnEditorUnsplit(CodeBlocksEvent& event);
        void OnAppStartupDone(CodeBlocksEvent& event);
        void OnWindowDestroy(wxWindowDestroyEvent& event);

        bool m_bBound;
        wxWindow*       pcbWindow;
        cbVike *pVike;

        #if not defined EVT_EDITOR_SPLIT || not defined EVT_EDITOR_UNSPLIT
            void OnWindowCreate(wxWindowCreateEvent &event);
            UnattachedPair m_UnattachedPair;
        #endif
};//class cbVike

#endif // __CBVIKE_H__
