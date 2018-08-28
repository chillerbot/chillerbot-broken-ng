/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_MENUS_H
#define GAME_CLIENT_COMPONENTS_MENUS_H

#include <base/vmath.h>
#include <base/tl/sorted_array.h>

#include <engine/demo.h>
#include <engine/friends.h>

#include <game/voting.h>
#include <game/client/component.h>


// compnent to fetch keypresses, override all other input
class CMenusKeyBinder : public CComponent
{
public:
	bool m_TakeKey;
	bool m_GotKey;
	IInput::CEvent m_Key;
	CMenusKeyBinder();
	virtual bool OnInput(IInput::CEvent Event);
};

class CMenus : public CComponent
{
	static vec4 ms_GuiColor;
	static vec4 ms_ColorTabbarInactiveOutgame;
	static vec4 ms_ColorTabbarActiveOutgame;
	static vec4 ms_ColorTabbarInactiveIngame;
	static vec4 ms_ColorTabbarActiveIngame;
	static vec4 ms_ColorTabbarInactive;
	static vec4 ms_ColorTabbarActive;

	vec4 ButtonColorMul(const void *pID);

	struct CListboxItem
	{
		int m_Visible;
		int m_Selected;
	};

	CListboxItem UiDoListboxNextItem(const void *pID, bool Selected = false, bool KeyEvents = true);
	CListboxItem UiDoListboxNextRow();
	int UiDoListboxEnd(float *pScrollValue, bool *pItemActivated);

	int m_GamePage;
	int m_Popup;
	int m_ActivePage;
	bool m_MenuActive;
	bool m_UseMouseButtons;
	vec2 m_MousePos;
	bool m_MouseSlow;

	int64 m_LastInput;

	// loading
	int m_LoadCurrent;
	int m_LoadTotal;

	//
	char m_aMessageTopic[512];
	char m_aMessageBody[512];
	char m_aMessageButton[512];

	void PopupMessage(const char *pTopic, const char *pBody, const char *pButton);

	// TODO: this is a bit ugly but.. well.. yeah
	enum { MAX_INPUTEVENTS = 32 };
	static IInput::CEvent m_aInputEvents[MAX_INPUTEVENTS];
	static int m_NumInputEvents;

	// some settings
	static float ms_ButtonHeight;
	static float ms_ListheaderHeight;
	static float ms_ListitemAdditionalHeight;
	static float ms_FontmodHeight;

	// for settings
	bool m_NeedRestartSkins;
	bool m_NeedRestartUpdate;
	bool m_NeedRestartDDNet;
	bool m_NeedSendinfo;
	bool m_NeedSendDummyinfo;
	int m_SettingPlayerPage;

	//
	bool m_EscapePressed;
	bool m_EnterPressed;
	bool m_DeletePressed;

	// for map download popup
	int64 m_DownloadLastCheckTime;
	int m_DownloadLastCheckSize;
	float m_DownloadSpeed;

	// for call vote
	int m_CallvoteSelectedOption;
	int m_CallvoteSelectedPlayer;
	char m_aCallvoteReason[VOTE_REASON_LENGTH];
	char m_aFilterString[25];

	// demo
	struct CDemoItem
	{
		char m_aFilename[128];
		char m_aName[128];
		bool m_IsDir;
		int m_StorageType;
		time_t m_Date;

		bool m_InfosLoaded;
		bool m_Valid;
		CDemoHeader m_Info;

		bool operator<(const CDemoItem &Other)
		{
			if (g_Config.m_BrDemoSort)
			{
				if (g_Config.m_BrDemoSortOrder)
				{
					return !str_comp(m_aFilename, "..") ? true : !str_comp(Other.m_aFilename, "..") ? false :
														m_IsDir && !Other.m_IsDir ? true : !m_IsDir && Other.m_IsDir ? false :
														m_Date < Other.m_Date;
				}
				else
				{
					return !str_comp(m_aFilename, "..") ? true : !str_comp(Other.m_aFilename, "..") ? false :
														m_IsDir && !Other.m_IsDir ? true : !m_IsDir && Other.m_IsDir ? false :
														m_Date > Other.m_Date;
				}
			}
			else
			{
				if (g_Config.m_BrDemoSortOrder)
				{
					return !str_comp(m_aFilename, "..") ? true : !str_comp(Other.m_aFilename, "..") ? false :
														m_IsDir && !Other.m_IsDir ? true : !m_IsDir && Other.m_IsDir ? false :
														str_comp_nocase(m_aFilename, Other.m_aFilename) < 0;
				}
				else
				{
					return !str_comp(m_aFilename, "..") ? true : !str_comp(Other.m_aFilename, "..") ? false :
														m_IsDir && !Other.m_IsDir ? true : !m_IsDir && Other.m_IsDir ? false :
														str_comp_nocase(m_aFilename, Other.m_aFilename) > 0;
				}
			}
		}
	};

