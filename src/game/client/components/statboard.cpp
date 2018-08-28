#include <engine/shared/config.h>
#include <engine/serverbrowser.h>
#include <game/generated/client_data.h>
#include <game/client/gameclient.h>
#include <game/client/animstate.h>
#include <game/client/components/motd.h>
#include <game/client/components/statboard.h>

CStatboard::CStatboard()
{
	m_Active = false;
	m_ScreenshotTaken = false;
	m_ScreenshotTime = -1;
}

void CStatboard::OnReset()
{
	for(int i=0; i<MAX_CLIENTS; i++)
		m_pClient->m_aStats[i].Reset();
	m_Active = false;
	m_ScreenshotTaken = false;
	m_ScreenshotTime = -1;
}

void CStatboard::OnRelease()
{
	m_Active = false;
}

void CStatboard::ConKeyStats(IConsole::IResult *pResult, void *pUserData)
{
	((CStatboard *)pUserData)->m_Active = pResult->GetInteger(0) != 0;
}

void CStatboard::OnConsoleInit()
{
	Console()->Register("+statboard", "", CFGFLAG_CLIENT, ConKeyStats, this, "Show stats");
}

bool CStatboard::IsActive()
{
	return m_Active;
}

void CStatboard::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_KILLMSG)
	{
		CNetMsg_Sv_KillMsg *pMsg = (CNetMsg_Sv_KillMsg *)pRawMsg;
		CGameClient::CClientStats *pStats = m_pClient->m_aStats;

		pStats[pMsg->m_Victim].m_Deaths++;
		pStats[pMsg->m_Victim].m_CurrentSpree = 0;
		if(pMsg->m_Weapon >= 0)
			pStats[pMsg->m_Victim].m_aDeathsFrom[pMsg->m_Weapon]++;
		if(pMsg->m_Victim != pMsg->m_Killer)
		{
			pStats[pMsg->m_Killer].m_Frags++;
			pStats[pMsg->m_Killer].m_CurrentSpree++;

			if(pStats[pMsg->m_Killer].m_CurrentSpree > pStats[pMsg->m_Killer].m_BestSpree)
				pStats[pMsg->m_Killer].m_BestSpree = pStats[pMsg->m_Killer].m_CurrentSpree;
			if(pMsg->m_Weapon >= 0)
				pStats[pMsg->m_Killer].m_aFragsWith[pMsg->m_Weapon]++;
		}
		else
			pStats[pMsg->m_Victim].m_Suicides++;
	}
	else if(MsgType == NETMSGTYPE_SV_CHAT)
	{
		CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
		if(pMsg->m_ClientID < 0)
		{
			const char *p;
			const char *pLookFor = "flag was captured by '";
			if(str_find(pMsg->m_pMessage, pLookFor) != 0)
			{
				// server info
				CServerInfo CurrentServerInfo;
				Client()->GetServerInfo(&CurrentServerInfo);

				p = str_find(pMsg->m_pMessage, pLookFor);
				char aName[64];
				p += str_length(pLookFor);
				str_copy(aName, p, sizeof(aName));
				// remove capture time
				if(str_comp(aName+str_length(aName)-9, " seconds)") == 0)
				{
					char *c = aName+str_length(aName)-10;
					while(c > aName)
					{
						c--;
						if(*c == '(')
						{
							*(c-1) = 0;
							break;
						}
					}
				}
				// remove the ' at the end
				aName[str_length(aName)-1] = 0;

				for(int i = 0; i < MAX_CLIENTS; i++)
				{
					if(!m_pClient->m_aStats[i].m_Active)
						continue;

					if(str_comp(m_pClient->m_aClients[i].m_aName, aName) == 0)
					{
						m_pClient->m_aStats[i].m_FlagCaptures++;
						break;
					}
				}
			}
		}
	}
}

void CStatboard::OnRender()
{
	if(IsActive())
		RenderGlobalStats();
}

void CStatboard::RenderGlobalStats()
{

}

void CStatboard::AutoStatScreenshot()
{
	if(Client()->State() != IClient::STATE_DEMOPLAYBACK)
		Client()->AutoStatScreenshot_Start();
}
