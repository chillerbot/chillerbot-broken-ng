/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/serverbrowser.h>
#include <engine/shared/config.h>

#include <game/generated/protocol.h>
#include <game/generated/client_data.h>
#include <game/layers.h>
#include <game/client/gameclient.h>
#include <game/client/animstate.h>
#include <game/client/components/scoreboard.h>

#include "controls.h"
#include "camera.h"
#include "hud.h"
#include "voting.h"
#include "binds.h"

CHud::CHud()
{
	// won't work if zero
	m_AverageFPS = 1.0f;
	OnReset();
}

void CHud::OnReset()
{
	m_CheckpointDiff = 0.0f;
	m_DDRaceTime = 0;
	m_LastReceivedTimeTick = 0;
	m_CheckpointTick = 0;
	m_DDRaceTick = 0;
	m_FinishTime = false;
	m_DDRaceTimeReceived = false;
	m_ServerRecord = -1.0f;
	m_PlayerRecord = -1.0f;
}

void CHud::RenderGameTimer()
{
	if(!(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_SUDDENDEATH))
	{
		char Buf[32];
		int Time = 0;
		if(m_pClient->m_Snap.m_pGameInfoObj->m_TimeLimit && !m_pClient->m_Snap.m_pGameInfoObj->m_WarmupTimer)
		{
			Time = m_pClient->m_Snap.m_pGameInfoObj->m_TimeLimit*60 - ((Client()->GameTick()-m_pClient->m_Snap.m_pGameInfoObj->m_RoundStartTick)/Client()->GameTickSpeed());

			if(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER)
				Time = 0;
		}
		else
			Time = (Client()->GameTick()-m_pClient->m_Snap.m_pGameInfoObj->m_RoundStartTick)/Client()->GameTickSpeed();

		CServerInfo Info;
		Client()->GetServerInfo(&Info);

		if(Time <= 0 && g_Config.m_ClShowDecisecs)
			str_format(Buf, sizeof(Buf), "00:00.0");
		else if(Time <= 0)
			str_format(Buf, sizeof(Buf), "00:00");
		else if(IsRace(&Info) && !IsDDRace(&Info) && m_ServerRecord >= 0)
			str_format(Buf, sizeof(Buf), "%02d:%02d", (int)(m_ServerRecord*100)/60, ((int)(m_ServerRecord*100)%60));
		else if(g_Config.m_ClShowDecisecs)
			str_format(Buf, sizeof(Buf), "%02d:%02d.%d", Time/60, Time%60, m_DDRaceTick/10);
		else
			str_format(Buf, sizeof(Buf), "%02d:%02d", Time/60, Time%60);
	}
}

void CHud::RenderPauseNotification()
{
	if(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_PAUSED &&
		!(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER))
	{
		//game paused
	}
}

void CHud::RenderSuddenDeath()
{
	if(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_SUDDENDEATH)
	{
		//sudden death
	}
}

