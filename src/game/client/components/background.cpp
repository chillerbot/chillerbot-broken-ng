#include <base/system.h>
#include <iostream>

#include <engine/shared/config.h>
#include <engine/map.h>

#include <game/client/components/camera.h>
#include <game/client/components/maplayers.h>

#include "background.h"

CBackground::CBackground()
{
	m_pLayers = new CMapLayers(CMapLayers::TYPE_BACKGROUND);
	m_pLayers->m_pLayers = new CLayers;
	m_pBackgroundLayers = m_pLayers->m_pLayers;
	//m_pImages = new CMapImages;
	m_pBackgroundImages = m_pImages;
	m_pMap = CreateEngineMap();
	m_pBackgroundMap = m_pMap;
	m_Loaded = false;
	m_aMapName[0] = '\0';
}

CBackground::~CBackground()
{
	if(m_pLayers->m_pLayers != GameClient()->Layers())
	{
		delete m_pLayers->m_pLayers;
		delete m_pLayers;
	}
}

void CBackground::OnInit()
{
	m_pLayers->m_pClient = GameClient();
	Kernel()->ReregisterInterface(static_cast<IEngineMap*>(m_pMap));
	if(g_Config.m_ClBackgroundEntities[0] != '\0' && str_comp(g_Config.m_ClBackgroundEntities, CURRENT))
		LoadBackground();
}

void CBackground::LoadBackground()
{

}

void CBackground::OnMapLoad()
{
	if(str_comp(g_Config.m_ClBackgroundEntities, CURRENT) == 0 || str_comp(g_Config.m_ClBackgroundEntities, m_aMapName))
		LoadBackground();
}

//code is from CMapLayers::OnRender()
void CBackground::OnRender()
{
	return;

	//probably not the best place for this
	if(g_Config.m_ClBackgroundEntities[0] != '\0' && str_comp(g_Config.m_ClBackgroundEntities, m_aMapName))
		LoadBackground();

	if(!m_Loaded)
		return;

	if(Client()->State() != IClient::STATE_ONLINE && Client()->State() != IClient::STATE_DEMOPLAYBACK)
		return;

	if(g_Config.m_ClOverlayEntities != 100)
		return;
}
