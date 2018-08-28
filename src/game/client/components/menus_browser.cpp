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


void CMenus::RenderServerbrowserServerList()
{

	struct CColumn
	{
		int m_ID;
		int m_Sort;
		CLocConstString m_Caption;
		int m_Direction;
		float m_Width;
		int m_Flags;
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
		{-1,			-1,						" ",		-1, 2.0f, 0},
		{COL_FLAG_LOCK,	-1,						" ",		-1, 14.0f, 0},
		{COL_FLAG_FAV,	-1,						" ",		-1, 14.0f, 0},
		{COL_NAME,		IServerBrowser::SORT_NAME,		"Name",		0, 50.0f, 0},	// Localize - these strings are localized within CLocConstString
		{COL_GAMETYPE,	IServerBrowser::SORT_GAMETYPE,	"Type",		1, 50.0f, 0},
		{COL_MAP,		IServerBrowser::SORT_MAP,			"Map", 		1, 100.0f + (480) / 8, 0},
		{COL_PLAYERS,	IServerBrowser::SORT_NUMPLAYERS,	"Players",	1, 60.0f, 0},
		{-1,			-1,						" ",		1, 10.0f, 0},
		{COL_PING,		IServerBrowser::SORT_PING,		"Ping",		1, 40.0f, FIXED},
	};
	// This is just for scripts/update_localization.py to work correctly (all other strings are already Localize()'d somewhere else). Don't remove!
	// Localize("Type");

	int NumCols = sizeof(s_aCols)/sizeof(CColumn);

	// do layout
	for(int i = 0; i < NumCols; i++)
	{
		if(s_aCols[i].m_Direction == -1)
		{

			if(i+1 < NumCols)
			{
				//Cols[i].flags |= SPACER;
			}
		}
	}

	for(int i = NumCols-1; i >= 0; i--)
	{
		if(s_aCols[i].m_Direction == 1)
		{
		}
	}

	for(int i = 0; i < NumCols; i++)
	{
		if (s_aCols[i].m_Direction == 0)
		{
		}
	}

	int NumServers = ServerBrowser()->NumSortedServers();

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


				m_SelectedIndex = NewIndex;

				const CServerInfo *pItem = ServerBrowser()->SortedGet(m_SelectedIndex);
				str_copy(g_Config.m_UiServerAddress, pItem->m_aAddress, sizeof(g_Config.m_UiServerAddress));
			}
		}
	}

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

		int Selected = str_comp(pItem->m_aAddress, g_Config.m_UiServerAddress) == 0; //selected_index==ItemIndex;

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
			char aTemp[64];

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