void CHud::RenderScoreHud()
{
	// render small score hud
	if(!(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER))
	{
		int GameFlags = m_pClient->m_Snap.m_pGameInfoObj->m_GameFlags;
		float StartY = 229.0f;

		if(GameFlags&GAMEFLAG_TEAMS && m_pClient->m_Snap.m_pGameDataObj)
		{
			char aScoreTeam[2][32];
			str_format(aScoreTeam[TEAM_RED], sizeof(aScoreTeam)/2, "%d", m_pClient->m_Snap.m_pGameDataObj->m_TeamscoreRed);
			str_format(aScoreTeam[TEAM_BLUE], sizeof(aScoreTeam)/2, "%d", m_pClient->m_Snap.m_pGameDataObj->m_TeamscoreBlue);
			int FlagCarrier[2] = { m_pClient->m_Snap.m_pGameDataObj->m_FlagCarrierRed, m_pClient->m_Snap.m_pGameDataObj->m_FlagCarrierBlue };

			for(int t = 0; t < 2; t++)
			{
				if(GameFlags&GAMEFLAG_FLAGS)
				{
					int BlinkTimer = (m_pClient->m_FlagDropTick[t] != 0 &&
										(Client()->GameTick()-m_pClient->m_FlagDropTick[t])/Client()->GameTickSpeed() >= 25) ? 10 : 20;
					if(FlagCarrier[t] == FLAG_ATSTAND || (FlagCarrier[t] == FLAG_TAKEN && ((Client()->GameTick()/BlinkTimer)&1)))
					{
						// draw flag
					}
					else if(FlagCarrier[t] >= 0)
					{
						// draw name of the flag holder
						//int ID = FlagCarrier[t]%MAX_CLIENTS;
						//const char *pName = m_pClient->m_aClients[ID].m_aName;
						//dbg_msg("flag:", "%s", pName);
					}
				}
				StartY += 8.0f;
			}
		}
		else
		{
			int Local = -1;
			int aPos[2] = { 1, 2 };
			const CNetObj_PlayerInfo *apPlayerInfo[2] = { 0, 0 };
			int i = 0;
			for(int t = 0; t < 2 && i < MAX_CLIENTS && m_pClient->m_Snap.m_paInfoByScore[i]; ++i)
			{
				if(m_pClient->m_Snap.m_paInfoByScore[i]->m_Team != TEAM_SPECTATORS)
				{
					apPlayerInfo[t] = m_pClient->m_Snap.m_paInfoByScore[i];
					if(apPlayerInfo[t]->m_ClientID == m_pClient->m_Snap.m_LocalClientID)
						Local = t;
					++t;
				}
			}
			// search local player info if not a spectator, nor within top2 scores
			if(Local == -1 && m_pClient->m_Snap.m_pLocalInfo && m_pClient->m_Snap.m_pLocalInfo->m_Team != TEAM_SPECTATORS)
			{
				for(; i < MAX_CLIENTS && m_pClient->m_Snap.m_paInfoByScore[i]; ++i)
				{
					if(m_pClient->m_Snap.m_paInfoByScore[i]->m_Team != TEAM_SPECTATORS)
						++aPos[1];
					if(m_pClient->m_Snap.m_paInfoByScore[i]->m_ClientID == m_pClient->m_Snap.m_LocalClientID)
					{
						apPlayerInfo[1] = m_pClient->m_Snap.m_paInfoByScore[i];
						Local = 1;
						break;
					}
				}
			}
			char aScore[2][32];
			for(int t = 0; t < 2; ++t)
			{
				if(apPlayerInfo[t])
				{
					CServerInfo Info;
					Client()->GetServerInfo(&Info);
					if(IsRace(&Info) && g_Config.m_ClDDRaceScoreBoard)
					{
						if (apPlayerInfo[t]->m_Score != -9999)
							str_format(aScore[t], sizeof(aScore[t]), "%02d:%02d", abs(apPlayerInfo[t]->m_Score)/60, abs(apPlayerInfo[t]->m_Score)%60);
						else
							aScore[t][0] = 0;
					}
					else
						str_format(aScore[t], sizeof(aScore)/2, "%d", apPlayerInfo[t]->m_Score);
				}
				else
					aScore[t][0] = 0;
			}

			for(int t = 0; t < 2; t++)
			{
				if(apPlayerInfo[t])
				{
					// draw name
					int ID = apPlayerInfo[t]->m_ClientID;
					if(ID >= 0 && ID < MAX_CLIENTS)
					{
						//const char *pName = m_pClient->m_aClients[ID].m_aName;
						//dbg_msg("Score", "%s", pName);
					}
				}

				// draw position
				char aBuf[32];
				str_format(aBuf, sizeof(aBuf), "%d.", aPos[t]);

				StartY += 8.0f;
			}
		}
	}
}

