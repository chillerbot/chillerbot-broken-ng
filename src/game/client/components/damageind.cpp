/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <engine/demo.h>
#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/gamecore.h> // get_angle
#include "damageind.h"

CDamageInd::CDamageInd()
{
	m_Lastupdate = 0;
	m_NumItems = 0;
}

CDamageInd::CItem *CDamageInd::CreateI()
{
	if (m_NumItems < MAX_ITEMS)
	{
		CItem *p = &m_aItems[m_NumItems];
		m_NumItems++;
		return p;
	}
	return 0;
}

void CDamageInd::DestroyI(CDamageInd::CItem *i)
{
	m_NumItems--;
	*i = m_aItems[m_NumItems];
}

void CDamageInd::Create(vec2 Pos, vec2 Dir)
{
	CItem *i = CreateI();
	if (i)
	{
		i->m_Pos = Pos;
		i->m_StartTime = Client()->LocalTime();
		i->m_Dir = Dir*-1;
		i->m_StartAngle = (( (float)rand()/(float)RAND_MAX) - 1.0f) * 2.0f * pi;
	}
}

void CDamageInd::OnRender()
{

}

void CDamageInd::Reset()
{
	for(int i = 0; i < m_NumItems;)
	{
		DestroyI(&m_aItems[i]);
	}
}
