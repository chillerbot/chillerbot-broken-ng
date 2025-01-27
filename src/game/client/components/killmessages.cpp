/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <game/generated/protocol.h>
#include <game/generated/client_data.h>
#include <engine/shared/config.h>

#include <game/client/gameclient.h>
#include "killmessages.h"

void CKillMessages::OnReset()
{
	m_KillmsgCurrent = 0;
	for(int i = 0; i < MAX_KILLMSGS; i++)
		m_aKillmsgs[i].m_Tick = -100000;
}

void CKillMessages::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_KILLMSG)
	{
		CNetMsg_Sv_KillMsg *pMsg = (CNetMsg_Sv_KillMsg *)pRawMsg;

		// unpack messages
		CKillMsg Kill;
		Kill.m_VictimID = pMsg->m_Victim;
		Kill.m_VictimTeam = m_pClient->m_aClients[Kill.m_VictimID].m_Team;
		Kill.m_VictimDDTeam = m_pClient->m_Teams.Team(Kill.m_VictimID);
		str_copy(Kill.m_aVictimName, m_pClient->m_aClients[Kill.m_VictimID].m_aName, sizeof(Kill.m_aVictimName));
		Kill.m_VictimRenderInfo = m_pClient->m_aClients[Kill.m_VictimID].m_RenderInfo;
		Kill.m_KillerID = pMsg->m_Killer;
		Kill.m_KillerTeam = m_pClient->m_aClients[Kill.m_KillerID].m_Team;
		str_copy(Kill.m_aKillerName, m_pClient->m_aClients[Kill.m_KillerID].m_aName, sizeof(Kill.m_aKillerName));
		Kill.m_KillerRenderInfo = m_pClient->m_aClients[Kill.m_KillerID].m_RenderInfo;
		Kill.m_Weapon = pMsg->m_Weapon;
		Kill.m_ModeSpecial = pMsg->m_ModeSpecial;
		Kill.m_Tick = Client()->GameTick();

		// add the message
		m_KillmsgCurrent = (m_KillmsgCurrent+1)%MAX_KILLMSGS;
		m_aKillmsgs[m_KillmsgCurrent] = Kill;
	}
}

void CKillMessages::OnRender()
{
	if (!g_Config.m_ClShowKillMessages)
		return;

	for (int i = 1; i <= MAX_KILLMSGS; i++)
	{
		int r = (m_KillmsgCurrent + i) % MAX_KILLMSGS;
		if (Client()->GameTick() > m_aKillmsgs[r].m_Tick + 50 * 10)
			continue;

		//dbg_msg("kill", "victim=%d:%s weapon=%d killer=%s", m_aKillmsgs[r].m_VictimID, m_aKillmsgs[r].m_aVictimName, m_aKillmsgs[r].m_Weapon, m_aKillmsgs[r].m_aKillerName);
	}
}
