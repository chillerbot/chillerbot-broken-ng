/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/tl/sorted_array.h>
#include <stdio.h>

#include <base/math.h>

#include <engine/shared/config.h>
#include <engine/serverbrowser.h>

#include <game/collision.h>
#include <game/client/gameclient.h>
#include <game/client/component.h>
#include <game/client/components/chat.h>
#include <game/client/components/menus.h>
#include <game/client/components/scoreboard.h>

#include "controls.h"

enum { LEFT_JOYSTICK_X = 0, LEFT_JOYSTICK_Y = 1,
	RIGHT_JOYSTICK_X = 2, RIGHT_JOYSTICK_Y = 3,
	SECOND_RIGHT_JOYSTICK_X = 20, SECOND_RIGHT_JOYSTICK_Y = 21,
	NUM_JOYSTICK_AXES = 22 };

CControls::CControls()
{
	mem_zero(&m_LastData, sizeof(m_LastData));
	m_LastDummy = 0;
	m_OtherFire = 0;
}

void CControls::OnReset()
{
	ResetInput(0);
	ResetInput(1);

	m_JoystickFirePressed = false;
	m_JoystickRunPressed = false;
	m_JoystickTapTime = 0;
	for( int i = 0; i < NUM_WEAPONS; i++ )
		m_AmmoCount[i] = 0;
	m_OldMouseX = m_OldMouseY = 0.0f;
}

void CControls::ResetInput(int dummy)
{
	m_LastData[dummy].m_Direction = 0;
	//m_LastData.m_Hook = 0;
	// simulate releasing the fire button
	if((m_LastData[dummy].m_Fire&1) != 0)
		m_LastData[dummy].m_Fire++;
	m_LastData[dummy].m_Fire &= INPUT_STATE_MASK;
	m_LastData[dummy].m_Jump = 0;
	m_InputData[dummy] = m_LastData[dummy];

	m_InputDirectionLeft[dummy] = 0;
	m_InputDirectionRight[dummy] = 0;
}

void CControls::OnRelease()
{
	//OnReset();
}

void CControls::OnPlayerDeath()
{
	if (g_Config.m_ClResetWantedWeaponOnDeath)
		m_LastData[g_Config.m_ClDummy].m_WantedWeapon = m_InputData[g_Config.m_ClDummy].m_WantedWeapon = 0;
	for( int i = 0; i < NUM_WEAPONS; i++ )
		m_AmmoCount[i] = 0;
	m_JoystickTapTime = 0; // Do not launch hook on first tap
}

struct CInputState
{
	CControls *m_pControls;
	int *m_pVariable1;
	int *m_pVariable2;
};

static void ConKeyInputState(IConsole::IResult *pResult, void *pUserData)
{

}

static void ConKeyInputCounter(IConsole::IResult *pResult, void *pUserData)
{

}

struct CInputSet
{
	CControls *m_pControls;
	int *m_pVariable1;
	int *m_pVariable2;
	int m_Value;
};

static void ConKeyInputSet(IConsole::IResult *pResult, void *pUserData)
{

}

static void ConKeyInputNextPrevWeapon(IConsole::IResult *pResult, void *pUserData)
{

}

