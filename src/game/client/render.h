/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_RENDER_H
#define GAME_CLIENT_RENDER_H

#include <base/vmath.h>
#include <game/mapitems.h>

class CTeeRenderInfo
{
public:
	CTeeRenderInfo()
	{
		m_Texture = -1;
		m_ColorBody = vec4(1,1,1,1);
		m_ColorFeet = vec4(1,1,1,1);
		m_Size = 1.0f;
		m_GotAirJump = 1;
	};

	int m_Texture;
	vec4 m_ColorBody;
	vec4 m_ColorFeet;
	float m_Size;
	int m_GotAirJump;
};

// sprite renderings
enum
{
	SPRITE_FLAG_FLIP_Y=1,
	SPRITE_FLAG_FLIP_X=2,

	LAYERRENDERFLAG_OPAQUE=1,
	LAYERRENDERFLAG_TRANSPARENT=2,

	TILERENDERFLAG_EXTEND=4,
};

typedef void (*ENVELOPE_EVAL)(float TimeOffset, int Env, float *pChannels, void *pUser);

class CRenderTools
{
public:

	//typedef struct SPRITE;

	void SelectSprite(struct CDataSprite *pSprite, int Flags=0, int sx=0, int sy=0);
	void SelectSprite(int id, int Flags=0, int sx=0, int sy=0);

	void DrawSprite(float x, float y, float size);

	// rects
	void DrawRoundRect(float x, float y, float w, float h, float r);
	void DrawRoundRectExt(float x, float y, float w, float h, float r, int Corners);

	void DrawCircle(float x, float y, float r, int Segments);

	// larger rendering methods
	void RenderTilemapGenerateSkip(class CLayers *pLayers);

	// object render methods (gc_render_obj.cpp)
	void RenderTee(class CAnimState *pAnim, CTeeRenderInfo *pInfo, int Emote, vec2 Dir, vec2 Pos, bool Alpha = false);

	// helpers
	void MapscreenToWorld(float CenterX, float CenterY, float ParallaxX, float ParallaxY,
		float OffsetX, float OffsetY, float Aspect, float Zoom, float *pPoints);
};

#endif
