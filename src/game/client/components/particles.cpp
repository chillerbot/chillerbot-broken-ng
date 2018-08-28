/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <base/math.h>
#include <engine/demo.h>

#include <game/generated/client_data.h>
#include <game/gamecore.h>
#include "particles.h"

CParticles::CParticles()
{
	OnReset();
	m_RenderTrail.m_pParts = this;
	m_RenderExplosions.m_pParts = this;
	m_RenderGeneral.m_pParts = this;
}


void CParticles::OnReset()
{
	// reset particles
	for(int i = 0; i < MAX_PARTICLES; i++)
	{
		m_aParticles[i].m_PrevPart = i-1;
		m_aParticles[i].m_NextPart = i+1;
	}

	m_aParticles[0].m_PrevPart = 0;
	m_aParticles[MAX_PARTICLES-1].m_NextPart = -1;
	m_FirstFree = 0;

	for(int i = 0; i < NUM_GROUPS; i++)
		m_aFirstPart[i] = -1;
}

void CParticles::Add(int Group, CParticle *pPart)
{

}

void CParticles::Update(float TimePassed)
{

}

void CParticles::OnRender()
{

}

void CParticles::RenderGroup(int Group)
{

}
