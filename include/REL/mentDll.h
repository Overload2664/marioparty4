#ifndef REL_MENTDLL_H
#define REL_MENTDLL_H

#include "game/object.h"
extern s32 menuSoundFXTbl[][4];

// the prev and current struct member might be vice versa
typedef struct MentDllUnkBss64Struct {
    /* 0x00 */ void (*func)(void);
    /* 0x04 */ void (*func2)(f32 arg9);
    /* 0x08 */ Vec center;
    /* 0x14 */ Vec prevCenter;
    /* 0x20 */ Vec rot;
    /* 0x2C */ Vec prevRot;
    /* 0x38 */ f32 zoom;
    /* 0x3C */ f32 prevZoom;
    /* 0x40 */ s32 frames;
} MenuCamera; /* size = 0x44 */
extern MenuCamera menuCamera;

f32 LerpClamped(f32 start, f32 end, f32 weight, f32 max_weight);
f32 WeightedBlend(f32 start, f32 end, f32 weight);
f32 CosEaseClamped(f32 start, f32 end, f32 progress, f32 maxProgress);
f32 SinEaseClamped(f32 start, f32 end, f32 progress, f32 maxProgress);
f32 SinOscillateClamped(f32 start, f32 end, f32 progress, f32 maxProgress);
void MenuPrcSleep(s32 time);
void MenuPrcVSleep(void);
void MenuLightInit(void);
void MenuShadowInit(s32 eventNo);
void MenuWinInit(void);
s32 OpenWinBottom(s32 xAlign, s32 arg1, s32 arg2);
void DestroyWin(s32 winId);
void WinSetMessAndWait(s32 winId, s32 message_id, s32 max_waits, s32 sleep_duration);
s32 OpenConfirmDlgYesDef(s32 mess, s32 mode, s32 arg2);
s32 OpenConfirmDlgNoDef(s32 mess, s32 mode, s32 arg2);
s32 OpenAvailControlsWin(s32 mess);
void destroyAvailControlsWin(s32 arg0);
void moveCameraWithMethod(void (*camera_move_method)(void));
void MenuCameraInit(HUPROCESS *objman_process, void (*camera_intro_method)(void));
void MenuCameraSnapshot(MenuCamera *camera);
void MenuCameraSinEaseFollow(MenuCamera *srcCamera, MenuCamera *targetCamera, f32 progress, f32 maxProgress, f32 weight);
void MenuCameraCosEaseFollow(MenuCamera *srcCamera, MenuCamera *targetCamera, f32 progress, f32 maxProgress, f32 weight);
void motionShift(OMOBJ *obj, s32 mdlId, s32 mtnId, s32 shiftTime, s32 attr);
void motionShiftIfChanged(OMOBJ *obj, s32 mdlId, s32 mtnId, s32 shiftTime, s32 attr);
void motionShiftTick(OMOBJ *obj);
void WaitAnimEnd(OMOBJ *obj, s32 model_id, s32 extra_initial_delay);
void MenuMoveChar(OMOBJ *obj, s32 mdlId, Vec targePos, float endRotAngle, float speed, float rotDur, s32 enableMove, s32 enableRot);
void sprPosSet_YPadded(s32 grpId, s32 memberNo, f32 posX, f32 posY);
void MenuMain(HUPROCESS *objman);

#endif