void CControls::OnConsoleInit()
{
	// game commands
	{ static CInputState s_State = {this, &m_InputDirectionLeft[0], &m_InputDirectionLeft[1]}; Console()->Register("+left", "", CFGFLAG_CLIENT, ConKeyInputState, (void *)&s_State, "Move left"); }
	{ static CInputState s_State = {this, &m_InputDirectionRight[0], &m_InputDirectionRight[1]}; Console()->Register("+right", "", CFGFLAG_CLIENT, ConKeyInputState, (void *)&s_State, "Move right"); }
	{ static CInputState s_State = {this, &m_InputData[0].m_Jump, &m_InputData[1].m_Jump}; Console()->Register("+jump", "", CFGFLAG_CLIENT, ConKeyInputState, (void *)&s_State, "Jump"); }
	{ static CInputState s_State = {this, &m_InputData[0].m_Hook, &m_InputData[1].m_Hook}; Console()->Register("+hook", "", CFGFLAG_CLIENT, ConKeyInputState, (void *)&s_State, "Hook"); }
	{ static CInputState s_State = {this, &m_InputData[0].m_Fire, &m_InputData[1].m_Fire}; Console()->Register("+fire", "", CFGFLAG_CLIENT, ConKeyInputCounter, (void *)&s_State, "Fire"); }
	{ static CInputState s_State = {this, &m_ShowHookColl[0], &m_ShowHookColl[1]}; Console()->Register("+showhookcoll", "", CFGFLAG_CLIENT, ConKeyInputState, (void *)&s_State, "Show Hook Collision"); }

	{ static CInputSet s_Set = {this, &m_InputData[0].m_WantedWeapon, &m_InputData[1].m_WantedWeapon, 1}; Console()->Register("+weapon1", "", CFGFLAG_CLIENT, ConKeyInputSet, (void *)&s_Set, "Switch to hammer"); }
	{ static CInputSet s_Set = {this, &m_InputData[0].m_WantedWeapon, &m_InputData[1].m_WantedWeapon, 2}; Console()->Register("+weapon2", "", CFGFLAG_CLIENT, ConKeyInputSet, (void *)&s_Set, "Switch to gun"); }
	{ static CInputSet s_Set = {this, &m_InputData[0].m_WantedWeapon, &m_InputData[1].m_WantedWeapon, 3}; Console()->Register("+weapon3", "", CFGFLAG_CLIENT, ConKeyInputSet, (void *)&s_Set, "Switch to shotgun"); }
	{ static CInputSet s_Set = {this, &m_InputData[0].m_WantedWeapon, &m_InputData[1].m_WantedWeapon, 4}; Console()->Register("+weapon4", "", CFGFLAG_CLIENT, ConKeyInputSet, (void *)&s_Set, "Switch to grenade"); }
	{ static CInputSet s_Set = {this, &m_InputData[0].m_WantedWeapon, &m_InputData[1].m_WantedWeapon, 5}; Console()->Register("+weapon5", "", CFGFLAG_CLIENT, ConKeyInputSet, (void *)&s_Set, "Switch to rifle"); }

	{ static CInputSet s_Set = {this, &m_InputData[0].m_NextWeapon, &m_InputData[1].m_NextWeapon, 0}; Console()->Register("+nextweapon", "", CFGFLAG_CLIENT, ConKeyInputNextPrevWeapon, (void *)&s_Set, "Switch to next weapon"); }
	{ static CInputSet s_Set = {this, &m_InputData[0].m_PrevWeapon, &m_InputData[1].m_PrevWeapon, 0}; Console()->Register("+prevweapon", "", CFGFLAG_CLIENT, ConKeyInputNextPrevWeapon, (void *)&s_Set, "Switch to previous weapon"); }
}

void CControls::OnMessage(int Msg, void *pRawMsg)
{
	if(Msg == NETMSGTYPE_SV_WEAPONPICKUP)
	{
		CNetMsg_Sv_WeaponPickup *pMsg = (CNetMsg_Sv_WeaponPickup *)pRawMsg;
		if(g_Config.m_ClAutoswitchWeapons)
			m_InputData[g_Config.m_ClDummy].m_WantedWeapon = pMsg->m_Weapon+1;
		// We don't really know ammo count, until we'll switch to that weapon, but any non-zero count will suffice here
		m_AmmoCount[pMsg->m_Weapon%NUM_WEAPONS] = 10;
	}
}

void CControls::StartHook(int pre, int hook)
{
    m_pre_hook_tick = pre;
    m_hook_tick = hook;
}

void CControls::DoHook()
{
    if (m_pre_hook_tick > 0)
    {
        m_InputData[g_Config.m_ClDummy].m_Hook = 0;
        m_pre_hook_tick--;
        return;
    }

    if (m_hook_tick > 0)
    {
        m_InputData[g_Config.m_ClDummy].m_Hook = 1;
        m_hook_tick--;
    }
}

void CControls::StartJump(int pre, int jump)
{
    if (m_pre_jump_tick > 0 || m_jump_tick > 0)
        return;
    m_pre_jump_tick = pre;
    m_jump_tick = jump;
}

void CControls::DoJump()
{
    if (m_pre_jump_tick > 0)
    {
        //printf("pre jump\n");
        m_InputData[g_Config.m_ClDummy].m_Jump = 0;
        m_pre_jump_tick--;
        return;
    }

    if (m_jump_tick > 0)
    {
        //printf("jump\n");
        m_InputData[g_Config.m_ClDummy].m_Jump = 1;
        m_jump_tick--;
    }
}

void CControls::DoFire()
{
    m_InputData[g_Config.m_ClDummy].m_Fire++;
}

