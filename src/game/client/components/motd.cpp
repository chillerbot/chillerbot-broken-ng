/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/shared/config.h>
#include <engine/keys.h>

#include <game/generated/protocol.h>
#include <game/generated/client_data.h>
#include <game/client/gameclient.h>

#include "motd.h"

void CMotd::Clear()
{
	m_ServerMotdTime = 0;
}

bool CMotd::IsActive()
{
	return time_get() < m_ServerMotdTime;
}

void CMotd::OnStateChange(int NewState, int OldState)
{
	if(OldState == IClient::STATE_ONLINE || OldState == IClient::STATE_OFFLINE)
		Clear();
}

void CMotd::OnRender()
{
	if(!IsActive())
		return;
	//dbg_msg("motd","%s", m_aServerMotd);
}

void CMotd::OnMessage(int MsgType, void *pRawMsg)
{
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		return;

	if(MsgType == NETMSGTYPE_SV_MOTD)
	{
		CNetMsg_Sv_Motd *pMsg = (CNetMsg_Sv_Motd *)pRawMsg;

		char* pLast = m_aServerMotd;
		// process escaping
		str_copy(m_aServerMotd, pMsg->m_pMessage, sizeof(m_aServerMotd));
		for(int i = 0; m_aServerMotd[i]; i++)
		{
			if(m_aServerMotd[i] == '\\')
			{
				if(m_aServerMotd[i+1] == 'n')
				{
					m_aServerMotd[i] = '\0';
					m_pClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "motd", pLast, true);
					m_aServerMotd[i] = ' ';
					m_aServerMotd[i+1] = '\n';
					i++;
					pLast = m_aServerMotd+i+1;
				}
			}
		}
		if(*pLast)
			m_pClient->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "motd", pLast, true);

		if(m_aServerMotd[0] && g_Config.m_ClMotdTime)
			m_ServerMotdTime = time_get()+time_freq()*g_Config.m_ClMotdTime;
		else
			m_ServerMotdTime = 0;
	}
}

bool CMotd::OnInput(IInput::CEvent Event)
{
	if(IsActive() && Event.m_Flags&IInput::FLAG_PRESS && Event.m_Key == KEY_ESCAPE)
	{
		Clear();
		return true;
	}
	return false;
}
