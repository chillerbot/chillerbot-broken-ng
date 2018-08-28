/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <base/tl/sorted_array.h>

#include <engine/demo.h>
#include <engine/engine.h>
#include <engine/shared/config.h>
#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/gamecore.h> // get_angle
#include <game/client/animstate.h>
#include <game/client/gameclient.h>
#include <game/client/ui.h>
#include <game/client/render.h>

#include <game/client/components/flow.h>
#include <game/client/components/skins.h>
#include <game/client/components/effects.h>
#include <game/client/components/controls.h>

#include <engine/textrender.h>

#include "players.h"
#include <stdio.h>

void CPlayers::RenderHand(CTeeRenderInfo *pInfo, vec2 CenterPos, vec2 Dir, float AngleOffset, vec2 PostRotOffset)
{

}

inline float NormalizeAngular(float f)
{
	return fmod(f+pi*2, pi*2);
}

inline float AngularMixDirection (float Src, float Dst) { return sinf(Dst-Src) >0?1:-1; }
inline float AngularDistance(float Src, float Dst) { return asinf(sinf(Dst-Src)); }

inline float AngularApproach(float Src, float Dst, float Amount)
{
	float d = AngularMixDirection (Src, Dst);
	float n = Src + Amount*d;
	if(AngularMixDirection (n, Dst) != d)
		return Dst;
	return n;
}

void CPlayers::Predict(
	const CNetObj_Character *pPrevChar,
	const CNetObj_Character *pPlayerChar,
	const CNetObj_PlayerInfo *pPrevInfo,
	const CNetObj_PlayerInfo *pPlayerInfo,
	vec2 &PrevPredPos,
	vec2 &SmoothPos,
	int &MoveCnt,
	vec2 &Position
	)
{
	CNetObj_Character Prev;
	CNetObj_Character Player;
	Prev = *pPrevChar;
	Player = *pPlayerChar;

	CNetObj_PlayerInfo pInfo = *pPlayerInfo;


	// set size

	float IntraTick = Client()->IntraGameTick();


	//float angle = 0;

	if(pInfo.m_Local && Client()->State() != IClient::STATE_DEMOPLAYBACK)
	{
		// just use the direct input if it's local player we are rendering
	}
	else
	{
		/*
		float mixspeed = Client()->FrameTime()*2.5f;
		if(player.attacktick != prev.attacktick) // shooting boosts the mixing speed
			mixspeed *= 15.0f;

		// move the delta on a constant speed on a x^2 curve
		float current = g_GameClient.m_aClients[info.cid].angle;
		float target = player.angle/256.0f;
		float delta = angular_distance(current, target);
		float sign = delta < 0 ? -1 : 1;
		float new_delta = delta - 2*mixspeed*sqrt(delta*sign)*sign + mixspeed*mixspeed;

		// make sure that it doesn't vibrate when it's still
		if(fabs(delta) < 2/256.0f)
			angle = target;
		else
			angle = angular_approach(current, target, fabs(delta-new_delta));

		g_GameClient.m_aClients[info.cid].angle = angle;*/
	}

vec2 NonPredPos = mix(vec2(Prev.m_X, Prev.m_Y), vec2(Player.m_X, Player.m_Y), IntraTick);

	// use preditect players if needed
	if(g_Config.m_ClPredict && Client()->State() != IClient::STATE_DEMOPLAYBACK)
	{
		if(m_pClient->m_Snap.m_pLocalCharacter && !(m_pClient->m_Snap.m_pGameInfoObj && m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER))
		{
			// apply predicted results
			m_pClient->m_aClients[pInfo.m_ClientID].m_Predicted.Write(&Player);
			m_pClient->m_aClients[pInfo.m_ClientID].m_PrevPredicted.Write(&Prev);

			IntraTick = Client()->PredIntraGameTick();
		}
	}

	Position = mix(vec2(Prev.m_X, Prev.m_Y), vec2(Player.m_X, Player.m_Y), IntraTick);


	static double ping = 0;

	if(pInfo.m_Local) {
		ping = mix(ping, (double)pInfo.m_Latency, 0.1);
	}

	if(!pInfo.m_Local)
	{
		/*
		for ping = 260, usual missprediction distances:

		move = 120-140
		jump = 130
		dj = 250

		normalized:
		move = 0.461 - 0.538
		jump = 0.5
		dj = .961

		*/
		//printf("%d\n", m_pClient->m_Snap.m_pLocalInfo->m_Latency);


		if(m_pClient->m_Snap.m_pLocalInfo)
			ping = mix(ping, (double)m_pClient->m_Snap.m_pLocalInfo->m_Latency, 0.1);

		double d = length(PrevPredPos - Position)/ping;

		if((d > 0.4) && (d < 5.))
		{
//			if(MoveCnt == 0)
//				printf("[\n");
			if(MoveCnt == 0)
				SmoothPos = NonPredPos;

			MoveCnt = 10;
//		SmoothPos = PrevPredPos;
//		SmoothPos = mix(NonPredPos, Position, 0.6);
		}

		PrevPredPos = Position;

		if(MoveCnt > 0)
		{
//		Position = mix(mix(NonPredPos, Position, 0.5), SmoothPos, (((float)MoveCnt))/15);
//		Position = mix(mix(NonPredPos, Position, 0.5), SmoothPos, 0.5);
			Position = mix(NonPredPos, Position, 0.5);

			SmoothPos = Position;
			MoveCnt--;
//		if(MoveCnt == 0)
//			printf("]\n\n");
		}
	}
}

