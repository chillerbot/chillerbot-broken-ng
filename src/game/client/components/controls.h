/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_CONTROLS_H
#define GAME_CLIENT_COMPONENTS_CONTROLS_H
#include <base/vmath.h>
#include <base/system.h>
#include <game/client/component.h>

class CControls : public CComponent
{
public:
	vec2 m_MousePos[2];
	vec2 m_TargetPos[2];
	float m_OldMouseX;
	float m_OldMouseY;
	bool m_JoystickFirePressed;
	bool m_JoystickRunPressed;
	int64 m_JoystickTapTime;

	bool m_UsingGamepad;

	int m_AmmoCount[NUM_WEAPONS];

	CNetObj_PlayerInput m_InputData[2];
	CNetObj_PlayerInput m_LastData[2];
	int m_InputDirectionLeft[2];
	int m_InputDirectionRight[2];
	int m_ShowHookColl[2];
	int m_LastDummy;
	int m_OtherFire;

	CControls();

	virtual void OnReset();
	virtual void OnRelease();
	virtual void OnRender();
	virtual void OnMessage(int MsgType, void *pRawMsg);
	virtual bool OnMouseMove(float x, float y);
	virtual void OnConsoleInit();
	virtual void OnPlayerDeath();

	int SnapInput(int *pData);
	void ClampMousePos();
	void ResetInput(int dummy);

    //ChillerDragon
    //hook
    void DoHook();
    void StartHook(int pre, int hook);
    int m_hook_tick;
    int m_pre_hook_tick;
    //jump
    void DoJump();
    void StartJump(int pre, int jump);
    int m_jump_tick;
    int m_pre_jump_tick;
    //fire
    void DoFire();
	//hax
	vec2 GetClosestPlayerPos(vec2 Pos, float Radius, const CNetObj_Character *pNotThis);
};
#endif