void CHud::RenderWarmupTimer()
{
	// render warmup timer
	if(m_pClient->m_Snap.m_pGameInfoObj->m_WarmupTimer)
	{
		char Buf[256];

		int Seconds = m_pClient->m_Snap.m_pGameInfoObj->m_WarmupTimer/SERVER_TICK_SPEED;
		if(Seconds < 5)
			str_format(Buf, sizeof(Buf), "%d.%d", Seconds, (m_pClient->m_Snap.m_pGameInfoObj->m_WarmupTimer*10/SERVER_TICK_SPEED)%10);
		else
			str_format(Buf, sizeof(Buf), "%d", Seconds);
	}
}

void CHud::MapscreenToGroup(float CenterX, float CenterY, CMapItemGroup *pGroup)
{

}

void CHud::RenderTextInfo()
{
	if(g_Config.m_ClShowfps)
	{
		// calculate avg. fps
		float FPS = 1.0f / Client()->RenderFrameTime(); //ChillerDragon thinks this can also be used in the nogui client
		m_AverageFPS = (m_AverageFPS*(1.0f-(1.0f/m_AverageFPS))) + (FPS*(1.0f/m_AverageFPS));
		char Buf[512];
		str_format(Buf, sizeof(Buf), "%d", (int)m_AverageFPS);
	}
	if(g_Config.m_ClShowpred)
	{
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "%d", Client()->GetPredictionTime());
	}
}

void CHud::RenderConnectionWarning()
{
	if(Client()->ConnectionProblems())
	{
		//conection problems...
	}
}

void CHud::RenderTeambalanceWarning()
{
	// render prompt about team-balance
}


void CHud::RenderVoting()
{
	if((!g_Config.m_ClShowVotesAfterVoting && !m_pClient->m_pScoreboard->Active() && m_pClient->m_pVoting->TakenChoice()) || !m_pClient->m_pVoting->IsVoting() || Client()->State() == IClient::STATE_DEMOPLAYBACK)
		return;
}

void CHud::RenderCursor()
{
	if(!m_pClient->m_Snap.m_pLocalCharacter || Client()->State() == IClient::STATE_DEMOPLAYBACK)
		return;
}

void CHud::RenderHealthAndAmmo(const CNetObj_Character *pCharacter)
{
	if(!pCharacter)
		return;
}

void CHud::RenderSpectatorHud()
{

}

void CHud::RenderLocalTime(float x)
{

}

void CHud::OnRender()
{
	//ChillerDragon: keep all the stuff here because we can use it for the nogui version aswell
	if(!m_pClient->m_Snap.m_pGameInfoObj)
		return;

	if(g_Config.m_ClShowhud)
	{
		if(m_pClient->m_Snap.m_pLocalCharacter && !(m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER))
		{
			if (g_Config.m_ClShowhudHealthAmmo)
				RenderHealthAndAmmo(m_pClient->m_Snap.m_pLocalCharacter);
			RenderDDRaceEffects();
		}
		else if(m_pClient->m_Snap.m_SpecInfo.m_Active)
		{
			if(m_pClient->m_Snap.m_SpecInfo.m_SpectatorID != SPEC_FREEVIEW && g_Config.m_ClShowhudHealthAmmo)
				RenderHealthAndAmmo(&m_pClient->m_Snap.m_aCharacters[m_pClient->m_Snap.m_SpecInfo.m_SpectatorID].m_Cur);
			RenderSpectatorHud();
		}

		RenderGameTimer();
		RenderPauseNotification();
		RenderSuddenDeath();
		if (g_Config.m_ClShowhudScore)
			RenderScoreHud();
		RenderWarmupTimer();
		RenderTextInfo();
		RenderLocalTime((m_Width/7)*3);
		if(Client()->State() != IClient::STATE_DEMOPLAYBACK)
			RenderConnectionWarning();
		RenderTeambalanceWarning();
		RenderVoting();
		if (g_Config.m_ClShowRecord)
			RenderRecord();
	}
	RenderCursor();
}