int CControls::SnapInput(int *pData)
{
	static int64 LastSendTime = 0;
	bool Send = false;

	// update player state
	//m_InputData[g_Config.m_ClDummy].m_PlayerFlags = PLAYERFLAG_CHATTING;
	//m_InputData[g_Config.m_ClDummy].m_PlayerFlags = PLAYERFLAG_IN_MENU;
	m_InputData[g_Config.m_ClDummy].m_PlayerFlags = PLAYERFLAG_PLAYING;

	//m_InputData[g_Config.m_ClDummy].m_PlayerFlags |= PLAYERFLAG_SCOREBOARD;

	/*
	if(m_InputData[g_Config.m_ClDummy].m_PlayerFlags != PLAYERFLAG_PLAYING)
		m_JoystickTapTime = 0; // Do not launch hook on first tap

	if (m_pClient->m_pControls->m_ShowHookColl[g_Config.m_ClDummy])
		m_InputData[g_Config.m_ClDummy].m_PlayerFlags |= PLAYERFLAG_AIM;
	*/

	if(m_LastData[g_Config.m_ClDummy].m_PlayerFlags != m_InputData[g_Config.m_ClDummy].m_PlayerFlags)
		Send = true;

	m_LastData[g_Config.m_ClDummy].m_PlayerFlags = m_InputData[g_Config.m_ClDummy].m_PlayerFlags;

	// we freeze the input if chat or menu is activated
	if(!(m_InputData[g_Config.m_ClDummy].m_PlayerFlags&PLAYERFLAG_PLAYING))
	{
		ResetInput(g_Config.m_ClDummy);

		mem_copy(pData, &m_InputData[g_Config.m_ClDummy], sizeof(m_InputData[0]));

		// send once a second just to be sure
		if(time_get() > LastSendTime + time_freq())
			Send = true;
	}
	else
	{
		m_InputData[g_Config.m_ClDummy].m_TargetX = (int)10;
		m_InputData[g_Config.m_ClDummy].m_TargetY = (int)10;
		if(!m_InputData[g_Config.m_ClDummy].m_TargetX && !m_InputData[g_Config.m_ClDummy].m_TargetY)
		{
			m_InputData[g_Config.m_ClDummy].m_TargetX = 1;
			m_MousePos[g_Config.m_ClDummy].x = 1;
		}

		// set direction
		m_InputData[g_Config.m_ClDummy].m_Direction = 0;
		if(m_InputDirectionLeft[g_Config.m_ClDummy] && !m_InputDirectionRight[g_Config.m_ClDummy])
			m_InputData[g_Config.m_ClDummy].m_Direction = -1;
		if(!m_InputDirectionLeft[g_Config.m_ClDummy] && m_InputDirectionRight[g_Config.m_ClDummy])
			m_InputData[g_Config.m_ClDummy].m_Direction = 1;

		// dummy copy moves
		if(g_Config.m_ClDummyCopyMoves)
		{
			CNetObj_PlayerInput *DummyInput = &Client()->m_DummyInput;
			DummyInput->m_Direction = m_InputData[g_Config.m_ClDummy].m_Direction;
			DummyInput->m_Hook = m_InputData[g_Config.m_ClDummy].m_Hook;
			DummyInput->m_Jump = m_InputData[g_Config.m_ClDummy].m_Jump;
			DummyInput->m_PlayerFlags = m_InputData[g_Config.m_ClDummy].m_PlayerFlags;
			DummyInput->m_TargetX = m_InputData[g_Config.m_ClDummy].m_TargetX;
			DummyInput->m_TargetY = m_InputData[g_Config.m_ClDummy].m_TargetY;
			DummyInput->m_WantedWeapon = m_InputData[g_Config.m_ClDummy].m_WantedWeapon;



			DummyInput->m_Fire += m_InputData[g_Config.m_ClDummy].m_Fire - m_LastData[g_Config.m_ClDummy].m_Fire;
			DummyInput->m_NextWeapon += m_InputData[g_Config.m_ClDummy].m_NextWeapon - m_LastData[g_Config.m_ClDummy].m_NextWeapon;
			DummyInput->m_PrevWeapon += m_InputData[g_Config.m_ClDummy].m_PrevWeapon - m_LastData[g_Config.m_ClDummy].m_PrevWeapon;

			m_InputData[!g_Config.m_ClDummy] = *DummyInput;
		}

		// stress testing
		if(g_Config.m_DbgStress)
		{
			float t = Client()->LocalTime();
			mem_zero(&m_InputData[g_Config.m_ClDummy], sizeof(m_InputData[0]));

			m_InputData[g_Config.m_ClDummy].m_Direction = ((int)t/2)&1;
			m_InputData[g_Config.m_ClDummy].m_Jump = ((int)t);
			m_InputData[g_Config.m_ClDummy].m_Fire = ((int)(t*10));
			m_InputData[g_Config.m_ClDummy].m_Hook = ((int)(t*2))&1;
			m_InputData[g_Config.m_ClDummy].m_WantedWeapon = ((int)t)%NUM_WEAPONS;
			m_InputData[g_Config.m_ClDummy].m_TargetX = (int)(sinf(t*3)*100.0f);
			m_InputData[g_Config.m_ClDummy].m_TargetY = (int)(cosf(t*3)*100.0f);
		}

		// ChillerDragon moves
		if (GameClient()->m_Snap.m_pLocalCharacter) //fng movebot
		{
			float t = Client()->LocalTime();
			mem_zero(&m_InputData[g_Config.m_ClDummy], sizeof(m_InputData[0]));
			m_InputData[g_Config.m_ClDummy].m_TargetX = (int)(sinf(t*3)*100.0f);
			m_InputData[g_Config.m_ClDummy].m_TargetY = (int)(cosf(t*3)*100.0f);
			//m_InputData[g_Config.m_ClDummy].m_Fire = ((int)(t*10));
            DoFire();
			m_InputData[g_Config.m_ClDummy].m_Direction = 0;
			m_InputData[g_Config.m_ClDummy].m_Jump = 0;
            DoJump();
            DoHook();
			if (GameClient()->m_Snap.m_pLocalCharacter->m_X < 128 * 32) //red base
			{
                m_InputData[g_Config.m_ClDummy].m_Direction = 1;
				//GameClient()->SendKill(g_Config.m_ClDummy);
			}
			if (GameClient()->m_Snap.m_pLocalCharacter->m_X > 170 * 32) //blue base
			{
                m_InputData[g_Config.m_ClDummy].m_Direction = -1;
				m_InputData[g_Config.m_ClDummy].m_Jump = 1;
			}


			if (GameClient()->m_Snap.m_pLocalCharacter->m_VelX < 0.5f)
			{
                StartJump(50, 100);
                /*
                m_InputData[g_Config.m_ClDummy].m_Jump = 1;
                if ((int)t % 5 == 0)
                    m_InputData[g_Config.m_ClDummy].m_Jump = 0;
                */
			}
		}

		// check if we need to send input
		if(m_InputData[g_Config.m_ClDummy].m_Direction != m_LastData[g_Config.m_ClDummy].m_Direction) Send = true;
		else if(m_InputData[g_Config.m_ClDummy].m_Jump != m_LastData[g_Config.m_ClDummy].m_Jump) Send = true;
		else if(m_InputData[g_Config.m_ClDummy].m_Fire != m_LastData[g_Config.m_ClDummy].m_Fire) Send = true;
		else if(m_InputData[g_Config.m_ClDummy].m_Hook != m_LastData[g_Config.m_ClDummy].m_Hook) Send = true;
		else if(m_InputData[g_Config.m_ClDummy].m_WantedWeapon != m_LastData[g_Config.m_ClDummy].m_WantedWeapon) Send = true;
		else if(m_InputData[g_Config.m_ClDummy].m_NextWeapon != m_LastData[g_Config.m_ClDummy].m_NextWeapon) Send = true;
		else if(m_InputData[g_Config.m_ClDummy].m_PrevWeapon != m_LastData[g_Config.m_ClDummy].m_PrevWeapon) Send = true;

		// send at at least 10hz
		if(time_get() > LastSendTime + time_freq()/25)
			Send = true;

		if(m_pClient->m_Snap.m_pLocalCharacter && m_pClient->m_Snap.m_pLocalCharacter->m_Weapon == WEAPON_NINJA
			&& (m_InputData[g_Config.m_ClDummy].m_Direction || m_InputData[g_Config.m_ClDummy].m_Jump || m_InputData[g_Config.m_ClDummy].m_Hook))
			Send = true;
	}

	// copy and return size
	m_LastData[g_Config.m_ClDummy] = m_InputData[g_Config.m_ClDummy];

	if(!Send)
		return 0;

	LastSendTime = time_get();
	mem_copy(pData, &m_InputData[g_Config.m_ClDummy], sizeof(m_InputData[0]));
	return sizeof(m_InputData[0]);
}

void CControls::OnRender()
{
	//ChillerDragon TODO: check this file for move inputs

	// update target pos
	if(m_pClient->m_Snap.m_pGameInfoObj && !m_pClient->m_Snap.m_SpecInfo.m_Active)
		m_TargetPos[g_Config.m_ClDummy] = m_pClient->m_LocalCharacterPos + m_MousePos[g_Config.m_ClDummy];
	else if(m_pClient->m_Snap.m_SpecInfo.m_Active && m_pClient->m_Snap.m_SpecInfo.m_UsePosition)
		m_TargetPos[g_Config.m_ClDummy] = m_pClient->m_Snap.m_SpecInfo.m_Position + m_MousePos[g_Config.m_ClDummy];
	else
		m_TargetPos[g_Config.m_ClDummy] = m_MousePos[g_Config.m_ClDummy];
}

bool CControls::OnMouseMove(float x, float y)
{
	return true;
}

void CControls::ClampMousePos()
{

}
