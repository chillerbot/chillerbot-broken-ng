/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#include <math.h>

#include <base/math.h>

#include <engine/shared/config.h>
#include <engine/map.h>
#include <game/generated/client_data.h>
#include <game/generated/protocol.h>
#include <game/layers.h>
#include "animstate.h"
#include "render.h"

void CRenderTools::SelectSprite(CDataSprite *pSpr, int Flags, int sx, int sy)
{

}

void CRenderTools::SelectSprite(int Id, int Flags, int sx, int sy)
{

}

void CRenderTools::DrawSprite(float x, float y, float Size)
{

}

void CRenderTools::DrawRoundRectExt(float x, float y, float w, float h, float r, int Corners)
{

}

void CRenderTools::DrawRoundRect(float x, float y, float w, float h, float r)
{

}

void CRenderTools::DrawUIRect(const CUIRect *r, vec4 Color, int Corners, float Rounding)
{

}

void CRenderTools::DrawCircle(float x, float y, float r, int Segments)
{

}

void CRenderTools::RenderTee(CAnimState *pAnim, CTeeRenderInfo *pInfo, int Emote, vec2 Dir, vec2 Pos, bool Alpha)
{

}

static void CalcScreenParams(float Amount, float WMax, float HMax, float Aspect, float *w, float *h)
{
	float f = sqrtf(Amount) / sqrtf(Aspect);
	*w = f*Aspect;
	*h = f;

	// limit the view
	if(*w > WMax)
	{
		*w = WMax;
		*h = *w/Aspect;
	}

	if(*h > HMax)
	{
		*h = HMax;
		*w = *h*Aspect;
	}
}

void CRenderTools::MapscreenToWorld(float CenterX, float CenterY, float ParallaxX, float ParallaxY,
	float OffsetX, float OffsetY, float Aspect, float Zoom, float *pPoints)
{
	float Width, Height;
	CalcScreenParams(1150*1000, 1500, 1050, Aspect, &Width, &Height);
	CenterX *= ParallaxX;
	CenterY *= ParallaxY;
	Width *= Zoom;
	Height *= Zoom;
	pPoints[0] = OffsetX+CenterX-Width/2;
	pPoints[1] = OffsetY+CenterY-Height/2;
	pPoints[2] = pPoints[0]+Width;
	pPoints[3] = pPoints[1]+Height;
}

void CRenderTools::RenderTilemapGenerateSkip(class CLayers *pLayers)
{
	for(int g = 0; g < pLayers->NumGroups(); g++)
	{
		CMapItemGroup *pGroup = pLayers->GetGroup(g);

		for(int l = 0; l < pGroup->m_NumLayers; l++)
		{
			CMapItemLayer *pLayer = pLayers->GetLayer(pGroup->m_StartLayer+l);

			if(pLayer->m_Type == LAYERTYPE_TILES)
			{
				CMapItemLayerTilemap *pTmap = (CMapItemLayerTilemap *)pLayer;
				CTile *pTiles = (CTile *)pLayers->Map()->GetData(pTmap->m_Data);
				for(int y = 0; y < pTmap->m_Height; y++)
				{
					for(int x = 1; x < pTmap->m_Width;)
					{
						int sx;
						for(sx = 1; x+sx < pTmap->m_Width && sx < 255; sx++)
						{
							if(pTiles[y*pTmap->m_Width+x+sx].m_Index)
								break;
						}

						pTiles[y*pTmap->m_Width+x].m_Skip = sx-1;
						x += sx;
					}
				}
			}
		}
	}
}