void CHud::OnMessage(int MsgType, void *pRawMsg)
{
	if(MsgType == NETMSGTYPE_SV_DDRACETIME)
	{
		m_DDRaceTimeReceived = true;

		CNetMsg_Sv_DDRaceTime *pMsg = (CNetMsg_Sv_DDRaceTime *)pRawMsg;

		m_DDRaceTime = pMsg->m_Time;
		m_DDRaceTick = 0;

		m_LastReceivedTimeTick = Client()->GameTick();

		m_FinishTime = pMsg->m_Finish ? true : false;

		if(pMsg->m_Check)
		{
			m_CheckpointDiff = (float)pMsg->m_Check/100;
			m_CheckpointTick = Client()->GameTick();
		}
	}
	else if(MsgType == NETMSGTYPE_SV_KILLMSG)
	{
		CNetMsg_Sv_KillMsg *pMsg = (CNetMsg_Sv_KillMsg *)pRawMsg;
		if(pMsg->m_Victim == m_pClient->m_Snap.m_LocalClientID)
		{
			m_CheckpointTick = 0;
			m_DDRaceTime = 0;
		}
	}
	else if(MsgType == NETMSGTYPE_SV_RECORD)
	{
		CServerInfo Info;
		Client()->GetServerInfo(&Info);

		CNetMsg_Sv_Record *pMsg = (CNetMsg_Sv_Record *)pRawMsg;

		// NETMSGTYPE_SV_RACETIME on old race servers
		if(!IsDDRace(&Info) && IsRace(&Info))
		{
			m_DDRaceTimeReceived = true;

			m_DDRaceTime = pMsg->m_ServerTimeBest; // First value: m_Time
			m_DDRaceTick = 0;

			m_LastReceivedTimeTick = Client()->GameTick();

			if(pMsg->m_PlayerTimeBest) // Second value: m_Check
			{
				m_CheckpointDiff = (float)pMsg->m_PlayerTimeBest/100;
				m_CheckpointTick = Client()->GameTick();
			}
		}
		else
		{
			m_ServerRecord = (float)pMsg->m_ServerTimeBest/100;
			m_PlayerRecord = (float)pMsg->m_PlayerTimeBest/100;
		}
	}
}

void CHud::RenderDDRaceEffects()
{
	// check racestate
	if(m_FinishTime && m_LastReceivedTimeTick + Client()->GameTickSpeed()*2 < Client()->GameTick())
	{
		m_FinishTime = false;
		m_DDRaceTimeReceived = false;
		return;
	}

	if(m_DDRaceTime)
	{
		char aBuf[64];
		if(m_FinishTime)
		{
			str_format(aBuf, sizeof(aBuf), "Finish time: %02d:%02d.%02d", m_DDRaceTime/6000, m_DDRaceTime/100-m_DDRaceTime/6000 * 60, m_DDRaceTime % 100);
		}
	}

	static int LastChangeTick = 0;
	if(LastChangeTick != Client()->PredGameTick())
	{
		m_DDRaceTick += 100/Client()->GameTickSpeed();
		LastChangeTick = Client()->PredGameTick();
	}

	if(m_DDRaceTick >= 100)
		m_DDRaceTick = 0;
}

void CHud::RenderRecord()
{
	if(m_ServerRecord > 0 )
	{
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "Server best:");
		str_format(aBuf, sizeof(aBuf), "%02d:%05.2f", (int)m_ServerRecord/60, m_ServerRecord-((int)m_ServerRecord/60*60));
	}

	if(m_PlayerRecord > 0 )
	{
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "Personal best:");
		str_format(aBuf, sizeof(aBuf), "%02d:%05.2f", (int)m_PlayerRecord/60, m_PlayerRecord-((int)m_PlayerRecord/60*60));
	}
}