void CPlayers::RenderHook(
	const CNetObj_Character *pPrevChar,
	const CNetObj_Character *pPlayerChar,
	const CNetObj_PlayerInfo *pPrevInfo,
	const CNetObj_PlayerInfo *pPlayerInfo,
	const vec2 &parPosition,
	const vec2 &PositionTo
	)
{

}

void CPlayers::RenderPlayer(
	const CNetObj_Character *pPrevChar,
	const CNetObj_Character *pPlayerChar,
	const CNetObj_PlayerInfo *pPrevInfo,
	const CNetObj_PlayerInfo *pPlayerInfo,
	const vec2 &parPosition
/*	vec2 &PrevPos,
	vec2 &SmoothPos,
	int &MoveCnt
*/	)
{

}

void CPlayers::OnRender()
{
	static vec2 PrevPos[MAX_CLIENTS];
	static vec2 SmoothPos[MAX_CLIENTS];
	static int MoveCnt[MAX_CLIENTS] = {0};
	static vec2 PredictedPos[MAX_CLIENTS];

	static int predcnt = 0;

	if (m_pClient->AntiPingPlayers())
	{
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			if(!m_pClient->m_Snap.m_aCharacters[i].m_Active)
				continue;
			const void *pPrevInfo = Client()->SnapFindItem(IClient::SNAP_PREV, NETOBJTYPE_PLAYERINFO, i);
			const void *pInfo = Client()->SnapFindItem(IClient::SNAP_CURRENT, NETOBJTYPE_PLAYERINFO, i);

			if(pPrevInfo && pInfo)
			{
				CNetObj_Character PrevChar = m_pClient->m_Snap.m_aCharacters[i].m_Prev;
				CNetObj_Character CurChar = m_pClient->m_Snap.m_aCharacters[i].m_Cur;

				Predict(
						&PrevChar,
						&CurChar,
						(const CNetObj_PlayerInfo *)pPrevInfo,
						(const CNetObj_PlayerInfo *)pInfo,
						PrevPos[i],
						SmoothPos[i],
						MoveCnt[i],
						PredictedPos[i]
					);
			}
		}

		if(m_pClient->AntiPingPlayers() && g_Config.m_ClPredict && Client()->State() != IClient::STATE_DEMOPLAYBACK)
			if(m_pClient->m_Snap.m_pLocalCharacter && !(m_pClient->m_Snap.m_pGameInfoObj && m_pClient->m_Snap.m_pGameInfoObj->m_GameStateFlags&GAMESTATEFLAG_GAMEOVER))
			{
	//			double ping = m_pClient->m_Snap.m_pLocalInfo->m_Latency;
	//			static double fps;
	//			fps = mix(fps, (1. / Client()->RenderFrameTime()), 0.1);

	//			int predmax = (fps * ping / 1000.);

				int predmax = 19;
	//			if( 0 <= predmax && predmax <= 100)
						predcnt = (predcnt + 1) % predmax;
	//			else
	//			    predcnt = (predcnt + 1) % 2;
			}
	}

	// render other players in two passes, first pass we render the other, second pass we render our self
	for(int p = 0; p < 4; p++)
	{
		for(int i = 0; i < MAX_CLIENTS; i++)
		{
			// only render active characters
			if(!m_pClient->m_Snap.m_aCharacters[i].m_Active)
				continue;

			const void *pPrevInfo = Client()->SnapFindItem(IClient::SNAP_PREV, NETOBJTYPE_PLAYERINFO, i);
			const void *pInfo = Client()->SnapFindItem(IClient::SNAP_CURRENT, NETOBJTYPE_PLAYERINFO, i);

			if(pPrevInfo && pInfo)
			{
				//
				bool Local = ((const CNetObj_PlayerInfo *)pInfo)->m_Local !=0;
				if((p % 2) == 0 && Local) continue;
				if((p % 2) == 1 && !Local) continue;

				CNetObj_Character PrevChar = m_pClient->m_Snap.m_aCharacters[i].m_Prev;
				CNetObj_Character CurChar = m_pClient->m_Snap.m_aCharacters[i].m_Cur;

				if(p<2)
				{
					if(PrevChar.m_HookedPlayer != -1)
						RenderHook(
								&PrevChar,
								&CurChar,
								(const CNetObj_PlayerInfo *)pPrevInfo,
								(const CNetObj_PlayerInfo *)pInfo,
								PredictedPos[i],
								PredictedPos[PrevChar.m_HookedPlayer]
							);
					else
						RenderHook(
								&PrevChar,
								&CurChar,
								(const CNetObj_PlayerInfo *)pPrevInfo,
								(const CNetObj_PlayerInfo *)pInfo,
								PredictedPos[i],
								PredictedPos[i]
							);
				}
				else
				{
					RenderPlayer(
							&PrevChar,
							&CurChar,
							(const CNetObj_PlayerInfo *)pPrevInfo,
							(const CNetObj_PlayerInfo *)pInfo,
							PredictedPos[i]
						);
				}
			}
		}
	}
}