	//sorted_array<CDemoItem> m_lDemos;
	char m_aCurrentDemoFolder[256];
	char m_aCurrentDemoFile[64];
	int m_DemolistSelectedIndex;
	bool m_DemolistSelectedIsDir;
	int m_DemolistStorageType;

	void DemolistOnUpdate(bool Reset);
	//void DemolistPopulate();
	static int DemolistFetchCallback(const char *pName, time_t Date, int IsDir, int StorageType, void *pUser);

	// friends
	struct CFriendItem
	{
		const CFriendInfo *m_pFriendInfo;
		int m_NumFound;

		bool operator<(const CFriendItem &Other)
		{
			if(m_NumFound && !Other.m_NumFound)
				return true;
			else if(!m_NumFound && Other.m_NumFound)
				return false;
			else
			{
				int Result = str_comp(m_pFriendInfo->m_aName, Other.m_pFriendInfo->m_aName);
				if(Result)
					return Result < 0;
				else
					return str_comp(m_pFriendInfo->m_aClan, Other.m_pFriendInfo->m_aClan) < 0;
			}
		}
	};

	sorted_array<CFriendItem> m_lFriends;
	int m_FriendlistSelectedIndex;

	void FriendlistOnUpdate();

	// found in menus.cpp
	int Render();

	// found in menus_browser.cpp
	int m_SelectedIndex;
	int m_DoubleClickIndex;
	int m_ScrollOffset;
	void RenderServerbrowserServerList();
	static void ConchainFriendlistUpdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);
	static void ConchainServerbrowserUpdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData);

	void SetActive(bool Active);
public:

	void UseMouseButtons(bool Use) { m_UseMouseButtons = Use; }

	static CMenusKeyBinder m_Binder;

	CMenus();

	bool IsActive() const { return m_MenuActive; }

	virtual void OnInit();

	virtual void OnStateChange(int NewState, int OldState);
	virtual void OnReset();
	virtual void OnRender();
	virtual bool OnInput(IInput::CEvent Event);
	virtual bool OnMouseMove(float x, float y);

	enum
	{
		PAGE_NEWS=1,
		PAGE_GAME,
		PAGE_PLAYERS,
		PAGE_SERVER_INFO,
		PAGE_CALLVOTE,
		PAGE_INTERNET,
		PAGE_LAN,
		PAGE_FAVORITES,
		PAGE_DDNET,
		PAGE_DEMOS,
		PAGE_SETTINGS,
		PAGE_SYSTEM,
		PAGE_NETWORK,
		PAGE_GHOST
	};

	// DDRace
	sorted_array<CDemoItem> m_lDemos;
	void DemolistPopulate();
	bool m_Dummy;

	// Ghost
	struct CGhostItem
	{
		char m_aFilename[256];
		char m_aPlayer[MAX_NAME_LENGTH];

		float m_Time;

		bool m_Active;
		int m_ID;

		bool operator<(const CGhostItem &Other) { return m_Time < Other.m_Time; }
		bool operator==(const CGhostItem &Other) { return m_ID == Other.m_ID; }
	};

	sorted_array<CGhostItem> m_lGhosts;
	CGhostItem *m_OwnGhost;
	void GhostlistPopulate();
	void setPopup(int Popup) { m_Popup = Popup; }

	int m_DemoPlayerState;
	char m_aDemoPlayerPopupHint[256];

	enum
	{
		POPUP_NONE=0,
		POPUP_FIRST_LAUNCH,
		POPUP_CONNECTING,
		POPUP_MESSAGE,
		POPUP_DISCONNECTED,
		POPUP_PURE,
		POPUP_LANGUAGE,
		POPUP_COUNTRY,
		POPUP_DELETE_DEMO,
		POPUP_RENAME_DEMO,
		POPUP_REMOVE_FRIEND,
		POPUP_PASSWORD,
		POPUP_QUIT,
		POPUP_DISCONNECT,

		// demo player states
		DEMOPLAYER_NONE=0,
		DEMOPLAYER_SLICE_SAVE,
	};

private:

	static int GhostlistFetchCallback(const char *pName, int IsDir, int StorageType, void *pUser);
};
#endif
