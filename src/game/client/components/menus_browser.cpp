/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/config.h>
#include <engine/friends.h>
#include <engine/keys.h>
#include <engine/serverbrowser.h>

#include <engine/updater.h>
#include <engine/shared/config.h>

#include <game/generated/client_data.h>
#include <game/generated/protocol.h>

#include <game/localization.h>
#include <game/version.h>
#include <game/client/components/countryflags.h>
#include <game/client/components/console.h>

#include <stdio.h> //ChillerDragon printf for serverbrowser

#include "menus.h"


void CMenus::RenderServerbrowserServerList(CUIRect View)
{
	CUIRect Headers;
	CUIRect Status;

	View.HSplitTop(ms_ListheaderHeight, &Headers, &View);
	View.HSplitBottom(28.0f, &View, &Status);

	struct CColumn
	{
		int m_ID;
		int m_Sort;
		CLocConstString m_Caption;
		int m_Direction;
		float m_Width;
		int m_Flags;
		CUIRect m_Rect;
		CUIRect m_Spacer;
	};

	enum
	{
		FIXED=1,
		SPACER=2,

		COL_FLAG_LOCK=0,
		COL_FLAG_FAV,
		COL_NAME,
		COL_GAMETYPE,
		COL_MAP,
		COL_PLAYERS,
		COL_PING,
		COL_VERSION,
	};

	CColumn s_aCols[] = {
		{-1,			-1,						" ",		-1, 2.0f, 0, {0}, {0}},
		{COL_FLAG_LOCK,	-1,						" ",		-1, 14.0f, 0, {0}, {0}},
		{COL_FLAG_FAV,	-1,						" ",		-1, 14.0f, 0, {0}, {0}},
		{COL_NAME,		IServerBrowser::SORT_NAME,		"Name",		0, 50.0f, 0, {0}, {0}},	// Localize - these strings are localized within CLocConstString
		{COL_GAMETYPE,	IServerBrowser::SORT_GAMETYPE,	"Type",		1, 50.0f, 0, {0}, {0}},
		{COL_MAP,		IServerBrowser::SORT_MAP,			"Map", 		1, 100.0f + (Headers.w - 480) / 8, 0, {0}, {0}},
		{COL_PLAYERS,	IServerBrowser::SORT_NUMPLAYERS,	"Players",	1, 60.0f, 0, {0}, {0}},
		{-1,			-1,						" ",		1, 10.0f, 0, {0}, {0}},
		{COL_PING,		IServerBrowser::SORT_PING,		"Ping",		1, 40.0f, FIXED, {0}, {0}},
#if defined(__ANDROID__)
		{-1,			-1,						" ",		1, 50.0f, 0, {0}, {0}}, // Scrollbar
#endif
	};
	// This is just for scripts/update_localization.py to work correctly (all other strings are already Localize()'d somewhere else). Don't remove!
	// Localize("Type");

	int NumCols = sizeof(s_aCols)/sizeof(CColumn);

	// do layout
	for(int i = 0; i < NumCols; i++)
	{
		if(s_aCols[i].m_Direction == -1)
		{
			Headers.VSplitLeft(s_aCols[i].m_Width, &s_aCols[i].m_Rect, &Headers);

			if(i+1 < NumCols)
			{
				//Cols[i].flags |= SPACER;
				Headers.VSplitLeft(2, &s_aCols[i].m_Spacer, &Headers);
			}
		}
	}

	for(int i = NumCols-1; i >= 0; i--)
	{
		if(s_aCols[i].m_Direction == 1)
		{
			Headers.VSplitRight(s_aCols[i].m_Width, &Headers, &s_aCols[i].m_Rect);
			Headers.VSplitRight(2, &Headers, &s_aCols[i].m_Spacer);
		}
	}

	for(int i = 0; i < NumCols; i++)
	{
		if(s_aCols[i].m_Direction == 0)
			s_aCols[i].m_Rect = Headers;
	}

	// do headers
	for(int i = 0; i < NumCols; i++)
	{
		if(DoButton_GridHeader(s_aCols[i].m_Caption, s_aCols[i].m_Caption, g_Config.m_BrSort == s_aCols[i].m_Sort, &s_aCols[i].m_Rect))
		{
			if(s_aCols[i].m_Sort != -1)
			{
				if(g_Config.m_BrSort == s_aCols[i].m_Sort)
					g_Config.m_BrSortOrder ^= 1;
				else
					g_Config.m_BrSortOrder = 0;
				g_Config.m_BrSort = s_aCols[i].m_Sort;
			}
		}
	}

	CUIRect Scroll;
#if defined(__ANDROID__)
	View.VSplitRight(50, &View, &Scroll);
#else
	View.VSplitRight(15, &View, &Scroll);
#endif

	int NumServers = ServerBrowser()->NumSortedServers();

	int Num = (int)(View.h/s_aCols[0].m_Rect.h) + 1;
	static int s_ScrollBar = 0;
	static float s_ScrollValue = 0;

	Scroll.HMargin(5.0f, &Scroll);
	s_ScrollValue = DoScrollbarV(&s_ScrollBar, &Scroll, s_ScrollValue);

	int ScrollNum = NumServers-Num+1;
	if(ScrollNum > 0)
	{
		if(m_ScrollOffset >= 0)
		{
			s_ScrollValue = (float)(m_ScrollOffset)/ScrollNum;
			m_ScrollOffset = -1;
		}
		if(Input()->KeyPress(KEY_MOUSE_WHEEL_UP) && UI()->MouseInside(&View))
			s_ScrollValue -= 3.0f/ScrollNum;
		if(Input()->KeyPress(KEY_MOUSE_WHEEL_DOWN) && UI()->MouseInside(&View))
			s_ScrollValue += 3.0f/ScrollNum;
	}
	else
		ScrollNum = 0;

	if(m_SelectedIndex > -1)
	{
		for(int i = 0; i < m_NumInputEvents; i++)
		{
			int NewIndex = -1;
			if(m_aInputEvents[i].m_Flags&IInput::FLAG_PRESS)
			{
				if(m_aInputEvents[i].m_Key == KEY_DOWN) NewIndex = m_SelectedIndex + 1;
				if(m_aInputEvents[i].m_Key == KEY_UP) NewIndex = m_SelectedIndex - 1;
			}
			if(NewIndex > -1 && NewIndex < NumServers)
			{
				//scroll
				float IndexY = View.y - s_ScrollValue*ScrollNum*s_aCols[0].m_Rect.h + NewIndex*s_aCols[0].m_Rect.h;
				int Scroll = View.y > IndexY ? -1 : View.y+View.h < IndexY+s_aCols[0].m_Rect.h ? 1 : 0;
				if(Scroll)
				{
					if(Scroll < 0)
					{
						int NumScrolls = (View.y-IndexY+s_aCols[0].m_Rect.h-1.0f)/s_aCols[0].m_Rect.h;
						s_ScrollValue -= (1.0f/ScrollNum)*NumScrolls;
					}
					else
					{
						int NumScrolls = (IndexY+s_aCols[0].m_Rect.h-(View.y+View.h)+s_aCols[0].m_Rect.h-1.0f)/s_aCols[0].m_Rect.h;
						s_ScrollValue += (1.0f/ScrollNum)*NumScrolls;
					}
				}

				m_SelectedIndex = NewIndex;

				const CServerInfo *pItem = ServerBrowser()->SortedGet(m_SelectedIndex);
				str_copy(g_Config.m_UiServerAddress, pItem->m_aAddress, sizeof(g_Config.m_UiServerAddress));
			}
		}
	}

	if(s_ScrollValue < 0) s_ScrollValue = 0;
	if(s_ScrollValue > 1) s_ScrollValue = 1;

#if defined(__ANDROID__)
	int DoubleClicked = 0;
#endif
	int NumPlayers = 0;

	// reset friend counter
	for(int i = 0; i < m_lFriends.size(); m_lFriends[i++].m_NumFound = 0);

	for (int i = 0; i < NumServers; i++)
	{
		int ItemIndex = i;
		const CServerInfo *pItem = ServerBrowser()->SortedGet(ItemIndex);
		NumPlayers += g_Config.m_BrFilterSpectators ? pItem->m_NumPlayers : pItem->m_NumClients;
		CUIRect Row;
		CUIRect SelectHitBox;

		int Selected = str_comp(pItem->m_aAddress, g_Config.m_UiServerAddress) == 0; //selected_index==ItemIndex;

		View.HSplitTop(ms_ListheaderHeight, &Row, &View);
		SelectHitBox = Row;

		if(Selected)
			m_SelectedIndex = i;

		// update friend counter
		if(pItem->m_FriendState != IFriends::FRIEND_NO)
		{
			for(int j = 0; j < pItem->m_NumClients; ++j)
			{
				if(pItem->m_aClients[j].m_FriendState != IFriends::FRIEND_NO)
				{
					unsigned NameHash = str_quickhash(pItem->m_aClients[j].m_aName);
					unsigned ClanHash = str_quickhash(pItem->m_aClients[j].m_aClan);
					for(int f = 0; f < m_lFriends.size(); ++f)
					{
						if(((g_Config.m_ClFriendsIgnoreClan && m_lFriends[f].m_pFriendInfo->m_aName[0]) || ClanHash == m_lFriends[f].m_pFriendInfo->m_ClanHash) &&
							(!m_lFriends[f].m_pFriendInfo->m_aName[0] || NameHash == m_lFriends[f].m_pFriendInfo->m_NameHash))
						{
							m_lFriends[f].m_NumFound++;
							if(m_lFriends[f].m_pFriendInfo->m_aName[0])
								break;
						}
					}
				}
			}
		}

		for(int c = 0; c < NumCols; c++)
		{
			CUIRect Button;
			char aTemp[64];
			Button.x = s_aCols[c].m_Rect.x;
			Button.y = Row.y;
			Button.h = Row.h;
			Button.w = s_aCols[c].m_Rect.w;

			int ID = s_aCols[c].m_ID;

			if(ID == COL_FLAG_LOCK)
			{
				if (pItem->m_Flags & SERVER_FLAG_PASSWORD)
				{
					//printf("[<3]");
				}
			}
			else if(ID == COL_FLAG_FAV)
			{
				if (pItem->m_Favorite)
				{
					//printf("[<3]");
				}
			}
			else if(ID == COL_NAME)
			{
				if(g_Config.m_BrFilterString[0] && (pItem->m_QuickSearchHit&IServerBrowser::QUICK_SERVERNAME))
				{
					//dbg_msg("browser", "%s", pItem->m_aName);
				}
				else
				{
					//dbg_msg("browser", "%s", pItem->m_aName);
				}
			}
			else if(ID == COL_MAP)
			{
				if(g_Config.m_BrFilterString[0] && (pItem->m_QuickSearchHit&IServerBrowser::QUICK_MAPNAME))
				{
					//dbg_msg("browser", "%s", pItem->m_aMap);
				}
				else
				{
					//dbg_msg("browser", "%s", pItem->m_aMap);
				}
			}
			else if(ID == COL_PLAYERS)
			{
				CUIRect Icon;
				Button.VMargin(4.0f, &Button);
				if(pItem->m_FriendState != IFriends::FRIEND_NO)
				{
					Button.VSplitLeft(Button.h, &Icon, &Button);
					Icon.Margin(2.0f, &Icon);
					DoButton_Icon(IMAGE_BROWSEICONS, SPRITE_BROWSE_HEART, &Icon);
				}

				if(g_Config.m_BrFilterSpectators)
					str_format(aTemp, sizeof(aTemp), "%i/%i", pItem->m_NumPlayers, pItem->m_MaxPlayers);
				else
					str_format(aTemp, sizeof(aTemp), "%i/%i", pItem->m_NumClients, pItem->m_MaxClients);
			}
			else if(ID == COL_PING)
			{
				str_format(aTemp, sizeof(aTemp), "%i", pItem->m_Latency);
			}
			else if(ID == COL_VERSION)
			{
				//const char *pVersion = pItem->m_aVersion;
			}
			else if(ID == COL_GAMETYPE)
			{
				if (g_Config.m_UiColorizeGametype)
				{
					vec3 hsl = vec3(1.0f, 1.0f, 1.0f);

					if (IsVanilla(pItem))
						hsl = vec3(0.33f, 1.0f, 0.75f);
					else if (IsCatch(pItem))
						hsl = vec3(0.17f, 1.0f, 0.75f);
					else if (IsInsta(pItem))
						hsl = vec3(0.00f, 1.0f, 0.75f);
					else if (IsFNG(pItem))
						hsl = vec3(0.83f, 1.0f, 0.75f);
					else if (IsDDNet(pItem))
						hsl = vec3(0.58f, 1.0f, 0.75f);
					else if (IsDDRace(pItem))
						hsl = vec3(0.75f, 1.0f, 0.75f);
					else if (IsRace(pItem))
						hsl = vec3(0.46f, 1.0f, 0.75f);

					//dbg_msg("browser", "gametype: %s", pItem->m_aGameType)
				}
				else
				{
					// no colors
					//dbg_msg("browser", "gametype: %s", pItem->m_aGameType)
				}
			}
		}
	}

	// render status
	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), Localize("%d of %d servers, %d players"), ServerBrowser()->NumSortedServers(), ServerBrowser()->NumServers(), NumPlayers);
}

