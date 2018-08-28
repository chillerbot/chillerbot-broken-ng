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

	m_NeedRestartGraphics = false;
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
	if(UI()->ActiveItem() == pID)
		return vec4(1,1,1,0.5f);
	else if(UI()->HotItem() == pID)
		return vec4(1,1,1,1.5f);
	return vec4(1,1,1,1);
}

int CMenus::DoButton_Icon(int ImageId, int SpriteId, const CUIRect *pRect)
{
	return 0;
}

int CMenus::DoButton_Toggle(const void *pID, int Checked, const CUIRect *pRect, bool Active)
{
	return 0;
}

int CMenus::DoButton_Menu(const void *pID, const char *pText, int Checked, const CUIRect *pRect)
{
	return 0;
}

void CMenus::DoButton_KeySelect(const void *pID, const char *pText, int Checked, const CUIRect *pRect)
{

}

int CMenus::DoButton_MenuTab(const void *pID, const char *pText, int Checked, const CUIRect *pRect, int Corners)
{
	return 0;
}

int CMenus::DoButton_GridHeader(const void *pID, const char *pText, int Checked, const CUIRect *pRect)
{
	return 0;
}

int CMenus::DoButton_CheckBox_Common(const void *pID, const char *pText, const char *pBoxText, const CUIRect *pRect)
{
	return 0;
}

int CMenus::DoButton_CheckBox(const void *pID, const char *pText, int Checked, const CUIRect *pRect)
{
	return 0;
}


int CMenus::DoButton_CheckBox_Number(const void *pID, const char *pText, int Checked, const CUIRect *pRect)
{
	return 0;
}

int CMenus::DoEditBox(void *pID, const CUIRect *pRect, char *pStr, unsigned StrSize, float FontSize, float *Offset, bool Hidden, int Corners, const char *pEmptyText)
{
	return 0;
}

float CMenus::DoScrollbarV(const void *pID, const CUIRect *pRect, float Current)
{
	return 0.0f;
}



float CMenus::DoScrollbarH(const void *pID, const CUIRect *pRect, float Current)
{
	return 0.0f;
}

int CMenus::DoKeyReader(void *pID, const CUIRect *pRect, int Key)
{
	return 0;
}


int CMenus::RenderMenubar(CUIRect r)
{
	return 0;
}

void CMenus::RenderLoading()
{

}

void CMenus::RenderNews(CUIRect MainView)
{

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
#if defined(__ANDROID__)
	UI()->AndroidShowScreenKeys(!m_MenuActive && !m_pClient->m_pControls->m_UsingGamepad);
#endif
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

void CMenus::RenderBackground()
{

}

int CMenus::DoButton_CheckBox_DontCare(const void *pID, const char *pText, int Checked, const CUIRect *pRect)
{
	return 0;
}

void CMenus::RenderUpdating(const char *pCaption, int current, int total)
{

}
