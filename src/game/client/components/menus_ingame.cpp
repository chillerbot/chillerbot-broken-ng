/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/math.h>

#include <engine/config.h>
#include <engine/demo.h>
#include <engine/friends.h>
#include <engine/serverbrowser.h>
#include <engine/shared/config.h>

#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/client/animstate.h>
#include <game/client/gameclient.h>

#include "menus.h"
#include "motd.h"
#include "voting.h"

#include <base/tl/string.h>
#include <engine/storage.h>
#include "ghost.h"

void CMenus::RenderGame(CUIRect MainView)
{

}

void CMenus::RenderPlayers(CUIRect MainView)
{

}

void CMenus::RenderServerInfo(CUIRect MainView)
{

}

bool CMenus::RenderServerControlServer(CUIRect MainView)
{
	return false;
}

bool CMenus::RenderServerControlKick(CUIRect MainView, bool FilterSpectators)
{
	return false;
}

void CMenus::RenderServerControl(CUIRect MainView)
{

}

void CMenus::RenderInGameNetwork(CUIRect MainView)
{
	return;
}

// ghost stuff
int CMenus::GhostlistFetchCallback(const char *pName, int IsDir, int StorageType, void *pUser)
{
	CMenus *pSelf = (CMenus *)pUser;
	int Length = str_length(pName);
	if((pName[0] == '.' && (pName[1] == 0 ||
		(pName[1] == '.' && pName[2] == 0))) ||
		(!IsDir && (Length < 4 || str_comp(pName+Length-4, ".gho"))))
		return 0;

	CGhost::CGhostHeader Header;
	if(!pSelf->m_pClient->m_pGhost->GetInfo(pName, &Header))
		return 0;

	CGhostItem Item;
	str_copy(Item.m_aFilename, pName, sizeof(Item.m_aFilename));
	str_copy(Item.m_aPlayer, Header.m_aOwner, sizeof(Item.m_aPlayer));
	Item.m_Time = Header.m_Time;
	Item.m_Active = false;
	Item.m_ID = pSelf->m_lGhosts.add(Item);

	return 0;
}

void CMenus::GhostlistPopulate()
{
	m_OwnGhost = 0;
	m_lGhosts.clear();
	Storage()->ListDirectory(IStorage::TYPE_ALL, "ghosts", GhostlistFetchCallback, this);

	for(int i = 0; i < m_lGhosts.size(); i++)
	{
		if(str_comp(m_lGhosts[i].m_aPlayer, g_Config.m_PlayerName) == 0 && (!m_OwnGhost || m_lGhosts[i] < *m_OwnGhost))
			m_OwnGhost = &m_lGhosts[i];
	}

	if(m_OwnGhost)
	{
		m_OwnGhost->m_ID = -1;
		m_OwnGhost->m_Active = true;
		m_pClient->m_pGhost->Load(m_OwnGhost->m_aFilename, -1);
	}
}

void CMenus::RenderGhost(CUIRect MainView)
{

}