void CMenus::RenderServerbrowserFilters(CUIRect View)
{

}

void CMenus::RenderServerbrowserServerDetail(CUIRect View)
{

}

void CMenus::FriendlistOnUpdate()
{
	m_lFriends.clear();
	for(int i = 0; i < m_pClient->Friends()->NumFriends(); ++i)
	{
		CFriendItem Item;
		Item.m_pFriendInfo = m_pClient->Friends()->GetFriend(i);
		Item.m_NumFound = 0;
		m_lFriends.add_unsorted(Item);
	}
	m_lFriends.sort_range();
}

void CMenus::RenderServerbrowserFriends(CUIRect View)
{

}

void CMenus::RenderServerbrowser(CUIRect MainView)
{
	/*
		+-----------------+	+-------+
		|				  |	|		|
		|				  |	| tool	|
		|   server list	  |	| box 	|
		|				  |	|	  	|
		|				  |	|		|
		+-----------------+	|	 	|
			status box	tab	+-------+
	*/
}

void CMenus::ConchainFriendlistUpdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->NumArguments() == 2 && ((CMenus *)pUserData)->Client()->State() == IClient::STATE_OFFLINE)
	{
		((CMenus *)pUserData)->FriendlistOnUpdate();
		((CMenus *)pUserData)->Client()->ServerBrowserUpdate();
	}
}

void CMenus::ConchainServerbrowserUpdate(IConsole::IResult *pResult, void *pUserData, IConsole::FCommandCallback pfnCallback, void *pCallbackUserData)
{
	pfnCallback(pResult, pCallbackUserData);
	if(pResult->NumArguments() && (g_Config.m_UiPage == PAGE_FAVORITES || g_Config.m_UiPage == PAGE_DDNET) && ((CMenus *)pUserData)->Client()->State() == IClient::STATE_OFFLINE)
		((CMenus *)pUserData)->ServerBrowser()->Refresh(IServerBrowser::TYPE_FAVORITES);
}
