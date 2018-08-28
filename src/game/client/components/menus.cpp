/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <base/tl/array.h>
#include <sstream>
#include <string>

#include <math.h>

#include <base/system.h>
#include <base/math.h>
#include <base/vmath.h>

#include <engine/config.h>
#include <engine/engine.h>
#include <engine/friends.h>
#include <engine/keys.h>
#include <engine/serverbrowser.h>
#include <engine/storage.h>
#include <engine/shared/config.h>

#include <game/version.h>
#include <game/generated/protocol.h>

#include <game/generated/client_data.h>
#include <game/client/gameclient.h>
#include <game/client/lineinput.h>
#include <mastersrv/mastersrv.h>
#include <versionsrv/versionsrv.h>

#include "countryflags.h"
#include "menus.h"
#include "skins.h"
#include "controls.h"

vec4 CMenus::ms_ColorTabbarInactiveOutgame;
vec4 CMenus::ms_ColorTabbarActiveOutgame;
vec4 CMenus::ms_ColorTabbarInactive;
vec4 CMenus::ms_ColorTabbarActive = vec4(0,0,0,0.5f);
vec4 CMenus::ms_ColorTabbarInactiveIngame;
vec4 CMenus::ms_ColorTabbarActiveIngame;

#if defined(__ANDROID__)
float CMenus::ms_ButtonHeight = 50.0f;
float CMenus::ms_ListheaderHeight = 17.0f;
float CMenus::ms_ListitemAdditionalHeight = 33.0f;
#else
float CMenus::ms_ButtonHeight = 25.0f;
float CMenus::ms_ListheaderHeight = 17.0f;
#endif
float CMenus::ms_FontmodHeight = 0.8f;

IInput::CEvent CMenus::m_aInputEvents[MAX_INPUTEVENTS];
int CMenus::m_NumInputEvents;


CMenus::CMenus()
{
	m_Popup = POPUP_NONE;
	m_ActivePage = PAGE_INTERNET;
	m_GamePage = PAGE_GAME;

	m_NeedSendinfo = false;
	m_NeedSendDummyinfo = false;
	m_MenuActive = true;
	m_UseMouseButtons = true;
	m_MouseSlow = false;

	m_EscapePressed = false;
	m_EnterPressed = false;
	m_DeletePressed = false;
	m_NumInputEvents = 0;

	m_LastInput = time_get();

	str_copy(m_aCurrentDemoFolder, "demos", sizeof(m_aCurrentDemoFolder));
	m_aCallvoteReason[0] = 0;

	m_FriendlistSelectedIndex = -1;
	m_DoubleClickIndex = -1;

	m_DemoPlayerState = DEMOPLAYER_NONE;
	m_Dummy = false;
}

vec4 CMenus::ButtonColorMul(const void *pID)
{
	return vec4(1,1,1,1);
}

void CMenus::OnInit()
{
	g_Config.m_ClShowWelcome = 0;

	Console()->Chain("add_favorite", ConchainServerbrowserUpdate, this);
	Console()->Chain("remove_favorite", ConchainServerbrowserUpdate, this);
	Console()->Chain("add_friend", ConchainFriendlistUpdate, this);
	Console()->Chain("remove_friend", ConchainFriendlistUpdate, this);

	// setup load amount
	m_LoadCurrent = 0;
	m_LoadTotal = g_pData->m_NumImages;
}

void CMenus::PopupMessage(const char *pTopic, const char *pBody, const char *pButton)
{

}


int CMenus::Render()
{
	return 0;
}


void CMenus::SetActive(bool Active)
{
	m_MenuActive = Active;
	if(!m_MenuActive)
	{
		if(m_NeedSendinfo)
		{
			m_pClient->SendInfo(false);
			m_NeedSendinfo = false;
		}

		if(m_NeedSendDummyinfo)
		{
			m_pClient->SendDummyInfo(false);
			m_NeedSendDummyinfo = false;
		}

		if(Client()->State() == IClient::STATE_ONLINE)
		{
			m_pClient->OnRelease();
		}
	}
	else if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
	{
		m_pClient->OnRelease();
	}
}

void CMenus::OnReset()
{
}

bool CMenus::OnMouseMove(float x, float y)
{
	return false;
}

bool CMenus::OnInput(IInput::CEvent e)
{
	return false;
}

void CMenus::OnStateChange(int NewState, int OldState)
{

}

extern "C" void font_debug_render();

void CMenus::OnRender()
{
	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		SetActive(true);

	if(Client()->State() == IClient::STATE_ONLINE && m_pClient->m_ServerMode == m_pClient->SERVERMODE_PUREMOD)
	{
		Client()->Disconnect();
		SetActive(true);
		m_Popup = POPUP_PURE;
	}

	if(!IsActive())
	{
		m_EscapePressed = false;
		m_EnterPressed = false;
		m_DeletePressed = false;
		m_NumInputEvents = 0;
		return;
	}
}