#include "game/board/main.h"
#include "REL/mentDll.h"
#include "dolphin/os.h"
#include "ext_math.h"
#include "game/armem.h"
#include "game/chrman.h"
#include "game/hsfex.h"
#include "game/hu3d.h"
#include "game/minigame_seq.h"
#include "game/objsub.h"
#include "game/pad.h"
#include "game/printfunc.h"
#include "game/saveload.h"
#include "game/sprite.h"
#include "game/window.h"
#include "game/wipe.h"

#ifndef __MWERKS__
extern s32 rand8(void);
#include "game/audio.h"
#endif

// Similar to ffs
typedef struct MentDllUnkBssE4Struct {
    /* 0x00 */ s32 air_change_dir_timer;
    /* 0x04 */ s32 state;
    /* 0x08 */ s32 rot_speed;
    /* 0x0C */ float x_speed;
    /* 0x10 */ float land_rot_dir;
    /* 0x14 */ float z_speed;
    /* 0x18 */ float ground_timer;
} MentDllUnkBssE4Struct; /* size = 0x1C */

typedef struct MentDllUnkBss16C4Struct {
    /* 0x00 */ s32 unk_00;
    /* 0x04 */ s32 unk_04;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ float unk_0C;
    /* 0x10 */ float unk_10;
    /* 0x14 */ float unk_14;
    /* 0x18 */ float unk_18;
    /* 0x1C */ float unk_1C;
    /* 0x20 */ float unk_20;
    /* 0x24 */ float unk_24;
    /* 0x28 */ float unk_28;
    /* 0x2C */ float unk_2C;
} MentDllUnkBss16C4Struct; /* size = 0x30 */

typedef void (*MentDllUnkFunc)(OMOBJ *, ...);

// unk_70[0] checks if a player has selected their character or not, 0 means not selected and 1 means selected
// mtPlayerData[i].unk_70[3] = mtPlayerData[i].iscom = 0;
typedef struct MTPlayerConfig {
    /* 0x00 */ OMOBJ *obj;
    /* 0x04 */ MentDllUnkFunc proc_selection_handler;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ char pad_10[0x48]; /* maybe part of unk_0C[0x13]? */
    /* 0x58 */ s32 character_highlighted;
    /* 0x5C */ s32 group;
    /* 0x60 */ s32 iscom;
    /* 0x64 */ s32 diff;
    /* 0x68 */ s32 character;
    /* 0x6C */ s32 pad_idx;
    /* 0x70 */ s32 unk_70[4];
    /* 0x80 */ s32 unk_80[4];
} MTPlayerConfig; /* size = 0x90 */

// typedef struct player_state {
// /* 0x00 */ struct {
//         u16 diff : 2;
//         u16 com : 1;
//         u16 character : 4;
//         u16 auto_size : 2;
//         u16 draw_ticket : 1;
//         u16 ticket_player : 6;
//     };
// /* 0x02 */ struct {
//         u8 team : 1;
//         u8 spark : 1;
//         u8 player_idx : 2;
//     };
// /* 0x03 */ s8 handicap;
// /* 0x04 */ s8 port;
// /* 0x05 */ s8 items[3];
// /* 0x08 */ struct {
//         u16 color : 2;
//         u16 moving : 1;
//         u16 jump : 1;
//         u16 show_next : 1;
//         u16 size : 2;
//         u16 num_dice : 2;
//         u16 rank : 2;
//         u16 bowser_suit : 1;
//         u16 team_backup : 1;
//     };
// /* 0x0A */ s8 roll;
// /* 0x0C */ s16 space_curr;
// /* 0x0E */ s16 space_prev;
// /* 0x10 */ s16 space_next;
// /* 0x12 */ s16 space_shock;
// /* 0x14 */ s8 blue_count;
// /* 0x15 */ s8 red_count;
// /* 0x16 */ s8 question_count;
// /* 0x17 */ s8 fortune_count;
// /* 0x18 */ s8 bowser_count;
// /* 0x19 */ s8 battle_count;
// /* 0x1A */ s8 mushroom_count;
// /* 0x1B */ s8 warp_count;
// /* 0x1C */ s16 coins;
// /* 0x1E */ s16 coins_mg;
// /* 0x20 */ s16 coins_total;
// /* 0x22 */ s16 coins_max;
// /* 0x24 */ s16 coins_battle;
// /* 0x26 */ s16 coin_collect;
// /* 0x28 */ s16 coin_win;
// /* 0x2A */ s16 stars;
// /* 0x2C */ s16 stars_max;
// /* 0x2E */ char unk_2E[2];
// } PlayerState; //size of 0x30

typedef struct MentDllUnkBss33ACStruct { // maybe the same as the other one?
    /* 0x00 */ OMOBJ *obj;
    /* 0x04 */ MentDllUnkFunc objCallback;
    /* 0x08 */ s32 unk_08;
    /* 0x0C */ s32 unk_0C;
    /* 0x10 */ char pad_10[0x20];
    /* 0x30 */ s32 idx;
    /* 0x34 */ s32 unk_34;
    /* 0x38 */ char pad_38[0x20];
} MentDllUnkBss33ACStruct; /* size = 0x58 */
// story_mg_list = board_configs[0].board_settings[1];
typedef struct MentBoardMenuConfig {
    /* 0x00 */ OMOBJ *obj;
    /* 0x04 */ MentDllUnkFunc obj_callback;
    /* 0x08 */ s32 handicaps[5]; // handicaps[0]: current_setting_section
    /* 0x1C */ s32 board_settings[5]; // board_settings[0]: has_team | board_settings[1]: max_turn | board_settings[2]: mg_list | board_settings[3]: has_bonus_star | board_settings[4]: current player handicap selection
    /* 0x30 */ s32 choosecharacter_group;
    /* 0x34 */ s32 chosen_characters_overlay_group;
    /* 0x38 */ s32 chooseboard_group;
    /* 0x3C */ s32 boardsettings_group;
    /* 0x40 */ s32 humancount_group;
    /* 0x44 */ s32 settings_text_win[5]; // Windows holding settings things like "Teams", "Turns", "Mini-Games", "Bonus", "Handicap"
} MentBoardMenuConfig; /* size = 0x58 */

void runMenu(void);
void oscillateChest(OMOBJ *obj, MentBoardMenuConfig *board_config);
void createAndOpenChest(OMOBJ *obj, s32 map);
void createBoardMenu(void); // Creates the backdrop and the menuing required to fire up a game
void chooseBoardSillyGuyPos(OMOBJ *obj, MentDllUnkBss33ACStruct *silly_guy);
void createSillyGuy(void);
void fn_1_134A8(void);
void initPlayer(void);
void MenuCameraFastIntro(void);
void MenuCameraMoveThroughIntro(void);
void MenuCameraSlowIntro(void);
void MenuIntroBackOffAfterSillyGuys(void);
void fn_1_14058(void);
void fn_1_14148(void);
void fn_1_14238(void);
void fn_1_14328(void);
void fn_1_14418(void);
void fn_1_146D0(void);
void fn_1_147C0(void);
void fn_1_148B0(void);
void fn_1_149A0(void);
void fn_1_14A2C(void);
void fn_1_14AB8(void);
void hideChooseCharacters(void);
void pushDownCharacterSelection(void);
void moveUpCharacterSelction(void);
void createChooseCharacterGroup(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3);
void character_selection_grid_init(void);
void human_character_select(OMOBJ *arg0, MTPlayerConfig *arg1);
s32 fn_1_1648C(MTPlayerConfig *arg0);
void computer_character_select(OMOBJ *arg0, MTPlayerConfig *arg1);
void computer_character_selection_draw(MTPlayerConfig *arg0);
void removeComputerHighlight(MTPlayerConfig *player);
void resetChosenCharactersOverlay(void);
void createChosenCharactersOverlayGroup(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3);
void pushDownChosenCharactersOverlay(void);
void pushUpChosenCharactersOverlay(void);
void resetBoardOverviews(void);
void createChooseBoardGroup(MentBoardMenuConfig *game_config, s32 arg1, s32 arg2, s32 arg3);
void fn_1_18A54(s32 arg0);
void RescaleChooseBoardGrp(void);
void fn_1_18F74(OMOBJ *arg0, MentBoardMenuConfig *arg1);
void hideBowserGnarlyBoardSettings(void);
void configureBoardWithController(OMOBJ *board_menu_obj, MentBoardMenuConfig *board_config);
void createChooseBoardSettingsGroup(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3);
void setupSettingSelectionCursor(void);
void hideHighlighter(void);
void pushDownBoardSettings(void);
void pushUpBoardSettings(void);
void initCharacterSelectionPos(void);
void createChooseHumanCountGroupStory(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3);
void fn_1_1DED8(void);
void fn_1_1DF48(void);
void pushDownStorySettings(void);
void fn_1_1E1B4(void);
void selectStoryCharacterCallback(OMOBJ *arg0, MTPlayerConfig *player);
void selectStorySettingValesCallback(OMOBJ *arg0, MentBoardMenuConfig *board_config0);
void createChosenCharactersOverlayGroupStory(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3);
void fn_1_1F868(void);
void fn_1_1FA34(void);
void fn_1_1FC54(void);
void fn_1_1FF4C(OMOBJ *arg0, MentBoardMenuConfig *arg1);
void moveHumancountGroup(s32 arg0);
void human_count_menu_exit(void);
void fn_1_208F4(void);
void fn_1_20A24(void);
void createChooseHumanCountGroupParty(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3);
void MenuCreateBubbles(OMOBJ *object);
void createBirthdayConfetti(OMOBJ *object);
void goToNextOverlay(s32 next_overlay);
void fn_1_E654(s32 arg0);
void fn_1_E71C(s32 arg0);

Vec silly_guys_pos[6] = {
    { 0.0f, 0.0f, 460.0f },
    { -240.0f, 0.0f, 460.0f },
    { -120.0f, 0.0f, 460.0f },
    { 120.0f, 50.0f, 460.0f },
    { 240.0f, 0.0f, 460.0f },
    { 1200.0f, 0.0f, 460.0f },
};

float silly_guys_angle[6] = { 0.0f, 20.0f, 10.0f, -10.0f, -20.0f, 0.0f };

MentBoardMenuConfig menu_configuration_handler_holder;
                // board_configs[0].unk_1C[1] = mg_list;
                // board_configs[0].unk_1C[0] = GWPlayerCfg[0].diff;
MentBoardMenuConfig board_configs[2];
MentDllUnkBss33ACStruct silly_guys[6];
MentDllUnkBss33ACStruct lbl_1_bss_3354;
MTPlayerConfig mtPlayerData[4];
OMOBJ *MenuObject[3];
OMOBJ *birthdayConfettiObj;
MentDllUnkBss16C4Struct bubbles[0x8C];
MentDllUnkBssE4Struct confetti[0xC8];
s32 story_mg_list;
s32 lbl_1_bss_DC;
s32 is_board_loaded;
s32 player_index;
s32 silly_guy_idx;
s32 background_number;
HUPROCESS *menuProc;
// game_configs[0] = omovlevtno; (mode)
// game_configs[1] = omovlstat;
// game_configs[2] = map;
// game_configs[4] = GWGameStat.open_w06;
// game_configs[5] = GWGameStat.veryHardUnlock;
// game_configs[6] = GWGameStat.customPackEnable;
s32 gameConfigs[8];
MenuCamera menuCamera;
s32 lbl_1_bss_24[16];
s32 audio_channel_idx[4]; // Only audio_channel[0] is used
s32 lbl_1_bss_8[3];

void MenuMain(HUPROCESS *objman)
{
    s32 i;

    menuProc = objman;
    MenuWinInit();
    MenuLightInit();
    MenuShadowInit(gameConfigs[0]);
    if (gameConfigs[0] != 0xB) {
        CharDataClose(-1);
    }
    gameConfigs[4] = GWGameStat.open_w06;
    gameConfigs[5] = GWGameStat.veryHardUnlock;
    gameConfigs[6] = GWGameStat.customPackEnable;
    switch (gameConfigs[0]) {
        case 0:
            HuAudSndGrpSetSet(7);
            if (gameConfigs[1] == 0) {
                MenuCameraInit(menuProc, MenuCameraFastIntro);
            }
            else {
                MenuCameraInit(menuProc, MenuCameraSlowIntro);
            }
            for (i = 0; i < 2; i++) {
                HuPrcChildCreate(createBoardMenu, 0x64, 0x3000, 0, HuPrcCurrentGet());
            }
            for (i = 0; i < 6; i++) {
                HuPrcChildCreate(createSillyGuy, 0x64, 0x3000, 0, HuPrcCurrentGet());
            }
            for (i = 0; i < 4; i++) {
                HuPrcChildCreate(initPlayer, 0x64, 0x3000, 0, HuPrcCurrentGet());
            }
            if (gameConfigs[1] == 0) {
                MenuObject[0] = omAddObjEx(menuProc, 0x1000, 0x8D, 0x10, -1, MenuCreateBubbles);
            }
            break;
        case 1:
            HuAudSndGrpSetSet(7);
            if (gameConfigs[1] == 0) {
                MenuCameraInit(menuProc, MenuCameraFastIntro);
            }
            else {
                MenuCameraInit(menuProc, MenuCameraSlowIntro);
            }
            HuPrcChildCreate(createBoardMenu, 0x64, 0x3000, 0, HuPrcCurrentGet());
            for (i = 0; i < 5; i++) {
                HuPrcChildCreate(createSillyGuy, 0x64, 0x3000, 0, HuPrcCurrentGet());
            }
            HuPrcChildCreate(initPlayer, 0x64, 0x3000, 0, HuPrcCurrentGet());
            if (gameConfigs[1] == 0) {
                MenuObject[0] = omAddObjEx(menuProc, 0x1000, 0x8D, 0x10, -1, MenuCreateBubbles);
            }
            birthdayConfettiObj = omAddObjEx(menuProc, 0x1000, 0xC9, 0x10, -1, createBirthdayConfetti);
            break;
        case 3:
            HuAudSndCommonGrpSet(0x56, 1);
            HuAudSndCommonGrpSet(0x6B, 0);
            MenuCameraInit(menuProc, MenuCameraFastIntro);
            HuPrcChildCreate(createBoardMenu, 0x64, 0x3000, 0, HuPrcCurrentGet());
            silly_guy_idx = 4;
            HuPrcChildCreate(createSillyGuy, 0x64, 0x3000, 0, HuPrcCurrentGet());
            MenuObject[0] = omAddObjEx(menuProc, 0x1000, 0x8D, 0x10, -1, MenuCreateBubbles);
            break;
        case 4:
            HuAudSndGrpSetSet(3);
            MenuCameraInit(menuProc, MenuCameraFastIntro);
            HuPrcChildCreate(createBoardMenu, 0x64, 0x3000, 0, HuPrcCurrentGet());
            silly_guy_idx = 2;
            HuPrcChildCreate(createSillyGuy, 0x64, 0x3000, 0, HuPrcCurrentGet());
            MenuObject[0] = omAddObjEx(menuProc, 0x1000, 0x8D, 0x10, -1, MenuCreateBubbles);
            break;
        case 5:
            MenuCameraInit(menuProc, MenuCameraFastIntro);
            HuPrcChildCreate(createBoardMenu, 0x64, 0x3000, 0, HuPrcCurrentGet());
            silly_guy_idx = 3;
            HuPrcChildCreate(createSillyGuy, 0x64, 0x3000, 0, HuPrcCurrentGet());
            MenuObject[0] = omAddObjEx(menuProc, 0x1000, 0x8D, 0x10, -1, MenuCreateBubbles);
            break;
        case 10:
            HuAudSndGrpSetSet(7);
            MenuCameraInit(menuProc, fn_1_149A0);
            HuPrcChildCreate(createBoardMenu, 0x64, 0x3000, 0, HuPrcCurrentGet());
            for (i = 0; i < 5; i++) {
                HuPrcChildCreate(createSillyGuy, 0x64, 0x3000, 0, HuPrcCurrentGet());
            }
            HuPrcChildCreate(initPlayer, 0x64, 0x3000, 0, HuPrcCurrentGet());
            break;
        case 11:
            background_number = 1;
            HuAudSndGrpSetSet(7);
            MenuCameraInit(menuProc, fn_1_14A2C);
            HuPrcChildCreate(createBoardMenu, 0x64, 0x3000, 0, HuPrcCurrentGet());
            HuPrcChildCreate(fn_1_134A8, 0x64, 0x3000, 0, HuPrcCurrentGet());
            MenuPrcSleep(2);
            HuDataDirClose(DATADIR_MENT);
            HuPrcChildCreate(initPlayer, 0x64, 0x3000, 0, HuPrcCurrentGet());
            break;
    }
    HuPrcChildCreate(runMenu, 0xC8, 0x3000, 0, HuPrcCurrentGet());
}

void fn_1_6C4C(void)
{
    s32 i;
    s32 var_r30 = 0x10;
    s32 var_r29 = 0x28;
    s32 var_r28 = 0;
    {
        GXColor sp10 = { 0x00, 0x00, 0x80, 0x80 };
        while (1) {
            MenuPrcVSleep();
            if (HuPadBtnDown[0] & PAD_TRIGGER_R) {
                var_r28++;
                var_r28 = var_r28 % 2;
            }
            if (var_r28 == 0) {
                continue;
            }
            printWin(var_r30, var_r29, 0xDC, 0x122, &sp10);
            if (menu_configuration_handler_holder.obj_callback == NULL) {
                print8(var_r30, var_r29, 1.0f, "PROC_MAIN    -> FALSE");
            }
            else {
                print8(var_r30, var_r29, 1.0f, "PROC_MAIN    -> TRUE");
            }
            for (i = 0; i < 2; i++) {
                if (board_configs[i].obj_callback == NULL) {
                    print8(var_r30, var_r29 + 0x14 + (i * 0xA), 1.0f, "PROC_MAP%d    -> FALSE", i);
                }
                else {
                    print8(var_r30, var_r29 + 0x14 + (i * 0xA), 1.0f, "PROC_MAP%d    -> TRUE", i);
                }
            }
            for (i = 0; i < 6; i++) {
                if (silly_guys[i].objCallback == NULL) {
                    print8(var_r30, var_r29 + 0x32 + (i * 0xA), 1.0f, "PROC_SUB%d    -> FALSE", i);
                }
                else {
                    print8(var_r30, var_r29 + 0x32 + (i * 0xA), 1.0f, "PROC_SUB%d    -> TRUE", i);
                }
            }
            for (i = 0; i < 4; i++) {
                if (mtPlayerData[i].proc_selection_handler == NULL) {
                    print8(var_r30, var_r29 + 0x78 + (i * 0xA), 1.0f, "PROC_PLAYER%d -> FALSE", i);
                }
                else {
                    print8(var_r30, var_r29 + 0x78 + (i * 0xA), 1.0f, "PROC_PLAYER%d -> TRUE", i);
                }
            }
            print8(var_r30, var_r29 + 0xAA, 1.0f, "MODE:%d MAP:%d", gameConfigs[0], gameConfigs[2]);
            print8(var_r30, var_r29 + 0xBE, 1.0f, "NO) IDX:GRP:COM:DIF:PAD:CHR");
            for (i = 0; i < 4; i++) {

                print8(var_r30, var_r29 + 0xC8 + (i * 0xA), 1.0f, "%2d) %3d:%3d:%3d:%3d:%3d:%3d", i, mtPlayerData[i].character_highlighted,
                    mtPlayerData[i].group, mtPlayerData[i].iscom, mtPlayerData[i].diff, mtPlayerData[i].pad_idx, mtPlayerData[i].character);
            }
            print8(var_r30, var_r29 + 0xF0, 1.0f, "NO) FG0:FG1:FG2:FG3");
            for (i = 0; i < 4; i++) {
                print8(var_r30, var_r29 + 0xFA + (i * 0xA), 1.0f, "%2d) %3d:%3d:%3d:%3d", i, mtPlayerData[i].unk_70[0],
                    mtPlayerData[i].unk_70[1], mtPlayerData[i].unk_70[2], mtPlayerData[i].unk_70[3]);
            }
        }
    }
}

s32 fn_1_7124(void)
{
    s32 var_r31 = 0;

    if (_CheckFlag(FLAG_ID_MAKE(0, 2)) != 0) {
        var_r31++;
    }
    if (_CheckFlag(FLAG_ID_MAKE(0, 3)) != 0) {
        var_r31++;
    }
    if (_CheckFlag(FLAG_ID_MAKE(0, 4)) != 0) {
        var_r31++;
    }
    if (_CheckFlag(FLAG_ID_MAKE(0, 5)) != 0) {
        var_r31++;
    }
    if (_CheckFlag(FLAG_ID_MAKE(0, 6)) != 0) {
        var_r31++;
    }
    if (_CheckFlag(FLAG_ID_MAKE(0, 7)) != 0) {
        var_r31++;
    }
    OSReport("DIF OMAKASE MAP %d\n", var_r31);
    var_r31 = var_r31 / 2;
    if (var_r31 < 0) {
        var_r31 = 0;
    }
    else if (var_r31 > 3) {
        var_r31 = 3;
    }
    OSReport("DIF OMAKASE DIF %d\n", var_r31);
    return var_r31;
}

s32 dir_tables[0xF] = {
    DATADIR_W01,
    DATADIR_W02,
    DATADIR_W03,
    DATADIR_W04,
    DATADIR_W05,
    DATADIR_W06,
    DATADIR_W10,
    DATADIR_MARIOMDL1,
    DATADIR_LUIGIMDL1,
    DATADIR_PEACHMDL1,
    DATADIR_YOSHIMDL1,
    DATADIR_WARIOMDL1,
    DATADIR_DONKEYMDL1,
    DATADIR_DAISYMDL1,
    DATADIR_WALUIGIMDL1,
};

void LoadBoard(void)
{
    s32 var_r31 = HuDataDirReadAsync(DATADIR_BOARD);

    if (var_r31 != -1) {
        while (HuDataGetAsyncStat(var_r31) == 0) {
            HuPrcVSleep();
        }
    }
    HuAR_MRAMtoARAM(DATADIR_BOARD);
    while (HuARDMACheck() != 0) {
        HuPrcVSleep();
    }
    HuDataDirClose(DATADIR_BOARD);
    if (_CheckFlag(FLAG_ID_MAKE(1, 11)) != 0) {
        var_r31 = HuDataDirReadAsync(DATADIR_W10);
    }
    else {
        var_r31 = HuDataDirReadAsync(dir_tables[GWSystem.board]);
    }
    if (var_r31 != -1) {
        while (HuDataGetAsyncStat(var_r31) == 0) {
            HuPrcVSleep();
        }
    }
    is_board_loaded = 1;
    HuPrcEnd();
    while (1) {
        HuPrcVSleep();
    }
}

void fn_1_7304(void)
{
    s32 var_r31;
    s32 var_r30;
    s32 var_r29;

    for (var_r31 = 1; var_r31 < 4; var_r31++) {
        mtPlayerData[var_r31].character = -1;
        mtPlayerData[var_r31].pad_idx = var_r31;
        mtPlayerData[var_r31].iscom = 1;
    }
    for (var_r31 = 1; var_r31 < 4; var_r31++) {
        do {
            mtPlayerData[var_r31].character = rand8() % 8;
            for (var_r29 = 0; var_r29 < 4; var_r29++) {
                if ((var_r31 != var_r29) && (mtPlayerData[var_r29].character != -1)
                    && (mtPlayerData[var_r31].character == mtPlayerData[var_r29].character)) {
                    break;
                }
            }
        } while (var_r29 != 4);
    }
    GWSystem.diff_story = board_configs[0].board_settings[0];
    for (var_r31 = 0; var_r31 < 4; var_r31++) {
        mtPlayerData[var_r31].diff = GWSystem.diff_story;
        if (GWSystem.diff_story == 4) {
            mtPlayerData[var_r31].diff = fn_1_7124();
        }
    }
    for (var_r31 = 0; var_r31 < 4; var_r31++) {
        GWPlayerCfg[var_r31].character = mtPlayerData[var_r31].character;
        GWPlayerCfg[var_r31].pad_idx = mtPlayerData[var_r31].pad_idx;
        GWPlayerCfg[var_r31].diff = mtPlayerData[var_r31].diff;
        GWPlayerCfg[var_r31].group = 0;
        GWPlayerCfg[var_r31].iscom = mtPlayerData[var_r31].iscom;
    }
    BoardSaveInit(gameConfigs[2]);
    CharDataClose(-1);
}

void fn_1_7684(void)
{
    s32 spC[7] = { 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F };
    WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_NORMAL, -1);
    while (WipeStatGet() != 0) {
        MenuPrcVSleep();
    }
    CharModelKill(-1);
    MGSeqKillAll();
    {
        OMOVLHIS *sp8 = omOvlHisGet(0);
    }
    omOvlHisChg(0, DLL_mstory3dll, 0, 0);
    do {
        MenuPrcVSleep();
    } while (is_board_loaded != 1);
    HuAudSeqAllFadeOut(0x3E8);
    HuAudSStreamAllFadeOut(0x3E8);
    CharMotionInit(GWPlayerCfg[0].character);
    CharMotionInit(GWPlayerCfg[1].character);
    CharMotionInit(GWPlayerCfg[2].character);
    CharMotionInit(GWPlayerCfg[3].character);
    omOvlCallEx(spC[GWSystem.board], 1, 0, 0);
    while (1) {
        MenuPrcVSleep();
    }
}

void MenuRunStoryIntro(void)
{
    s32 i;

    moveCameraWithMethod(MenuCameraMoveThroughIntro);
    MenuPrcSleep(0xF0);
    for (i = 0; i < 5; i++) {
        Hu3DModelAttrReset(silly_guys[i].obj->mdlId[1], HU3D_ATTR_DISPOFF);
    }
    MenuObject[0]->work[0] = 1;
    moveCameraWithMethod(MenuIntroBackOffAfterSillyGuys);
    MenuPrcSleep(0x8C);
}

void MenuStartScene3(void)
{
    s32 i;

    MenuPrcSleep(0x3C);
    audio_channel_idx[0] = HuAudSeqPlay(0x30);
    for (i = 0; i < 5; i++) {
        Hu3DModelAttrReset(silly_guys[i].obj->mdlId[1], HU3D_ATTR_DISPOFF);
    }
    WipeCreate(WIPE_MODE_IN, WIPE_TYPE_NORMAL, -1);

    while (WipeStatGet() != 0) {
        HuPrcVSleep();
    }
    MenuPrcSleep(0x3C);
}

void fn_1_7900(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    sp8.center.x = lbl_1_bss_DC;
    sp8.center.y = 60.0f;
    sp8.center.z = 0.0f;
    sp8.rot.x = 0.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 850.0f;
    MenuCameraSinEaseFollow(var_r31, &sp8, var_r31->frames++, 10.0f, 5.0f);
}

s32 lbl_1_data_2F0 = -1;

void fn_1_7A14(void)
{
    s32 var_r31;
    s32 var_r30;
    s32 var_r29;

    _ClearFlag(2);
    _ClearFlag(3);
    _ClearFlag(4);
    _ClearFlag(5);
    _ClearFlag(6);
    _ClearFlag(7);
    _ClearFlag(1);
    _ClearFlag(9);
    for (var_r31 = 0; var_r31 < 5; var_r31++) {
        motionShiftIfChanged(silly_guys[var_r31].obj, 1, 6, 5, 1);
    }
    motionShift(board_configs[0].obj, 2, 3, 0, 0);
    birthdayConfettiObj->work[0] = 1;
    var_r29 = -1;
    var_r29 = HuAudFXPlay(0x43);
    HuAudFXPanning(var_r29, 0x20);
    var_r29 = HuAudFXPlay(0x40);
    HuAudFXPanning(var_r29, 0x30);
    var_r29 = HuAudFXPlay(0x37);
    HuAudFXPanning(var_r29, 0x40);
    var_r29 = HuAudFXPlay(0x4B);
    HuAudFXPanning(var_r29, 0x4C);
    var_r29 = HuAudFXPlay(0x46);
    HuAudFXPanning(var_r29, 0x60);
    HuAudFXPlay(0x9A);
    HuAudFXPlay(0x306);
    MenuPrcSleep(0x96);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    MenuPrcSleep(0x1E);
    for (var_r31 = 0; var_r31 < 5; var_r31++) {
        silly_guys[var_r31].unk_08 = 0;
        silly_guys[var_r31].objCallback = (MentDllUnkFunc)chooseBoardSillyGuyPos;
    }
    silly_guys->unk_08 = 1;
    MenuPrcSleep(0x3C);
    for (var_r31 = 1; var_r31 < 5; var_r31++) {
        motionShiftIfChanged(silly_guys[var_r31].obj, 1, 1, 0x1E, 1);
    }
    var_r30 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
    HuWinInsertMesSet(var_r30, mtPlayerData->character, 0);
    WinSetMessAndWait(var_r30, 0x1E005F, -1, -1);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    DestroyWin(var_r30);
    MenuPrcSleep(0x1E);
    for (var_r31 = 1; var_r31 < 5; var_r31++) {
        silly_guys[var_r31].unk_08 = 0;
    }
    silly_guys[3].unk_08 = 1;
    menuCamera.frames = 0;
    lbl_1_bss_DC = silly_guys_pos[3].x;
    moveCameraWithMethod(fn_1_7900);
    MenuPrcSleep(0x1E);
    var_r30 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[3].obj, 1, 6, 0xF, 1);
    WinSetMessAndWait(var_r30, 0x1E0060, -1, -1);
    motionShiftIfChanged(silly_guys[3].obj, 1, 1, 0xF, 1);
    DestroyWin(var_r30);
    for (var_r31 = 0; var_r31 < 5; var_r31++) {
        silly_guys[var_r31].unk_08 = 0;
    }
    silly_guys[1].unk_08 = 1;
    menuCamera.frames = 0;
    lbl_1_bss_DC = silly_guys_pos[1].x;
    MenuPrcSleep(0x1E);
    var_r30 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[1].obj, 1, 6, 0xF, 1);
    WinSetMessAndWait(var_r30, 0x1E0061, -1, -1);
    motionShiftIfChanged(silly_guys[1].obj, 1, 1, 0xF, 1);
    DestroyWin(var_r30);
    for (var_r31 = 0; var_r31 < 5; var_r31++) {
        silly_guys[var_r31].unk_08 = 0;
    }
    silly_guys[4].unk_08 = 1;
    menuCamera.frames = 0;
    lbl_1_bss_DC = silly_guys_pos[4].x;
    MenuPrcSleep(0x1E);
    var_r30 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[4].obj, 1, 6, 0xF, 1);
    WinSetMessAndWait(var_r30, 0x1E0062, -1, -1);
    motionShiftIfChanged(silly_guys[4].obj, 1, 1, 0xF, 1);
    DestroyWin(var_r30);
    for (var_r31 = 0; var_r31 < 5; var_r31++) {
        silly_guys[var_r31].unk_08 = 0;
    }
    silly_guys[2].unk_08 = 1;
    menuCamera.frames = 0;
    lbl_1_bss_DC = silly_guys_pos[2].x;
    MenuPrcSleep(0x1E);
    var_r30 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[2].obj, 1, 1, 0xF, 1);
    WinSetMessAndWait(var_r30, 0x1E0063, -1, -1);
    motionShiftIfChanged(silly_guys[2].obj, 1, 1, 0xF, 1);
    DestroyWin(var_r30);
    birthdayConfettiObj->work[0] = 3;
    for (var_r31 = 0; var_r31 < 5; var_r31++) {
        silly_guys[var_r31].unk_08 = 0;
    }
    silly_guys->unk_08 = 1;
    moveCameraWithMethod(MenuIntroBackOffAfterSillyGuys);
    MenuPrcSleep(0x3C);
    var_r30 = OpenWinBottom(0, 0, 0);
    lbl_1_data_2F0 = var_r30;
    motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(var_r30, 0x1E0064, -1, -1);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    for (var_r31 = 0; var_r31 < 5; var_r31++) {
        silly_guys[var_r31].objCallback = NULL;
    }
}

void fn_1_81A8(OMOBJ *arg0, void *arg1, void *arg2)
{
    fn_1_1F868();
    mtPlayerData->proc_selection_handler = NULL;
}

void fn_1_81D8(void)
{
    mtPlayerData[0].proc_selection_handler = (MentDllUnkFunc)fn_1_81A8;
}

void MenuStartScene(void)
{
    MenuPrcSleep(0x3C);
    HuAudSStreamPlay(0xC);
    OSReport("########### ME_MainProcFunc000\n");
    WipeColorSet(0xFF, 0xFF, 0xFF);
    WipeCreate(WIPE_MODE_IN, WIPE_TYPE_NORMAL, -1);
}

void MenuStartScene2(void)
{
    s32 i;

    MenuPrcSleep(0x3C);
    audio_channel_idx[0] = HuAudSeqPlay(0x30);
    if (gameConfigs[0] == 0) {
        for (i = 0; i < 6; i++) {
            Hu3DModelAttrReset(silly_guys[i].obj->mdlId[1], HU3D_ATTR_DISPOFF);
        }
    }
    else {
        for (i = 0; i < 5; i++) {
            Hu3DModelAttrReset(silly_guys[i].obj->mdlId[1], HU3D_ATTR_DISPOFF);
        }
    }
    Hu3DModelPosSet(silly_guys[0].obj->mdlId[1], 0.0f, 0.0f, 560.0f);
    motionShift(board_configs[0].obj, 2, 2, 0, 2);
    Hu3DModelAttrSet(board_configs[0].obj->mdlId[2], HU3D_ATTR_DISPOFF);
    OSReport("########### ME_MainProcFunc400\n");
    WipeCreate(WIPE_MODE_IN, WIPE_TYPE_NORMAL, -1);

    while (WipeStatGet() != 0) {
        HuPrcVSleep();
    }
    MenuPrcSleep(0x3C);
}

void MenuRunPartyIntro(void)
{
    s32 i;

    Vec toad_new_pos = { 0.0f, 0.0f, 560.0f };
    moveCameraWithMethod(MenuCameraMoveThroughIntro);
    MenuPrcSleep(0x5A);
    if (gameConfigs[0] == 0) {
        for (i = 0; i < 6; i++) {
            Hu3DModelAttrReset(silly_guys[i].obj->mdlId[1], HU3D_ATTR_DISPOFF);
        }
    }
    else {
        for (i = 0; i < 5; i++) {
            Hu3DModelAttrReset(silly_guys[i].obj->mdlId[1], HU3D_ATTR_DISPOFF);
        }
    }
    motionShift(board_configs[0].obj, 2, 3, 0, 0);
    MenuPrcSleep(0x96);
    MenuObject[0]->work[0] = 1;
    moveCameraWithMethod(MenuIntroBackOffAfterSillyGuys);
    MenuPrcSleep(0x5A);
    motionShiftIfChanged(silly_guys[0].obj, 1, 2, 0xF, 1);
    MenuMoveChar(silly_guys[0].obj, 1, toad_new_pos, 0.0f, 3.0f, 0.0f, 1, 0);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    Hu3DModelAttrSet(board_configs[0].obj->mdlId[2], HU3D_ATTR_DISPOFF);
    MenuPrcSleep(0x1E);
}

void MenuPartyAfterIntroPrelinimaries(void)
{
    s32 winId;
    s32 choice;

    audio_channel_idx[0] = HuAudSeqPlay(0x30);
    winId = OpenWinBottom(0, 0, 0);
    HuAudFXPlay(menuSoundFXTbl[0][0]);
    motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(winId, 0x1A0000, -1, -1);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    DestroyWin(winId);
    saveExecF = 0;
    if (GWGameStat.party_continue == 1) {
        winId = OpenWinBottom(0, 0, 0);
        while (1) {
            MenuPrcVSleep();
            motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
            WinSetMessAndWait(winId, 0x1A0001, -1, 5);
            motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
            choice = OpenConfirmDlgYesDef(0x1E0035, 3, 0);
            if (choice == -1) {
                motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
                WinSetMessAndWait(winId, 0x1A0003, -1, 5);
                motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
                choice = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
                if (choice == 0) {
                    DestroyWin(winId);
                    goToNextOverlay(0);
                }
                continue;
            }
            if (choice == 0) {
                HuAudFXPlay(menuSoundFXTbl[0][3]);
                saveExecF = 1;
                SLLoadBoard();
                HuDataDirClose(DATADIR_MENT);
                HuPrcChildCreate(LoadBoard, 0x64, 0x3000, 0, menuProc);
                DestroyWin(winId);
                goToNextOverlay(1);
                continue;
            }
            if (choice == 1) {
                break;
            }
        }
        motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(winId, 0x1A0002, -1, -1);
        motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
        DestroyWin(winId);
    }
}

void boardPartyTutorialPrompt(void)
{
    s32 i;
    s32 window_tutorial_ask;
    s32 window_confirm_dialog;

    window_tutorial_ask = OpenWinBottom(0, 0, 0);
    HuAudFXPlay(menuSoundFXTbl[0][3]);
    motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(window_tutorial_ask, 0x1A0004, -1, 5);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    window_confirm_dialog = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
    DestroyWin(window_tutorial_ask);
    if (window_confirm_dialog == 0) {
        s32 tutorial_characters[4] = { 3, 0, 2, 4 };
        _SetFlag(0x1000B);
        for (i = 0; i < 4; i++) {
            GWPlayerCfg[i].character = tutorial_characters[i];
            GWPlayerCfg[i].pad_idx = i;
            GWPlayerCfg[i].diff = 0;
            GWPlayerCfg[i].group = 0;
            GWPlayerCfg[i].iscom = 1;
            OSReport("ID-%d CHR-%d PAD-%d DIF-%d GRP-%d COM-%d\n", i, GWPlayerCfg[i].character, GWPlayerCfg[i].pad_idx,
                GWPlayerCfg[i].diff, GWPlayerCfg[i].group, GWPlayerCfg[i].iscom);
        }
        BoardSaveInit(6);
        GWSystem.max_turn = 0x14;
        HuDataDirClose(DATADIR_MENT);
        HuPrcChildCreate(LoadBoard, 0x64, 0x3000, 0, menuProc);
        {
            OMOVLHIS *sp8 = omOvlHisGet(0);
        }
        omOvlHisChg(0, DLL_mentdll, 0, 1);
        goToNextOverlay(2);
    }
    else {
        _ClearFlag(0x1000B);
    }
    hideChooseCharacters();
}

void fn_1_8B40(s32 arg0)
{
    moveHumancountGroup(arg0);
}

s32 human_count_menu(void)
{
    s32 i;
    s32 var_r30;
    s32 var_r29;
    s32 toads_se_id;
    s32 var_r26;
    s32 var_r27;
    s32 var_r25;
    s32 var_r24;
    s32 var_r23;

    var_r30 = 0;
    var_r23 = 0;
    toads_se_id = 0; // 0 is "yay" and 3 is "hahaha"
    if (gameConfigs[0] == 5) {
        toads_se_id = 3;
    }
    var_r27 = OpenWinBottom(0, 0, 0);
    HuAudFXPlay(menuSoundFXTbl[toads_se_id][2]);
    while (1) {
        MenuPrcVSleep();
        MenuPrcVSleep();
        motionShift(silly_guys[toads_se_id].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(var_r27, 0x1A0005, -1, -1);
        motionShift(silly_guys[toads_se_id].obj, 1, 1, 0xF, 1);
        OpenAvailControlsWin(0x1A0021);
        fn_1_208F4();
        board_configs[0].obj_callback = (MentDllUnkFunc)fn_1_1FF4C;
        var_r25 = 0x63;
        var_r23 = 0;
        while (1) {
            MenuPrcVSleep();
            var_r30 = 0;
            for (i = 0; i < 4; i++) {
                if (mtPlayerData[i].unk_70[3] == 0) {
                    var_r30++;
                }
            }
            if (var_r25 != var_r30) {
                var_r25 = var_r30;
                WinSetMessAndWait(var_r27, var_r30 + 0x1A001B, -1, -0x3E7);
            }
            if (board_configs[0].board_settings[0] != 0) {
                continue;
            }
            if (HuPadBtnDown[mtPlayerData->pad_idx] & PAD_BUTTON_A) {
                var_r29 = 1;
                HuAudFXPlay(2);
                board_configs[0].obj_callback = NULL;
                MenuPrcSleep(2);
                var_r26 = 0;
                var_r30 = var_r26;
                for (i = 0; i < 4; i++) {
                    if (mtPlayerData[i].iscom == 0) {
                        var_r26++;
                    }
                }
                for (i = 0; i < 4; i++) {
                    if (var_r30 < var_r26) {
                        if (HuPadStatGet(i) != -1) {
                            var_r30++;
                            mtPlayerData[i].iscom = 0;
                        }
                        else {
                            mtPlayerData[i].iscom = 1;
                        }
                    }
                }
                break;
            }
            else if (HuPadBtnDown[mtPlayerData->pad_idx] & PAD_BUTTON_B) {
                var_r29 = 0;
                HuAudFXPlay(3);
                break;
            }
        }
        fn_1_20A24();
        board_configs[0].obj_callback = NULL;
        destroyAvailControlsWin(0);
        if (var_r29 == 0) {
            motionShiftIfChanged(silly_guys[toads_se_id].obj, 1, 4, 0xF, 1);
            WinSetMessAndWait(var_r27, 0x1A0003, -1, 5);
            motionShiftIfChanged(silly_guys[toads_se_id].obj, 1, 1, 0xF, 1);
            var_r24 = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
            if (var_r24 == 0) {
                DestroyWin(var_r27);
                goToNextOverlay(0);
            }
        }
        else if (var_r29 == 1) {
            break;
        }
    }
    DestroyWin(var_r27);
    var_r30 = 0;
    for (i = 0; i < 4; i++) {
        if (mtPlayerData[i].iscom == 0) {
            var_r30++;
        }
    }
    if (var_r30 == 0) {
        var_r29 = 2;
    }
    else {
        var_r29 = 1;
    }
    return var_r29;
}

void character_selection_init(void)
{
    s32 i;

    human_count_menu_exit();
    character_selection_grid_init();
    for (i = 0; i < 4; i++) {
        mtPlayerData[i].character = i;
    }
}

void fn_1_8F98(void)
{
    pushDownCharacterSelection();
}

s32 playerSelection(void)
{
    s32 i;
    s32 selection_part; // 0 means still players deciding, 2 means player selection finished and waiting on computers and 3 means no human players involved
    s32 var_r29;
    s32 human_player_count;
    s32 var_r27;
    s32 player_selected_character; // Seems like doing nothing

    player_selected_character = 0;
    var_r29 = 0;
    if (gameConfigs[0] == 5) {
        var_r29 = 3;
    }
    character_selection_grid_init();
    var_r27 = OpenWinBottom(0, 0, 0);
    HuWinPushKeySet(var_r27, 0x300);
    HuAudFXPlay(menuSoundFXTbl[var_r29][2]);
    MenuPrcVSleep();
    motionShift(silly_guys[var_r29].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(var_r27, 0x1A0009, -1, -1);
    motionShift(silly_guys[var_r29].obj, 1, 1, 0xF, 1);
    OpenAvailControlsWin(0x1A0020);
    for (i = 0; i < 4; i++) {
        if (mtPlayerData[i].iscom == 0) {
            mtPlayerData[i].proc_selection_handler = (MentDllUnkFunc)human_character_select;
        }
    }
    // Infinite loop during character selection
    while (1) {
        MenuPrcVSleep();
        for (i = 0; i < 4; i++) {
            if ((mtPlayerData[i].iscom == 0) && (HuPadBtnDown[mtPlayerData[i].pad_idx] & PAD_BUTTON_A)) {
                player_selected_character = 1;
            }
        }
        if ((mtPlayerData[0].unk_70[0] == 0) && (HuPadBtnDown[mtPlayerData->pad_idx] & PAD_BUTTON_B)) {
            HuAudFXPlay(3);
            selection_part = 0;
            break;
        }
        else {
            for (i = 0; i < 4; i++) {
                if ((mtPlayerData[i].iscom == 0) && (mtPlayerData[i].unk_70[0] != 1)) {
                    break;
                }
            }
            if (i == 4) {
                selection_part = 2;
                break;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        mtPlayerData[i].proc_selection_handler = NULL;
    }
    destroyAvailControlsWin(0);
    DestroyWin(var_r27);
    if (selection_part == 2) {
        human_player_count = 0;
        for (i = 0; i < 4; i++) {
            if (mtPlayerData[i].iscom != 0) {
                mtPlayerData[i].character = 0;
                human_player_count++;
            }
        }
        if (human_player_count == 0) {
            selection_part = 3;
        }
        else {
            selection_part = 2;
        }
    }
    else {
        selection_part = 0;
    }
    return selection_part;
}

s32 computer_players_selection(void)
{
    s32 sp8;
    s32 i;
    s32 current_computer_player;
    s32 var_r29;
    s32 computer_or_human_count;
    s32 var_r27;
    s32 var_r26;
    s32 var_r25;
    s32 var_r24;

    current_computer_player = -1;
    var_r26 = -1;
    var_r24 = -1;
    sp8 = 0;
    var_r25 = 0;
    if (gameConfigs[0] == 5) {
        var_r25 = 3;
    }
    var_r27 = OpenWinBottom(0, 0, 0);
    HuAudFXPlay(menuSoundFXTbl[var_r25][3]);
    computer_or_human_count = 0;
    for (i = 0; i < 4; i++) {
        if (mtPlayerData[i].iscom != 0) {
            computer_or_human_count++;
        }
    }
    HuWinInsertMesSet(var_r27, computer_or_human_count + 0x1A0023, 1);
    motionShiftIfChanged(silly_guys[var_r25].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(var_r27, 0x1A0007, -1, -1);
    motionShiftIfChanged(silly_guys[var_r25].obj, 1, 1, 0xF, 1);
    for (i = 0; i < 4; i++) {
        if (mtPlayerData[i].iscom != 0) {
            mtPlayerData[i].character = mtPlayerData[i].diff = 0;
        }
    }
    while (1) {
        MenuPrcVSleep();
        OpenAvailControlsWin(0x1A0020);
        for (i = 0; i < 4; i++) {
            if ((mtPlayerData[i].iscom != 0) && (mtPlayerData[i].unk_70[0] == 0)) {
                current_computer_player = i;
                mtPlayerData[i].unk_70[1] = var_r26;
                computer_character_selection_draw(&mtPlayerData[i]);
                mtPlayerData[i].proc_selection_handler = (MentDllUnkFunc)computer_character_select;
                break;
            }
        }
        while (1) {
            MenuPrcVSleep();
            if ((var_r24 != mtPlayerData[current_computer_player].character) && (mtPlayerData[current_computer_player].unk_70[0] == 0)) {
                var_r24 = mtPlayerData[current_computer_player].character;
                WinSetMessAndWait(var_r27, mtPlayerData[current_computer_player].character + 0x1B0008, -1, -0x3E7);
            }
            if ((HuPadBtnDown[mtPlayerData->pad_idx] & PAD_BUTTON_B) && (mtPlayerData[current_computer_player].unk_70[0] == 0)) {
                HuAudFXPlay(3);
                if (var_r26 == -1) {
                    var_r29 = 0;
                }
                else {
                    var_r29 = 1;
                }
                break;
            }
            else if (mtPlayerData[current_computer_player].unk_70[0] == 1) {
                for (i = 0; i < 4; i++) {
                    if (mtPlayerData[i].unk_70[0] == 0) {
                        mtPlayerData[i].character = 0;
                    }
                }
                mtPlayerData[current_computer_player].proc_selection_handler = NULL;
                WinSetMessAndWait(var_r27, 0x1A0022, -1, -0x3E7);
                OpenAvailControlsWin(0x1A0023);
                var_r29 = fn_1_1648C(&mtPlayerData[current_computer_player]);
                if (var_r29 == 1) {
                    mtPlayerData[current_computer_player].unk_70[2] = 1;
                    var_r29 = 4;
                    var_r26 = current_computer_player;
                }
                else {
                    mtPlayerData[current_computer_player].unk_70[2] = 0;
                    var_r29 = 4;
                    mtPlayerData[current_computer_player].unk_70[0] = 0;
                    removeComputerHighlight(&mtPlayerData[current_computer_player]);
                    computer_character_selection_draw(&mtPlayerData[current_computer_player]);
                }
                break;
            }
        }
        for (i = 0; i < 4; i++) {
            mtPlayerData[i].proc_selection_handler = NULL;
        }
        for (i = 0; i < 4; i++) {
            if (mtPlayerData[i].unk_70[0] == 0) {
                break;
            }
        }
        if (i == 4) {
            var_r29 = 3;
            break;
        }
        else {
            if (var_r29 == 1) {
                computer_character_selection_draw(&mtPlayerData[current_computer_player]);
                removeComputerHighlight(&mtPlayerData[current_computer_player]);
                computer_character_selection_draw(&mtPlayerData[mtPlayerData[current_computer_player].unk_70[1]]);
                var_r26 = mtPlayerData[mtPlayerData[current_computer_player].unk_70[1]].unk_70[1];
                mtPlayerData[mtPlayerData[current_computer_player].unk_70[1]].unk_70[0] = 0;
            }
            else if (var_r29 == 4) {
            }
            else if (var_r29 == 0) {
                removeComputerHighlight(&mtPlayerData[current_computer_player]);
                break;
            }
            else {
                break;
            }
        }
        var_r24 = -1;
    }
    destroyAvailControlsWin(0);
    DestroyWin(var_r27);
    if (var_r29 == 0) {
        computer_or_human_count = 0;
        for (i = 0; i < 4; i++) {
            if (mtPlayerData[i].iscom == 0) {
                computer_or_human_count++;
            }
        }
        if (computer_or_human_count != 0) {
            var_r29 = 1;
        }
        else {
            var_r29 = 0;
        }
    }
    return var_r29;
}

void pushUpCharacterSelection(void)
{
    s32 i;
    s32 j;

    moveUpCharacterSelction();
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (mtPlayerData[i].iscom == 1) {
                mtPlayerData[i].unk_70[j] = 0;
                mtPlayerData[i].unk_80[j] = 0;
                removeComputerHighlight(&mtPlayerData[i]);
            }
        }
    }
}

void fn_1_99AC(void)
{
    pushDownChosenCharactersOverlay();
}

s32 chooseBoardMenu(void)
{
    s32 go_outside_confirm_window;
    s32 board_info_window;
    s32 selection_hold_timer;
    s32 action; // 0 means back (B button), 2 means select board (A button)
    s32 current_silly_guy;
    s32 i;

    s32 silly_guys_line_idx[6] = {
        0x00000001,
        0x00000002,
        0x00000000,
        0x00000003,
        0x00000004,
        0x00000005,
    };

    current_silly_guy = 2;
    board_info_window = OpenWinBottom(0, 0, 0);
    while (1) {
        MenuPrcVSleep();
        selection_hold_timer = 0;
        silly_guys->unk_34 = 1;
        HuAudFXPlay(menuSoundFXTbl[0][3]);
        motionShiftIfChanged(silly_guys[silly_guys_line_idx[current_silly_guy]].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(board_info_window, 0x1A0008U, -1, -1);
        motionShiftIfChanged(silly_guys[silly_guys_line_idx[current_silly_guy]].obj, 1, 1, 0xF, 1);
        OpenAvailControlsWin(0x1A0020U);

        for (i = 0; i < 6; i++) {
            silly_guys[i].unk_08 = 0;
            silly_guys[i].objCallback = (void (*)(OMOBJ *, ...))chooseBoardSillyGuyPos;
        }

        silly_guys[silly_guys_line_idx[current_silly_guy]].unk_08 = 1;
        board_configs->obj_callback = (void (*)(OMOBJ *, ...))fn_1_18F74;
        while (1) {
            MenuPrcVSleep();
            for (i = 0; i < 6; i++) {
                if (silly_guys[i].unk_08 != 2) {
                    break;
                }
            }

            if (i == 6 && selection_hold_timer >= 0x15) {
                if ((HuPadBtnDown[mtPlayerData->pad_idx] & 0x200)) {
                    HuAudFXPlay(3);
                    action = 0;
                    break;
                }
                else if ((HuPadBtnDown[mtPlayerData->pad_idx] & 0x100)) {
                    HuAudFXPlay(2);
                    action = 2;
                    break;
                }
                else {
                    if ((HuPadStkX[mtPlayerData->pad_idx] >= 50) || ((HuPadBtn[mtPlayerData->pad_idx] & 2))) {
                        if (gameConfigs[4] == 1) {
                            if (++current_silly_guy > 5) {
                                current_silly_guy = 5;
                            }
                            else {
                                selection_hold_timer = 0;
                            }
                        }
                        else {
                            if (++current_silly_guy > 4) {
                                current_silly_guy = 4;
                            }
                            else {
                                selection_hold_timer = 0;
                            }
                        }
                    }
                    else if ((HuPadStkX[mtPlayerData->pad_idx] <= -50) || ((HuPadBtn[mtPlayerData->pad_idx] & 1))) {
                        if (--current_silly_guy < 0) {
                            current_silly_guy = 0;
                        }
                        else {
                            selection_hold_timer = 0;
                        }
                    }
                }
            }
            if (selection_hold_timer == 0) {
                if (current_silly_guy != 5) {
                    moveCameraWithMethod(fn_1_14058);
                }
                else {
                    moveCameraWithMethod(fn_1_14148);
                }
                for (i = 0; i < 6; i++) {
                    silly_guys[i].unk_08 = 0;
                }
                silly_guys[silly_guys_line_idx[current_silly_guy]].unk_08 = 1;
            }
            if (selection_hold_timer++ == 0x14) {
                selection_hold_timer = 0x15;
                fn_1_18A54(silly_guys_line_idx[current_silly_guy]);
                WinSetMessAndWait(board_info_window, silly_guys_line_idx[current_silly_guy] + 0x1B0001, -1, -999);
                if ((s32)silly_guys_line_idx[current_silly_guy] != 0) {
                    HuAudFXPlay(menuSoundFXTbl[silly_guys_line_idx[current_silly_guy]][0]);
                }
                else {
                    HuAudFXPlay(menuSoundFXTbl[silly_guys_line_idx[current_silly_guy]][2]);
                }
            }
        }
        destroyAvailControlsWin(0);
        if (action == 0) {
            moveCameraWithMethod(fn_1_14058);
            if (current_silly_guy != 2) {
                current_silly_guy = 2;
                for (i = 0; i < 6; i++) {
                    silly_guys[i].unk_08 = 0;
                }

                silly_guys[silly_guys_line_idx[current_silly_guy]].unk_08 = 1;
            }
            RescaleChooseBoardGrp();
            MenuPrcSleep(0x1E);
        }
        else if (action == 1) {
            moveCameraWithMethod(fn_1_14058);
            if (current_silly_guy != 2) {
                current_silly_guy = 2;
                for (i = 0; i < 6; i++) {
                    silly_guys[i].unk_08 = 0;
                }
                silly_guys[silly_guys_line_idx[current_silly_guy]].unk_08 = 1;
            }
            RescaleChooseBoardGrp();
            MenuPrcSleep(0x1E);
            motionShiftIfChanged(silly_guys->obj, 1, 4, 0xF, 1);
            WinSetMessAndWait(board_info_window, 0x1A0003U, -1, 5);
            motionShiftIfChanged(silly_guys->obj, 1, 1, 0xF, 1);
            go_outside_confirm_window = OpenConfirmDlgNoDef(0x1E0035U, 3, 0);
            if (go_outside_confirm_window == 0) {
                DestroyWin(board_info_window);
                goToNextOverlay(0);
            }
            continue;
        }
        else if (action == 2) {
            for (i = 0; i < 6; i++) {
                silly_guys[i].objCallback = NULL;
            }
            gameConfigs[2] = silly_guys_line_idx[current_silly_guy];
        }
        else {
            continue;
        }
        break;
    }

    DestroyWin(board_info_window);

    if (action == 0) {
        for (i = 0; i < 4; i++) {
            if (mtPlayerData[i].iscom != 0) {
                break;
            }
        }
        if (i != 4) {
            action = 2;
        }
        else {
            action = 1;
        }
    }
    else {
        action = 3;
    }
    return action; // action 3 means select board, 1 and 2 means back off
}

void fn_1_A0A4(void)
{
    pushUpChosenCharactersOverlay();
}

void preMapRulesSelectionPrep(void)
{
    s32 map;

    map = gameConfigs[2];
    pushUpChosenCharactersOverlay();
    HuAudFXPlay(menuSoundFXTbl[map][3]);
    if (map != 5) {
        Vec silly_guy_new_pos = { -350.0f, 0.0f, 800.0f };
        moveCameraWithMethod(fn_1_14238);
        motionShift(silly_guys[map].obj, 1, 3, 0xF, 1);
        MenuMoveChar(silly_guys[map].obj, 1, silly_guy_new_pos, 30.0f, 6.0f, 10.0f, 1, 1);
        motionShift(silly_guys[map].obj, 1, 1, 0xF, 1);
        Hu3DModelAttrReset(board_configs[0].obj->mdlId[2], HU3D_ATTR_DISPOFF);
        motionShift(board_configs[0].obj, 2, 2, 0, 0);
        WaitAnimEnd(board_configs[0].obj, 2, 0);
    }
    else {
        Vec silly_guy_new_pos = { 850.0f, 0.0f, 800.0f };
        moveCameraWithMethod(fn_1_14328);
        motionShift(silly_guys[map].obj, 1, 3, 0xF, 1);
        MenuMoveChar(silly_guys[map].obj, 1, silly_guy_new_pos, 30.0f, 6.0f, 10.0f, 1, 1);
        motionShift(silly_guys[map].obj, 1, 1, 0xF, 1);
        Hu3DModelAttrReset(board_configs[1].obj->mdlId[2], HU3D_ATTR_DISPOFF);
        motionShift(board_configs[1].obj, 2, 2, 0, 0);
        WaitAnimEnd(board_configs[1].obj, 2, 0);
    }
}

void init_menu_something_useless(void)
{
    pushDownBoardSettings();
}

s32 getBoardSettings(void)
{
    char max_turns_buffer[10];
    s32 setting_info_window;
    s32 map;
    s32 next_section;
    s32 confirm_window;
    s32 setting_section_idx;
    s32 setting_section_value;

    setting_section_idx = -1;
    setting_section_value = -1;
    map = gameConfigs[2];
    setting_info_window = OpenWinBottom(0, 0, 0);
    if (map == 0) {
        HuAudFXPlay(menuSoundFXTbl[map][2]);
    }
    else {
        HuAudFXPlay(menuSoundFXTbl[map][0]);
    }
loop_3:
    MenuPrcVSleep();
    setting_section_idx = setting_section_value = -1;
    motionShiftIfChanged(silly_guys[map].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(setting_info_window, map + 0x1A000A, -1, -1);
    motionShiftIfChanged(silly_guys[map].obj, 1, 1, 0xF, 1);
    OpenAvailControlsWin(0x1A0021);
    board_configs[0].handicaps[1] = 0;
    setupSettingSelectionCursor();
    board_configs[1].obj_callback = (MentDllUnkFunc)configureBoardWithController;
loop_4:
    MenuPrcVSleep();
    if ((setting_section_idx != board_configs[1].handicaps[0]) || setting_section_value != board_configs[1].board_settings[board_configs[1].handicaps[0]]) {
        setting_section_idx = board_configs[1].handicaps[0];
        setting_section_value = board_configs[1].board_settings[board_configs[1].handicaps[0]];
        switch (board_configs[1].handicaps[0]) {
            case 0:
                switch (board_configs[1].board_settings[0]) {
                    case 0:
                        WinSetMessAndWait(setting_info_window, 0x1B0015, -1, -0x3E7);
                        break;
                    case 1:
                        HuWinInsertMesSet(setting_info_window, 0x1B001E, 0);
                        HuWinInsertMesSet(setting_info_window, 0x1B001F, 1);
                        HuWinInsertMesSet(setting_info_window, 0x1B0020, 2);
                        WinSetMessAndWait(setting_info_window, 0x1B0016, -1, -0x3E7);
                        break;
                    case 2:
                        HuWinInsertMesSet(setting_info_window, 0x1B001F, 0);
                        HuWinInsertMesSet(setting_info_window, 0x1B001E, 1);
                        HuWinInsertMesSet(setting_info_window, 0x1B0020, 2);
                        WinSetMessAndWait(setting_info_window, 0x1B0016, -1, -0x3E7);
                        break;
                    case 3:
                        HuWinInsertMesSet(setting_info_window, 0x1B0020, 0);
                        HuWinInsertMesSet(setting_info_window, 0x1B001E, 1);
                        HuWinInsertMesSet(setting_info_window, 0x1B001F, 2);
                        WinSetMessAndWait(setting_info_window, 0x1B0016, -1, -0x3E7);
                        break;
                }
                break;
            case 1:
                sprintf(max_turns_buffer, "%d", board_configs[1].board_settings[1]);
                HuWinInsertMesSet(setting_info_window, MAKE_MESSID_PTR(max_turns_buffer), 0);
                WinSetMessAndWait(setting_info_window, 0x1B0017, -1, -0x3E7);
                break;
            case 2:
                WinSetMessAndWait(setting_info_window, board_configs[1].board_settings[2] + 0x1B0018, -1, -0x3E7);
                break;
            case 3:
                WinSetMessAndWait(setting_info_window, board_configs[1].board_settings[3] + 0x1B001B, -1, -0x3E7);
                break;
            case 4:
                HuWinInsertMesSet(setting_info_window, mtPlayerData[board_configs[1].board_settings[4]].character, 0);
                WinSetMessAndWait(setting_info_window, 0x1B001D, -1, -0x3E7);
                break;
        }
    }
    if (board_configs[0].handicaps[3] != 0) {
        goto loop_4;
    }
    if (board_configs[0].handicaps[1] == -1) {
        hideHighlighter();
        next_section = 0;
    }
    else if (board_configs[0].handicaps[1] == 1) {
        hideHighlighter();
        next_section = 2;
    }
    else {
        goto loop_4;
    }
    destroyAvailControlsWin(0);
    board_configs[1].obj_callback = NULL;
    hideHighlighter();
    if (next_section == 0) {
        next_section = 1;
    }
    else if (next_section == 1) {
        motionShiftIfChanged(silly_guys[map].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(setting_info_window, 0x1A0003, -1, 5);
        motionShiftIfChanged(silly_guys[map].obj, 1, 1, 0xF, 1);
        confirm_window = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
        if (confirm_window == 0) {
            DestroyWin(setting_info_window);
            goToNextOverlay(0);
        }
        goto loop_3;
    }
    else if (next_section == 2) {
        HuAudFXPlay(menuSoundFXTbl[map][2]);
        WinSetMessAndWait(setting_info_window, 0x1B0000, -1, 5);
        confirm_window = OpenConfirmDlgYesDef(0x1E0035, 3, 0);
        if (confirm_window == 0) {
            next_section = 0x63;
        }
        else {
            WinSetMessAndWait(setting_info_window, 0x1B0007, -1, 5);
            confirm_window = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
            if (confirm_window == 0) {
                next_section = 0;
            }
            else {
                goto loop_3;
            }
        }
    }
    else {
        goto loop_3;
    }
    DestroyWin(setting_info_window);
    return next_section;
}

void fn_1_A990(void)
{
    pushUpBoardSettings();
}

void transitionToPartyBoard(void)
{
    s32 is_bowser_gnarly;
    s32 map;
    s32 i;
    s32 window;

    map = gameConfigs[2];
    {
        Vec new_pos = { -120.0f, 0.0f, 670.0f };
        if (gameConfigs[2] == 5) {
            is_bowser_gnarly = 1;
        }
        else {
            is_bowser_gnarly = 0;
        }
        for (i = 0; i < 6; i++) {
            Hu3DModelAttrSet(silly_guys[i].obj->mdlId[1], HU3D_ATTR_DISPOFF);
        }
        Hu3DModelAttrReset(silly_guys[map].obj->mdlId[1], HU3D_ATTR_DISPOFF);
        createAndOpenChest(board_configs[is_bowser_gnarly].obj, gameConfigs[2]);
        Hu3DModelAttrReset(board_configs[is_bowser_gnarly].obj->mdlId[4], HU3D_ATTR_DISPOFF);
        Hu3DModelAttrReset(board_configs[is_bowser_gnarly].obj->mdlId[5], HU3D_ATTR_DISPOFF);
        board_configs[is_bowser_gnarly].obj_callback = (MentDllUnkFunc)oscillateChest;
        if (map == 5) {
            new_pos.x += 1200.0f;
        }
        motionShift(silly_guys[map].obj, 1, 3, 0xF, 1);
        MenuMoveChar(silly_guys[map].obj, 1, new_pos, 5.0f, 6.0f, 10.0f, 1, 1);
        motionShift(silly_guys[map].obj, 1, 1, 0xF, 1);
        HuDataDirClose(DATADIR_MENT);
        HuPrcChildCreate(LoadBoard, 0x64, 0x3000, 0, menuProc);
        MenuPrcSleep(0x1E);
        motionShift(board_configs[is_bowser_gnarly].obj, 2, 3, 0, 0);
        window = OpenWinBottom(0, 0, 0);
        HuAudFXPlay(menuSoundFXTbl[map][0]);
        motionShiftIfChanged(silly_guys[map].obj, 1, 5, 0xF, 1);
        WinSetMessAndWait(window, map + 0x1A0010, -1, -1);
        DestroyWin(window);
        HuAudSeqFadeOut(audio_channel_idx[0], 0xBB8);
        WaitAnimEnd(board_configs[is_bowser_gnarly].obj, 2, 0);
        Hu3DModelAttrSet(board_configs[is_bowser_gnarly].obj->mdlId[2], HU3D_ATTR_DISPOFF);
        moveCameraWithMethod(fn_1_14418);
        MenuPrcSleep(0xA);
        HuAudFXPlay(GWPlayerCfg->character + 0x75);
        HuAudFXPlay(GWPlayerCfg[1].character + 0x75);
        HuAudFXPlay(GWPlayerCfg[2].character + 0x75);
        HuAudFXPlay(GWPlayerCfg[3].character + 0x75);
        MenuPrcSleep(0x8C);
    }
}

void resetToChooseBoard(void)
{
    s32 i;
    s32 map;
    s32 map_same;
    s32 board_config_idx;

    s32 silly_guys_line_idx[6] = { 1, 2, 0, 3, 4, 5 };
    map = gameConfigs[2];
    map_same = gameConfigs[2];
    if (map != 5) {
        board_config_idx = 0;
    }
    else {
        board_config_idx = 1;
    }
    motionShift(board_configs[board_config_idx].obj, 2, 3, 0, 0);
    WaitAnimEnd(board_configs[board_config_idx].obj, 2, 0);
    Hu3DModelAttrSet(board_configs[0].obj->mdlId[2], HU3D_ATTR_DISPOFF);
    Hu3DModelAttrSet(board_configs[1].obj->mdlId[2], HU3D_ATTR_DISPOFF);
    if (map != 5) {
        moveCameraWithMethod(fn_1_146D0);
    }
    else {
        moveCameraWithMethod(fn_1_147C0);
    }
    {
        Vec new_pos = { 0.0f, 0.0f, 560.0f };
        new_pos.x = silly_guys_pos[map].x;
        motionShift(silly_guys[map].obj, 1, 3, 0xF, 1);
        MenuMoveChar(silly_guys[map].obj, 1, new_pos, silly_guys_angle[map], 6.0f, 10.0f, 1, 1);
        motionShift(silly_guys[map].obj, 1, 1, 0xF, 1);
        for (i = 0; i < 6; i++) {
            silly_guys[i].unk_08 = 0;
            silly_guys[i].objCallback = (MentDllUnkFunc)chooseBoardSillyGuyPos;
        }
        silly_guys[silly_guys_line_idx[map_same]].unk_08 = 1;
        moveCameraWithMethod(fn_1_14058);
        if (map_same != 2) {
            map_same = 2;
            for (i = 0; i < 6; i++) {
                silly_guys[i].unk_08 = 0;
            }
            silly_guys[silly_guys_line_idx[map_same]].unk_08 = 1;
        }
        RescaleChooseBoardGrp();
        MenuPrcSleep(0x1E);
    }
}

void resetToBeginning(void)
{
    s32 sp28;
    s32 sp24;
    s32 sp20;
    s32 sp1C;
    s32 sp18;
    float sp14;
    s32 sp10;
    s32 spC;
    s32 sp8;
    s32 i;
    MTPlayerConfig *player;
    s32 j;
    s32 map;
    s32 board_config_idx;
    MTPlayerConfig *player_same;
    MentDllUnkBss33ACStruct *silly_guy;
    MentBoardMenuConfig *board_config;

    map = gameConfigs[2];
    if (map != 5) {
        board_config_idx = 0;
    }
    else {
        board_config_idx = 1;
    }
    for (i = 0; i < 6; i++) {
        if (i != map) {
            Hu3DModelAttrSet(silly_guys[i].obj->mdlId[1], HU3D_ATTR_DISPOFF);
        }
    }
    motionShift(board_configs[board_config_idx].obj, 2, 3, 0, 0);
    WaitAnimEnd(board_configs[board_config_idx].obj, 2, 0);
    if (map != 5) {
        moveCameraWithMethod(fn_1_146D0);
    }
    else {
        moveCameraWithMethod(fn_1_147C0);
    }
    {
        Vec new_pos = { 0.0f, 0.0f, 460.0f };

        if (map == 5) {
            new_pos.x = 1200.0f;
        }
        motionShift(silly_guys[map].obj, 1, 3, 0xF, 1);
        MenuMoveChar(silly_guys[map].obj, 1, new_pos, 0.0f, 6.0f, 10.0f, 1, 1);
        motionShift(silly_guys[map].obj, 1, 1, 0xF, 1);
        motionShift(board_configs[board_config_idx].obj, 2, 2, 0, 0);
        WaitAnimEnd(board_configs[board_config_idx].obj, 2, 0);
        motionShift(board_configs[0].obj, 2, 3, 0, 2);
        Hu3DModelAttrReset(board_configs[0].obj->mdlId[2], HU3D_ATTR_DISPOFF);
        if (map == 5) {
            moveCameraWithMethod(fn_1_14058);
        }
        MenuPrcSleep(0x1E);
        for (i = 0; i < 4; i++) {
            player = &mtPlayerData[i];
            player_same = &mtPlayerData[i];
            player->character_highlighted = i;
            player->group = 0;
            player->iscom = player->character_highlighted;
            if (player->iscom > 0) {
                player->iscom = 1;
            }
            player->diff = 0;
            player->character = player->character_highlighted;
            player->pad_idx = player->character_highlighted;
            for (j = 0; j < 4; j++) {
                player->unk_70[0] = player->unk_70[1] = 0;
            }
            for (j = 0; j < 5; j++) {
                player_same->unk_08 = player_same->unk_0C = 0;
            }
        }
        for (i = 0; i < 6; i++) {
            silly_guy = &silly_guys[i];
            for (j = 0; j < 5; j++) {
                silly_guy->unk_08 = silly_guy->unk_0C = 0;
            }
            Hu3DModelPosSet(silly_guys[i].obj->mdlId[1], silly_guys_pos[i].x, silly_guys_pos[i].y, silly_guys_pos[i].z);
            Hu3DModelRotSet(silly_guys[i].obj->mdlId[1], 0.0f, silly_guys_angle[i], 0.0f);
            Hu3DModelAttrReset(silly_guys[i].obj->mdlId[1], HU3D_ATTR_DISPOFF);
        }
        for (i = 0; i < 2; i++) {
            board_config = &board_configs[i];
            for (j = 0; j < 5; j++) {
                board_config->handicaps[0] = board_config->handicaps[1] = 0;
            }
            Hu3DModelAttrSet(board_configs[1].obj->mdlId[2], HU3D_ATTR_DISPOFF);
        }
        hideChooseCharacters();
        resetChosenCharactersOverlay();
        resetBoardOverviews();
        hideBowserGnarlyBoardSettings();
        motionShift(board_configs[0].obj, 2, 3, 0, 0);
        WaitAnimEnd(board_configs[0].obj, 2, 0);
        Hu3DModelAttrSet(board_configs[0].obj->mdlId[2], HU3D_ATTR_DISPOFF);
        {
            Vec new_pos = { 0.0f, 0.0f, 560.0f };
            motionShiftIfChanged(silly_guys[0].obj, 1, 2, 0xF, 1);
            MenuMoveChar(silly_guys[0].obj, 1, new_pos, 0.0f, 3.0f, 0.0f, 1, 0);
            motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
        }
    }
}

void openPartyMenu(OMOBJ *obj, MentBoardMenuConfig *handler_holder)
{
    s32 i;
    s32 configure_section; // Tells which part of configure section we are. 10 is human count, 0 is character selection
    s32 sub_section;  // Variable used to approximately indicate which sub_section we are in, probably a merge of multiple variables
    s32 mg_list;
    s32 has_team;
    s32 has_bonus_star;

    configure_section = 0xA;
    sub_section = 0;
    // OSReport("XD1\n");
    if (gameConfigs[1] == 0) {
        MenuStartScene();
        MenuRunPartyIntro();
        MenuPartyAfterIntroPrelinimaries();
        boardPartyTutorialPrompt();
    }
    else {
        MenuStartScene2();
        boardPartyTutorialPrompt();
    }
    do {
        MenuPrcVSleep();
        switch (configure_section) {
            case 10:
                configure_section = 0;
                moveHumancountGroup(sub_section);
                sub_section = human_count_menu();
                character_selection_init();
                break;
            case 0:
                configure_section = 1;
                pushDownCharacterSelection();
                while (1) {
                    MenuPrcVSleep();
                    switch (sub_section) {
                        case 1:
                            sub_section = playerSelection();
                            break;
                        case 2:
                            sub_section = computer_players_selection();
                            break;
                    }
                    if (sub_section == 0) {
                        configure_section = 0xA;
                        break;
                    }
                    else if (sub_section == 3) {
                        break;
                    }
                }
                pushUpCharacterSelection();
                break;
            case 1:
                configure_section = 2;
                pushDownChosenCharactersOverlay();
                sub_section = chooseBoardMenu();
                if (sub_section != 3) {
                    pushUpChosenCharactersOverlay();
                    configure_section = 0;
                }
                else {
                    preMapRulesSelectionPrep();
                }
                break;
            case 2:
                pushDownBoardSettings();
                configure_section = getBoardSettings();
                pushUpBoardSettings();
                switch (configure_section) {
                    case 0:
                        configure_section = 0xA;
                        sub_section = 0;
                        resetToBeginning();
                        break;
                    case 1:
                        configure_section = 1;
                        resetToChooseBoard();
                        break;
                }
                break;
        }
    } while (configure_section != 0x63);
    // OSReport("XD2\n");
    if (board_configs[1].board_settings[0] == 0) {
        has_team = 0;
    }
    else {
        has_team = 1;
    }
    if (board_configs[1].board_settings[3] == 0) {
        has_bonus_star = 1;
    }
    else {
        has_bonus_star = 0;
    }
    switch (board_configs[1].board_settings[2]) {
        case 0:
            mg_list = 0;
            break;
        case 1:
            mg_list = 1;
            break;
        case 2:
            mg_list = 2;
            break;
        default:
            mg_list = 0;
            break;
    }
    {
        s32 max_turn = board_configs[1].board_settings[1];
        s32 p1_handicap = board_configs[1].handicaps[1];
        s32 p2_handicap = board_configs[1].handicaps[2];
        s32 p3_handicap = board_configs[1].handicaps[3];
        s32 p4_handicap = board_configs[1].handicaps[4];
        BoardPartyConfigSet(has_team, has_bonus_star, mg_list, max_turn, p1_handicap, p2_handicap, p3_handicap, p4_handicap);
    }
    for (i = 0; i < 4; i++) {
        GWPlayerCfg[i].character = mtPlayerData[i].character;
        GWPlayerCfg[i].pad_idx = mtPlayerData[i].pad_idx;
        GWPlayerCfg[i].diff = mtPlayerData[i].diff;
        GWPlayerCfg[i].group = 0;
        GWPlayerCfg[i].iscom = mtPlayerData[i].iscom;
    }
    if (has_team == 1) {
        switch (board_configs[1].board_settings[0]) {
            case 1:
                GWPlayerCfg[2].group = GWPlayerCfg[3].group = 1;
                break;
            case 2:
                GWPlayerCfg[1].group = GWPlayerCfg[3].group = 1;
                break;
            case 3:
                GWPlayerCfg[1].group = GWPlayerCfg[2].group = 1;
                break;
            default:
                GWPlayerCfg[2].group = GWPlayerCfg[3].group = 1;
                break;
        }
    }
    BoardSaveInit(gameConfigs[2]);
    transitionToPartyBoard();
    if (gameConfigs[2] != 5) {
        WipeColorSet(0xFF, 0xFF, 0xFF);
    }
    else {
        WipeColorSet(0, 0, 0);
    }
    goToNextOverlay(3);
}

void goToNextOverlay(s32 next_overlay) // 0: previous overlay - 1 or 3: call selected board overlay - 2: call tutorial board
{
    s32 overlay_dlls[7] = { 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F };
    if (next_overlay == 0 || next_overlay == 1 || next_overlay == 2) {
        MenuPrcSleep(0x3C);
        HuAudSeqFadeOut(audio_channel_idx[0], 0x3E8);
    }
    WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_NORMAL, -1);

    while (WipeStatGet() != 0) {
        MenuPrcVSleep();
    }
    CharModelKill(-1);
    MGSeqKillAll();

    if (next_overlay != 0) {
        if (next_overlay == 1 || next_overlay == 3) {
            OMOVLHIS *sp8 = omOvlHisGet(0);
            omOvlHisChg(0, DLL_mstory3dll, 0, 0);
        }
        do {
            MenuPrcVSleep();
        } while (is_board_loaded != 1);
        CharMotionInit(GWPlayerCfg[0].character);
        CharMotionInit(GWPlayerCfg[1].character);
        CharMotionInit(GWPlayerCfg[2].character);
        CharMotionInit(GWPlayerCfg[3].character);
        HuAudSeqAllFadeOut(0x3E8);
        HuAudSStreamAllFadeOut(0x3E8);
    }

    switch (next_overlay) {
        case 0:
            omOvlReturnEx(1, 1);
            break;
        case 2:
            omOvlCallEx(overlay_dlls[6], 1, 0, 0);
            break;
        case 1:
        case 3:
            omOvlCallEx(overlay_dlls[GWSystem.board], 1, 0, 0);
            break;
    }
    while (1) {
        MenuPrcVSleep();
    }
}

s32 fn_1_C354(s32 arg0)
{
    if (arg0 == 0 && _CheckFlag(FLAG_ID_MAKE(0, 2)) != 0) {
        return 1;
    }
    if (arg0 == 1 && _CheckFlag(FLAG_ID_MAKE(0, 3)) != 0) {
        return 1;
    }
    if (arg0 == 2 && _CheckFlag(FLAG_ID_MAKE(0, 4)) != 0) {
        return 1;
    }
    if (arg0 == 3 && _CheckFlag(FLAG_ID_MAKE(0, 5)) != 0) {
        return 1;
    }
    if (arg0 == 4 && _CheckFlag(FLAG_ID_MAKE(0, 6)) != 0) {
        return 1;
    }
    if (arg0 == 5 && _CheckFlag(FLAG_ID_MAKE(0, 7)) != 0) {
        return 1;
    }
    return 0;
}

s32 fn_1_C440(void)
{
    if ((_CheckFlag(FLAG_ID_MAKE(0, 2)) != 0) && (_CheckFlag(FLAG_ID_MAKE(0, 3)) != 0) && (_CheckFlag(FLAG_ID_MAKE(0, 4)) != 0)
        && (_CheckFlag(FLAG_ID_MAKE(0, 5)) != 0) && (_CheckFlag(FLAG_ID_MAKE(0, 6)) != 0)) {
        return 1;
    }
    return 0;
}

void MenuStoryAfterIntroPrelinimaries(void)
{
    s32 window;
    s32 choice;

    window = OpenWinBottom(0, 0, 0);
    audio_channel_idx[0] = HuAudSeqPlay(0x30);
    motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(window, 0x1E005C, -1, -1);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    DestroyWin(window);
    saveExecF = 0;
    if (GWGameStat.story_continue != 1) {
        return;
    }
    window = OpenWinBottom(0, 0, 0);
    while (1) {
        MenuPrcVSleep();
        motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(window, 0x1E0001, -1, 5);
        motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
        choice = OpenConfirmDlgYesDef(0x1E0035, 3, 0);
        if (choice == -1) {
            motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
            WinSetMessAndWait(window, 0x1E0003, -1, 5);
            motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
            choice = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
            if (choice == 0) {
                DestroyWin(window);
                goToNextOverlay(0);
            }
            continue;
        }
        if (choice == 0) {
            DestroyWin(window);
            SLLoadBoardStory();
            if (_CheckFlag(FLAG_ID_MAKE(0, 7)) != 0 && _CheckFlag(FLAG_ID_MAKE(0, 9)) != 0) {
                OSReport("########## Next to Ending\n");
                fn_1_E654(1);
                omOvlGotoEx(DLL_mstory2dll, 1, 4, 0);
            }
            else {
                if (_CheckFlag(0x10000) != 0) {
                    OSReport("########### Next to Turn\n");
                    HuDataDirClose(DATADIR_MENT);
                    HuPrcChildCreate(LoadBoard, 0x64, 0x3000, 0, menuProc);
                    MenuPrcVSleep();
                    fn_1_E71C(0);
                }
                else {
                    if (fn_1_C354(GWSystem.board) != 0) {
                        if (_CheckFlag(FLAG_ID_MAKE(0, 9)) != 0) {
                            if (fn_1_C440() != 0) {
                                OSReport("########## Next to MapSelect KoopaEvent\n");
                                fn_1_E654(1);
                                omOvlGotoEx(DLL_mstory2dll, 1, 0, 0);
                            }
                            else {
                                OSReport("########## Next to MapSelect NormalEvent\n");
                                fn_1_E654(1);
                                omOvlGotoEx(DLL_mentdll, 1, 0xA, 0);
                            }
                        }
                        else if (GWSystem.board == 5) {
                            OSReport("########## Next to BoardClear KoopaEvent\n");
                            fn_1_E654(0);
                            omOvlGotoEx(DLL_mstory2dll, 1, 1, 0);
                        }
                        else {
                            OSReport("########## Next to BoardClear NormalEvent\n");
                            fn_1_E654(0);
                            omOvlGotoEx(DLL_mstorydll, 1, 0, 0);
                        }
                    }
                    else {
                        if (_CheckFlag(FLAG_ID_MAKE(0, 9)) != 0) {
                            if (GWSystem.board == 5) {
                                OSReport("########## Next to BoardMiss KoopaEvent\n");
                                fn_1_E654(0);
                                omOvlGotoEx(DLL_mstory2dll, 1, 2, 0);
                            }
                            else {
                                OSReport("########### Next to BoardMiss NormalEvent\n");
                                fn_1_E654(0);
                                omOvlGotoEx(DLL_mstorydll, 1, 1, 0);
                            }
                        }
                        else {
                            OSReport("########### Next to Turn\n");
                            HuDataDirClose(DATADIR_MENT);
                            HuPrcChildCreate(LoadBoard, 0x64, 0x3000, 0, menuProc);
                            MenuPrcVSleep();
                            fn_1_E71C(0);
                        }
                    }
                }
            }
            while (1) {
                MenuPrcVSleep();
            }
        }
        if (choice == 1) {
            motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
            WinSetMessAndWait(window, 0x1E0002, -1, -1);
            motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
            DestroyWin(window);
            break;
        }
    }
}

void boardStoryTutorialPrompt(void)
{
    s32 var_r31;
    s32 var_r30;
    s32 var_r29;

    var_r30 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(var_r30, 0x1A0004, -1, 5);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    var_r29 = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
    DestroyWin(var_r30);
    if (var_r29 == 0) {
        s32 spC[4] = { 3, 0, 2, 4 };
        _SetFlag(0x1000B);
        for (var_r31 = 0; var_r31 < 4; var_r31++) {
            GWPlayerCfg[var_r31].character = spC[var_r31];
            GWPlayerCfg[var_r31].pad_idx = var_r31;
            GWPlayerCfg[var_r31].diff = 0;
            GWPlayerCfg[var_r31].group = 0;
            GWPlayerCfg[var_r31].iscom = 1;
            OSReport("ID-%d CHR-%d PAD-%d DIF-%d GRP-%d COM-%d\n", var_r31, GWPlayerCfg[var_r31].character, GWPlayerCfg[var_r31].pad_idx,
                GWPlayerCfg[var_r31].diff, GWPlayerCfg[var_r31].group, GWPlayerCfg[var_r31].iscom);
        }
        BoardSaveInit(6);
        GWSystem.max_turn = 0x14;
        HuDataDirClose(DATADIR_MENT);
        HuPrcChildCreate(LoadBoard, 0x64, 0x3000, 0, menuProc);
        {
            OMOVLHIS *sp8 = omOvlHisGet(0);
        }
        omOvlHisChg(0, DLL_mentdll, 1, 1);
        goToNextOverlay(2);
    }
    else {
        _ClearFlag(0x1000B);
    }
}

void fn_1_CD6C(void)
{
    pushDownStorySettings();
}

s32 selectStoryCharacter(void)
{
    s32 sp8;
    s32 window;
    s32 character_selected;
    s32 choice;

    sp8 = 0;
    initCharacterSelectionPos();
    window = OpenWinBottom(0, 0, 0);
    while (1) {
        MenuPrcVSleep();
        motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(window, 0x1E005E, -1, -1);
        motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
        OpenAvailControlsWin(0x1A0020);
        mtPlayerData->proc_selection_handler = (MentDllUnkFunc)selectStoryCharacterCallback;
        while (1) {
            if (mtPlayerData->unk_70[0] == 1) {
                character_selected = 1;
                break;
            }
            else if ((HuPadBtnDown[mtPlayerData->pad_idx] & PAD_BUTTON_B) != 0) {
                HuAudFXPlay(3);
                mtPlayerData->proc_selection_handler = NULL;
                character_selected = 0;
                break;
            }
            else {
                MenuPrcVSleep();
            }
        }
        mtPlayerData->proc_selection_handler = NULL;
        destroyAvailControlsWin(0);
        if (character_selected != 0) {
            break;
        }
        motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(window, 0x1E0005, -1, 5);
        motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
        choice = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
        if (choice == 0) {
            DestroyWin(window);
            goToNextOverlay(0);
        }
    }
    DestroyWin(window);
    MenuPrcSleep(0x1E);
    return character_selected;
}

s32 setStorySettingValues(void)
{
    s32 sp8;
    s32 window;
    s32 section;
    s32 choice;
    s32 setting_index;
    s32 setting_value;

    sp8 = 0;
    setting_index = -1;
    setting_value = -1;
    window = OpenWinBottom(0, 0, 0);
    while (1) {
        MenuPrcVSleep();
        motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(window, 0x1E0009, -1, -1);
        motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
        OpenAvailControlsWin(0x1A0021);
        board_configs[1].handicaps[1] = 0;
        fn_1_1DED8();
        board_configs[0].obj_callback = (MentDllUnkFunc)selectStorySettingValesCallback;
        while (1) {
            MenuPrcVSleep();
            if ((setting_index != board_configs[0].handicaps[0]) || (setting_value != board_configs[0].board_settings[board_configs[0].handicaps[0] - 1])) {
                setting_index = board_configs[0].handicaps[0];
                setting_value = board_configs[0].board_settings[board_configs[0].handicaps[0] - 1];
                switch (board_configs[0].handicaps[0]) {
                    case 1:
                        HuWinInsertMesSet(window, board_configs[0].board_settings[0] + 0x1B0028, 0);
                        WinSetMessAndWait(window, 0x1B0027, -1, -0x3E7);
                        break;
                    case 2:
                        WinSetMessAndWait(window, board_configs[0].board_settings[1] + 0x1B0018, -1, -0x3E7);
                        break;
                }
            }
            if (board_configs[0].handicaps[3] != 0) {
                continue;
            }
            if (board_configs[1].handicaps[1] == 1) {
                section = 2;
                break;
            }
            else if (board_configs[1].handicaps[1] == -1) {
                section = 0;
                break;
            }
        }
        fn_1_1DF48();
        board_configs[0].obj_callback = NULL;
        destroyAvailControlsWin(0);
        if (section == 1) {
            motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
            WinSetMessAndWait(window, 0x1E0005, -1, 5);
            motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
            choice = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
            if (choice == 0) {
                DestroyWin(window);
                goToNextOverlay(0);
            }
        }
        else if (section == 2) {
            WinSetMessAndWait(window, 0x1B0000, -1, 5);
            choice = OpenConfirmDlgYesDef(0x1E0035, 3, 0);
            if (choice == 0) {
                break;
            }
            WinSetMessAndWait(window, 0x1B0007, -1, 5);
            choice = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
            if (choice == 0) {
                initCharacterSelectionPos();
                section = 0;
                break;
            }
            else {
                setting_index = -1;
                setting_value = -1;
            }
        }
        else {
            section = 0;
            break;
        }
    }
    story_mg_list = board_configs[0].board_settings[1];
    DestroyWin(window);
    return section;
}

void fn_1_D310(void)
{
    fn_1_1E1B4();
}

void fn_1_D330(void)
{
    fn_1_1F868();
}

void fn_1_D350(void)
{
    s32 var_r31;
    s32 var_r30;
    s32 var_r29;

    MenuPrcSleep(0x3C);
    var_r29 = OpenWinBottom(0, 0, 0);
    birthdayConfettiObj->work[0] = 1;
    var_r31 = -1;
    var_r31 = HuAudFXPlay(0x43);
    HuAudFXPanning(var_r31, 0x20);
    var_r31 = HuAudFXPlay(0x40);
    HuAudFXPanning(var_r31, 0x30);
    var_r31 = HuAudFXPlay(0x37);
    HuAudFXPanning(var_r31, 0x40);
    var_r31 = HuAudFXPlay(0x4B);
    HuAudFXPanning(var_r31, 0x4C);
    var_r31 = HuAudFXPlay(0x46);
    HuAudFXPanning(var_r31, 0x60);
    HuAudFXPlay(0x9A);
    for (var_r30 = 0; var_r30 < 5; var_r30++) {
        motionShiftIfChanged(silly_guys[var_r30].obj, 1, 6, 5, 1);
    }
    HuWinInsertMesSet(var_r29, mtPlayerData->character, 0);
    WinSetMessAndWait(var_r29, 0x1E0006, 0, 0xB4);
    for (var_r30 = 0; var_r30 < 5; var_r30++) {
        motionShiftIfChanged(silly_guys[var_r30].obj, 1, 1, 0xF, 1);
    }
    MenuPrcSleep(0x1E);
    motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
    HuWinInsertMesSet(var_r29, mtPlayerData->character, 0);
    WinSetMessAndWait(var_r29, 0x1E0007, -1, -1);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    moveCameraWithMethod(fn_1_148B0);
    HuAudFXPlay(0x39);
    motionShiftIfChanged(silly_guys[0].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(var_r29, 0x1E0008, 0, 0x78);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
    moveCameraWithMethod(MenuIntroBackOffAfterSillyGuys);
    DestroyWin(var_r29);
    MenuPrcSleep(0x78);
    birthdayConfettiObj->objFunc = NULL;
    _ClearFlag(2);
    _ClearFlag(3);
    _ClearFlag(4);
    _ClearFlag(5);
    _ClearFlag(6);
    _ClearFlag(7);
    _ClearFlag(1);
    _ClearFlag(9);
}

void fn_1_D634(void)
{
    s32 var_r31;
    s32 i;
    s32 var_r29;
    s32 var_r28;
    s32 var_r27;
    s32 var_r26;

    s32 sp8[6] = { 1, 2, 0, 3, 4, 5 };
    var_r28 = 0;
    var_r31 = 2;

    if (lbl_1_data_2F0 == -1) {
        var_r29 = OpenWinBottom(0, 0, 0);
    }
    else {
        var_r29 = lbl_1_data_2F0;
        lbl_1_data_2F0 = -1;
    }
    while (1) {
        MenuPrcVSleep();
        var_r28 = 0;
        silly_guys[0].unk_34 = 1;
        HuAudFXPlay(menuSoundFXTbl[0][3]);
        motionShiftIfChanged(silly_guys[sp8[var_r31]].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(var_r29, 0x1A0008, -1, -1);
        motionShiftIfChanged(silly_guys[sp8[var_r31]].obj, 1, 1, 0xF, 1);
        OpenAvailControlsWin(0x1A0020);
        for (i = 0; i < 5; i++) {
            silly_guys[i].unk_08 = 0;
            silly_guys[i].objCallback = (MentDllUnkFunc)chooseBoardSillyGuyPos;
        }
        silly_guys[sp8[var_r31]].unk_08 = 1;
        board_configs[0].obj_callback = (MentDllUnkFunc)fn_1_18F74;
        while (1) {
            MenuPrcVSleep();
            for (i = 0; i < 5; i++) {
                if (silly_guys[i].unk_08 != 2) {
                    break;
                }
            }
            if (i == 5 && var_r28 >= 0x15) {
                if ((HuPadBtnDown[mtPlayerData->pad_idx] & 0x100)) {
                    HuAudFXPlay(2);
                    var_r26 = 2;
                    break;
                }

                if ((HuPadStkX[mtPlayerData[0].pad_idx] >= 0x32) || ((HuPadBtn[mtPlayerData[0].pad_idx] & PAD_BUTTON_RIGHT) != 0)) {
                    var_r31++;
                    if (var_r31 > 4) {
                        var_r31 = 4;
                    }
                    else {
                        var_r28 = 0;
                    }
                }
                else if ((HuPadStkX[mtPlayerData[0].pad_idx] <= -50) || ((HuPadBtn[mtPlayerData[0].pad_idx] & PAD_BUTTON_LEFT) != 0)) {
                    var_r31--;
                    if (var_r31 < 0) {
                        var_r31 = 0;
                    }
                    else {
                        var_r28 = 0;
                    }
                }
                else if ((HuPadBtnDown[0] & 0x200)) {
                    var_r26 = 1;
                    break;
                }
            }
            if (var_r28 == 0) {
                for (i = 0; i < 5; i++) {
                    silly_guys[i].unk_08 = 0;
                }
                silly_guys[sp8[var_r31]].unk_08 = 1;
            }
            if (var_r28++ == 0x14) {
                var_r28 = 0x15;
                fn_1_18A54(sp8[var_r31]);
                WinSetMessAndWait(var_r29, sp8[var_r31] + 0x1B0001, -1, -0x3E7);
                if (sp8[var_r31] != 0) {
                    HuAudFXPlay(menuSoundFXTbl[sp8[var_r31]][0]);
                }
                else {
                    HuAudFXPlay(menuSoundFXTbl[sp8[var_r31]][2]);
                }
            }
        }
        destroyAvailControlsWin(0);
        if (var_r26 == 1) {
            moveCameraWithMethod(fn_1_14058);
            if (var_r31 != 2) {
                var_r31 = 2;
                for (i = 0; i < 5; i++) {
                    silly_guys[i].unk_08 = 0;
                }
                silly_guys[sp8[var_r31]].unk_08 = 1;
                silly_guys[sp8[var_r31]].unk_34 = 1;
            }
            RescaleChooseBoardGrp();
            MenuPrcSleep(0x1E);
            WinSetMessAndWait(var_r29, 0x1E0005, -1, 5);
            var_r27 = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
            if (var_r27 == 0) {
                DestroyWin(var_r29);
                goToNextOverlay(0);
            }
            continue;
        }
        if (var_r26 != 2) {
            continue;
        }
        if (((sp8[var_r31] == 0) && (_CheckFlag(FLAG_ID_MAKE(0, 2)) != 0)) || ((sp8[var_r31] == 1) && (_CheckFlag(FLAG_ID_MAKE(0, 3)) != 0))
            || ((sp8[var_r31] == 2) && (_CheckFlag(FLAG_ID_MAKE(0, 4)) != 0)) || ((sp8[var_r31] == 3) && (_CheckFlag(FLAG_ID_MAKE(0, 5)) != 0))
            || ((sp8[var_r31] == 4) && (_CheckFlag(FLAG_ID_MAKE(0, 6)) != 0))) {
            motionShiftIfChanged(silly_guys[sp8[var_r31]].obj, 1, 4, 0xF, 1);
            HuWinInsertMesSet(var_r29, sp8[var_r31] + 0x1B0021, 1);
            WinSetMessAndWait(var_r29, sp8[var_r31] + 0x1E0052, -1, -1);
            WinSetMessAndWait(var_r29, sp8[var_r31] + 0x1E0057, -1, 5);
            motionShiftIfChanged(silly_guys[sp8[var_r31]].obj, 1, 1, 0xF, 1);
            var_r27 = OpenConfirmDlgNoDef(0x1E0035, 3, 0);
            if (var_r27 == 0) {
                for (i = 0; i < 5; i++) {
                    silly_guys[i].objCallback = NULL;
                }
                gameConfigs[2] = sp8[var_r31];
            }
            else {
                if (var_r31 != 2) {
                    var_r31 = 2;
                    for (i = 0; i < 5; i++) {
                        silly_guys[i].unk_08 = 0;
                    }
                    silly_guys[sp8[var_r31]].unk_08 = 1;
                    silly_guys[sp8[var_r31]].unk_34 = 1;
                }
                RescaleChooseBoardGrp();
                MenuPrcSleep(0x1E);
                continue;
            }
        }
        else {
            for (i = 0; i < 5; i++) {
                silly_guys[i].objCallback = NULL;
            }
            gameConfigs[2] = sp8[var_r31];
        }
        break;
    }
    DestroyWin(var_r29);
}

void fn_1_DE60(void)
{
    s32 var_r31 = gameConfigs[2];
    {
        Vec sp14 = { -120.0f, 0.0f, 670.0f };

        fn_1_1FA34();
        moveCameraWithMethod(fn_1_14238);
        motionShift(silly_guys[var_r31].obj, 1, 3, 0xF, 1);
        MenuMoveChar(silly_guys[var_r31].obj, 1, sp14, 5.0f, 6.0f, 10.0f, 1, 1);
        motionShift(silly_guys[var_r31].obj, 1, 1, 0xF, 1);
        Hu3DModelAttrReset(board_configs[0].obj->mdlId[2], HU3D_ATTR_DISPOFF);
        motionShift(board_configs[0].obj, 2, 2, 0, 0);
        WaitAnimEnd(board_configs[0].obj, 2, 0);
        MenuPrcSleep(0x3C);
    }
}

void fn_1_DFDC(void)
{
    s32 var_r31;
    s32 var_r30;
    s32 var_r29;
    s32 var_r28;

    var_r31 = 0;
    var_r29 = gameConfigs[2];
    for (var_r30 = 0; var_r30 < 5; var_r30++) {
        Hu3DModelAttrSet(silly_guys[var_r30].obj->mdlId[1], HU3D_ATTR_DISPOFF);
    }
    Hu3DModelAttrReset(silly_guys[var_r29].obj->mdlId[1], HU3D_ATTR_DISPOFF);
    createAndOpenChest(board_configs[var_r31].obj, gameConfigs[2]);
    Hu3DModelAttrReset(board_configs[var_r31].obj->mdlId[4], HU3D_ATTR_DISPOFF);
    Hu3DModelAttrReset(board_configs[var_r31].obj->mdlId[5], HU3D_ATTR_DISPOFF);
    board_configs[var_r31].obj_callback = (MentDllUnkFunc)oscillateChest;
    HuDataDirClose(DATADIR_MENT);
    HuPrcChildCreate(LoadBoard, 0x64, 0x3000, 0, menuProc);
    motionShift(board_configs[var_r31].obj, 2, 3, 0, 0);
    var_r28 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[var_r29].obj, 1, 5, 0xF, 1);
    WinSetMessAndWait(var_r28, var_r29 + 0x1E000C, -1, -1);
    DestroyWin(var_r28);
    WaitAnimEnd(board_configs[var_r31].obj, 2, 0);
    Hu3DModelAttrSet(board_configs[var_r31].obj->mdlId[2], HU3D_ATTR_DISPOFF);
    HuAudSeqFadeOut(audio_channel_idx[0], 0xBB8);
    moveCameraWithMethod(fn_1_14418);
    MenuPrcSleep(0xA);
    HuAudFXPlay(GWPlayerCfg->character + 0x75);
    MenuPrcSleep(0x8C);
}

void fn_1_E244(void)
{
    audio_channel_idx[0] = HuAudSeqPlay(0x30);
    OSReport("########### ME_MainProcFunc200\n");
    WipeCreate(WIPE_MODE_IN, WIPE_TYPE_NORMAL, -1);
    while (WipeStatGet() != 0) {
        MenuPrcVSleep();
    }
    MenuPrcSleep(0x3C);
}

void fn_1_E2B4(void)
{
    Vec sp14 = { 0.0f, 0.0f, 560.0f };
    Hu3DModelAttrReset(board_configs[0].obj->mdlId[2], HU3D_ATTR_DISPOFF);
    motionShift(board_configs[0].obj, 2, 3, 0, 0);
    WaitAnimEnd(board_configs[0].obj, 2, 0);
    fn_1_D330();
    moveCameraWithMethod(fn_1_146D0);
    motionShiftIfChanged(silly_guys[0].obj, 1, 2, 0xF, 1);
    MenuMoveChar(silly_guys[0].obj, 1, sp14, 0.0f, 3.0f, 0.0f, 1, 0);
    motionShiftIfChanged(silly_guys[0].obj, 1, 1, 0xF, 1);
}

void fn_1_E3FC(void)
{
    audio_channel_idx[0] = HuAudSeqPlay(6);
    OSReport("########### ME_MainProcFunc300\n");
    WipeCreate(WIPE_MODE_IN, WIPE_TYPE_NORMAL, -1);
    while (WipeStatGet() != 0) {
        MenuPrcVSleep();
    }
    fn_1_D330();
    board_configs[1].obj_callback = (MentDllUnkFunc)fn_1_18F74;
    fn_1_18A54(5);
    MenuPrcSleep(0x3C);
}

void fn_1_E48C(void)
{
    s32 var_r31;

    gameConfigs[2] = 5;
    var_r31 = OpenWinBottom(0, 0, 0);
    HuAudFXPlay(menuSoundFXTbl[6][2]);
    WinSetMessAndWait(var_r31, 0x1F0008, 0, 0x3C);
    motionShiftIfChanged(lbl_1_bss_3354.obj, 1, 1, 0xF, 1);
    DestroyWin(var_r31);
    fn_1_1FC54();
    MenuPrcSleep(0x3C);
    Hu3DModelAttrReset(board_configs[1].obj->mdlId[4], HU3D_ATTR_DISPOFF);
    Hu3DModelAttrReset(board_configs[1].obj->mdlId[5], HU3D_ATTR_DISPOFF);
    fn_1_7304();
    HuDataDirClose(DATADIR_MENT);
    HuPrcChildCreate(LoadBoard, 0x64, 0x3000, 0, menuProc);
    motionShiftIfChanged(lbl_1_bss_3354.obj, 1, 2, 0xF, 1);
    motionShift(board_configs[1].obj, 2, 3, 0, 0);
    MenuPrcSleep(0x3C);
    HuAudFXPlay(menuSoundFXTbl[6][0]);
    WaitAnimEnd(board_configs[1].obj, 2, 0);
    Hu3DModelAttrSet(board_configs[1].obj->mdlId[2], HU3D_ATTR_DISPOFF);
    HuAudSeqFadeOut(audio_channel_idx[0], 0xBB8);
    moveCameraWithMethod(fn_1_14418);
    MenuPrcSleep(0xA);
    MenuPrcSleep(0x8C);
}

void fn_1_E654(s32 arg0)
{
    MenuPrcSleep(0x3C);
    HuAudSeqFadeOut(audio_channel_idx[0], 0x3E8);
    WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_NORMAL, -1);
    while (WipeStatGet() != 0) {
        MenuPrcVSleep();
    }
    CharModelKill(-1);
    MGSeqKillAll();
    HuAudAllStop();
    HuDataDirClose(DATADIR_MENT);
    CharMotionInit(GWPlayerCfg[0].character);
    if (arg0 == 0) {
        CharMotionInit(GWPlayerCfg[1].character);
        CharMotionInit(GWPlayerCfg[2].character);
        CharMotionInit(GWPlayerCfg[3].character);
    }
}

void fn_1_E71C(s32 arg0)
{
    s32 spC[7] = { 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F };
    MenuPrcSleep(0x3C);
    HuAudSeqFadeOut(audio_channel_idx[0], 0x3E8);
    WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_NORMAL, -1);
    while (WipeStatGet() != 0) {
        MenuPrcVSleep();
    }
    CharModelKill(-1);
    MGSeqKillAll();
    {
        OMOVLHIS *sp8 = omOvlHisGet(0);
    }
    omOvlHisChg(0, DLL_mstory3dll, 0, 0);
    do {
        MenuPrcVSleep();
    } while (is_board_loaded != 1);
    CharMotionInit(GWPlayerCfg[0].character);
    CharMotionInit(GWPlayerCfg[1].character);
    CharMotionInit(GWPlayerCfg[2].character);
    CharMotionInit(GWPlayerCfg[3].character);
    HuAudSeqAllFadeOut(0x3E8);
    HuAudSStreamAllFadeOut(0x3E8);
    omOvlCallEx(spC[GWSystem.board], 1, 0, 0);
    while (1) {
        MenuPrcVSleep();
    }
}

void openStoryMenu(OMOBJ *arg0, MentBoardMenuConfig *arg1)
{
    s32 i;
    s32 var_r29;
    s32 j;
    s32 cur_section;
    s32 mg_list;
    s32 diff;

    cur_section = 0;
    if (gameConfigs[1] == 0) {
        MenuStartScene();
        MenuRunStoryIntro();
        MenuStoryAfterIntroPrelinimaries();
        boardStoryTutorialPrompt();
    }
    else {
        MenuStartScene3();
        boardStoryTutorialPrompt();
    }
    pushDownStorySettings();
    do {
        MenuPrcVSleep();
        switch (cur_section) {
            case 0:
                cur_section = selectStoryCharacter();
                break;
            case 1:
                cur_section = setStorySettingValues();
                break;
        }
    } while (cur_section != 2);
    fn_1_D310();
    fn_1_81D8();
    fn_1_7A14();
    fn_1_D634();
    fn_1_DE60();
    switch (story_mg_list) {
        case 0:
            mg_list = 0;
            break;
        case 1:
            mg_list = 1;
            break;
        case 2:
            mg_list = 2;
            break;
        default:
            mg_list = 0;
            break;
    }
    if (board_configs[0].board_settings[0] == 4) {
        diff = 1;
    }
    else {
        diff = 0;
    }
    BoardStoryConfigSet(mg_list, diff);
    GWStoryCharSet(mtPlayerData[0].character);
    // looks a bit similar to fn_1_7304
    for (i = 1; i < 4; i++) {
        mtPlayerData[i].character = -1;
        mtPlayerData[i].pad_idx = i;
        mtPlayerData[i].iscom = 1;
    }
    for (i = 1; i < 4; i++) {
        do {
            mtPlayerData[i].character = rand8() % 8;
            for (j = 0; j < 4; j++) {
                if ((i != j) && (mtPlayerData[j].character != -1)
                    && (mtPlayerData[i].character == mtPlayerData[j].character)) {
                    break;
                }
            }
        } while (j != 4);
    }
    GWSystem.diff_story = board_configs[0].board_settings[0];
    for (i = 0; i < 4; i++) {
        mtPlayerData[i].diff = GWSystem.diff_story;
        if (GWSystem.diff_story == 4) {
            mtPlayerData[i].diff = fn_1_7124();
        }
    }
    for (i = 0; i < 4; i++) {
        GWPlayer[i].character = GWPlayerCfg[i].character = mtPlayerData[i].character;
        GWPlayer[i].port = GWPlayerCfg[i].pad_idx = mtPlayerData[i].pad_idx;
        GWPlayer[i].diff = GWPlayerCfg[i].diff = mtPlayerData[i].diff;
        GWPlayerCfg[i].group = 0;
        GWPlayer[i].com = GWPlayerCfg[i].iscom = mtPlayerData[i].iscom;
    }
    BoardSaveInit(gameConfigs[2]);
    fn_1_DFDC();
    fn_1_E71C(0);
}

void openStoryContinuationMenu(OMOBJ *arg0, MentBoardMenuConfig *arg1)
{
    _ClearFlag(FLAG_ID_MAKE(0, 9));
    board_configs[0].board_settings[0] = GWSystem.diff_story;
    fn_1_E244();
    fn_1_E2B4();
    fn_1_D634();
    fn_1_DE60();
    fn_1_7304();
    fn_1_DFDC();
    WipeColorSet(0xFF, 0xFF, 0xFF);
    fn_1_7684();
}

void openStoryGnarlyMenu(OMOBJ *arg0, MentBoardMenuConfig *arg1)
{
    _ClearFlag(9);
    board_configs[0].board_settings[0] = GWSystem.diff_story;
    fn_1_E3FC();
    fn_1_E48C();
    WipeColorSet(0, 0, 0);
    fn_1_7684();
}

void fn_1_10234(void)
{
    s32 var_r31;

    Vec sp2C = { 0.0f, 0.0f, 560.0f };
    Vec sp20 = { -120.0f, 0.0f, 670.0f };
    moveCameraWithMethod(MenuCameraMoveThroughIntro);
    MenuPrcSleep(0x5A);
    Hu3DModelAttrReset(silly_guys[4].obj->mdlId[1], HU3D_ATTR_DISPOFF);
    motionShift(board_configs[0].obj, 2, 3, 0, 0);
    MenuPrcSleep(0x96);
    MenuObject[0]->work[0] = 1;
    moveCameraWithMethod(MenuIntroBackOffAfterSillyGuys);
    MenuPrcSleep(0x5A);
    motionShiftIfChanged(silly_guys[4].obj, 1, 2, 0xF, 1);
    MenuMoveChar(silly_guys[4].obj, 1, sp2C, 0.0f, 3.0f, 0.0f, 1, 0);
    motionShiftIfChanged(silly_guys[4].obj, 1, 1, 0xF, 1);
    MenuPrcSleep(0x1E);
    HuAudSeqPlay(0x2D);
    var_r31 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[4].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(var_r31, 0x2F0000, -1, -1);
    motionShiftIfChanged(silly_guys[4].obj, 1, 1, 0xF, 1);
    DestroyWin(var_r31);
    motionShift(silly_guys[4].obj, 1, 3, 0xF, 1);
    MenuMoveChar(silly_guys[4].obj, 1, sp20, 5.0f, 6.0f, 10.0f, 1, 1);
    motionShift(silly_guys[4].obj, 1, 1, 0xF, 1);
    MenuPrcSleep(0x1E);
    motionShift(silly_guys[4].obj, 1, 5, 0xF, 1);
    moveCameraWithMethod(fn_1_14AB8);
    motionShift(board_configs[0].obj, 1, 0, 0, 0);
}

void fn_1_1053C(void)
{
    MenuPrcSleep(0x6E);
    WipeColorSet(0, 0, 0);
    WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_NORMAL, -1);
    while (WipeStatGet() != 0) {
        MenuPrcVSleep();
    }
    CharModelKill(-1);
    MGSeqKillAll();
    HuAudFXAllStop();
    HuAudSStreamAllStop();
    omOvlGotoEx(DLL_option, 1, 0, 0);
    while (1) {
        MenuPrcVSleep();
    }
}

void openOptionMenu(OMOBJ *arg0, MentBoardMenuConfig *arg1)
{
    MenuStartScene();
    fn_1_10234();
    fn_1_1053C();
}

void fn_1_10954(void)
{
    s32 var_r31;

    Vec sp2C = { 0.0f, 0.0f, 560.0f };
    Vec sp20 = { -120.0f, 0.0f, 670.0f };
    moveCameraWithMethod(MenuCameraMoveThroughIntro);
    MenuPrcSleep(0x5A);
    Hu3DModelAttrReset(silly_guys[2].obj->mdlId[1], HU3D_ATTR_DISPOFF);
    motionShift(board_configs[0].obj, 2, 3, 0, 0);
    MenuPrcSleep(0x96);
    MenuObject[0]->work[0] = 1;
    moveCameraWithMethod(MenuIntroBackOffAfterSillyGuys);
    MenuPrcSleep(0x5A);
    motionShiftIfChanged(silly_guys[2].obj, 1, 2, 0xF, 1);
    MenuMoveChar(silly_guys[2].obj, 1, sp2C, 0.0f, 3.0f, 0.0f, 1, 0);
    motionShiftIfChanged(silly_guys[2].obj, 1, 1, 0xF, 1);
    MenuPrcSleep(0x1E);
    HuAudSeqPlay(0x2F);
    var_r31 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[2].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(var_r31, 0x320000, -1, -1);
    motionShiftIfChanged(silly_guys[2].obj, 1, 1, 0xF, 1);
    DestroyWin(var_r31);
    motionShift(silly_guys[2].obj, 1, 3, 0xF, 1);
    MenuMoveChar(silly_guys[2].obj, 1, sp20, 5.0f, 6.0f, 10.0f, 1, 1);
    motionShift(silly_guys[2].obj, 1, 1, 0xF, 1);
    MenuPrcSleep(0x1E);
    motionShift(silly_guys[2].obj, 1, 5, 0xF, 1);
    moveCameraWithMethod(fn_1_14AB8);
    motionShift(board_configs[0].obj, 1, 0, 0, 0);
}

void fn_1_10C5C(void)
{
    MenuPrcSleep(0x6E);
    WipeColorSet(0, 0, 0);
    WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_NORMAL, -1);
    while (WipeStatGet() != 0) {
        MenuPrcVSleep();
    }
    CharModelKill(-1);
    MGSeqKillAll();
    HuAudFXAllStop();
    HuAudSStreamAllStop();
    omOvlGotoEx(DLL_present, 1, 0, 0);
    while (1) {
        MenuPrcVSleep();
    }
}

void openPresentMenu(OMOBJ *arg0, MentBoardMenuConfig *arg1)
{
    MenuStartScene();
    fn_1_10954();
    fn_1_10C5C();
}

void fn_1_11074(void)
{
    s32 var_r31;
    s32 var_r30;

    Vec sp14 = { 0.0f, 0.0f, 560.0f };
    moveCameraWithMethod(MenuCameraMoveThroughIntro);
    MenuPrcSleep(0x5A);
    Hu3DModelAttrReset(silly_guys[3].obj->mdlId[1], HU3D_ATTR_DISPOFF);
    motionShift(board_configs[0].obj, 2, 3, 0, 0);
    MenuPrcSleep(0x96);
    MenuObject[0]->work[0] = 1;
    moveCameraWithMethod(MenuIntroBackOffAfterSillyGuys);
    MenuPrcSleep(0x5A);
    motionShiftIfChanged(silly_guys[3].obj, 1, 2, 0xF, 1);
    MenuMoveChar(silly_guys[3].obj, 1, sp14, 0.0f, 3.0f, 0.0f, 1, 0);
    motionShiftIfChanged(silly_guys[3].obj, 1, 1, 0xF, 1);
    MenuPrcSleep(0x1E);
    var_r31 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[3].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(var_r31, 0x330000, -1, -1);
    motionShiftIfChanged(silly_guys[3].obj, 1, 1, 0xF, 1);
    DestroyWin(var_r31);
    var_r31 = OpenWinBottom(0, 0, 0);
    motionShiftIfChanged(silly_guys[3].obj, 1, 4, 0xF, 1);
    WinSetMessAndWait(var_r31, 0x330001, -1, 5);
    motionShiftIfChanged(silly_guys[3].obj, 1, 1, 0xF, 1);
    var_r30 = OpenConfirmDlgYesDef(0x1E0035, 3, 0);
    DestroyWin(var_r31);
    if (var_r30 == 0) {
        var_r31 = OpenWinBottom(0, 0, 0);
        motionShiftIfChanged(silly_guys[3].obj, 1, 4, 0xF, 1);
        WinSetMessAndWait(var_r31, 0x330002, -1, -1);
        motionShiftIfChanged(silly_guys[3].obj, 1, 1, 0xF, 1);
        DestroyWin(var_r31);
    }
}

void fn_1_11368(void)
{

    Vec sp14 = { -120.0f, 0.0f, 670.0f };
    motionShift(silly_guys[3].obj, 1, 3, 0xF, 1);
    MenuMoveChar(silly_guys[3].obj, 1, sp14, 5.0f, 6.0f, 10.0f, 1, 1);
    motionShift(silly_guys[3].obj, 1, 1, 0xF, 1);
    MenuPrcSleep(0x1E);
    motionShift(silly_guys[3].obj, 1, 5, 0xF, 1);
    moveCameraWithMethod(fn_1_14AB8);
    motionShift(board_configs[0].obj, 1, 0, 0, 0);
}

void fn_1_114A0(void)
{
    MenuPrcSleep(0x6E);
    WipeColorSet(0, 0, 0);
    WipeCreate(WIPE_MODE_OUT, WIPE_TYPE_NORMAL, -1);
    while (WipeStatGet() != 0) {
        MenuPrcVSleep();
    }
    CharModelKill(-1);
    MGSeqKillAll();
    HuAudFadeOut(1);
    omOvlReturnEx(1, 1);
    while (1) {
        MenuPrcVSleep();
    }
}

void openExtraRoomMenu(OMOBJ *arg0, MentBoardMenuConfig *arg1)
{
    s32 spC = 0xA;
    s32 sp8 = 0;
    MenuStartScene();
    fn_1_11074();
    fn_1_11368();
    fn_1_114A0();
}

void runMenu(void)
{
    OMOBJ *obj;

    obj = omAddObjEx(menuProc, 0x100, 0x10, 0x10, -1, NULL);
    menu_configuration_handler_holder.obj = obj;
    switch (gameConfigs[0]) {
        case 0:
        case 20:
            menu_configuration_handler_holder.obj_callback = (MentDllUnkFunc)openPartyMenu;
            break;
        case 1:
            menu_configuration_handler_holder.obj_callback = (MentDllUnkFunc)openStoryMenu;
            break;
        case 3:
            menu_configuration_handler_holder.obj_callback = (MentDllUnkFunc)openOptionMenu;
            break;
        case 4:
            menu_configuration_handler_holder.obj_callback = (MentDllUnkFunc)openPresentMenu;
            break;
        case 5:
            menu_configuration_handler_holder.obj_callback = (MentDllUnkFunc)openExtraRoomMenu;
            break;
        case 10:
            menu_configuration_handler_holder.obj_callback = (MentDllUnkFunc)openStoryContinuationMenu;
            break;
        case 11:
            menu_configuration_handler_holder.obj_callback = (MentDllUnkFunc)openStoryGnarlyMenu;
            break;
    }
    while (1) {
        MenuPrcVSleep();
        if (menu_configuration_handler_holder.obj_callback == NULL) {
            continue;
        }
        menu_configuration_handler_holder.obj_callback(obj, &menu_configuration_handler_holder);
    }
}

void oscillateChest(OMOBJ *obj, MentBoardMenuConfig *board_config) // Move's bowser's gnarly chest move up and down
{
    Hu3DData[obj->mdlId[4]].pos.y = SinOscillateClamped(80.0f, 90.0f, board_config->board_settings[0]++, 360.0f);
    if (board_config->board_settings[0] >= 0x168) {
        board_config->board_settings[0] = 0;
    }
}

void createAndOpenChest(OMOBJ *obj, s32 map)
{
    obj->mdlId[4] = Hu3DModelCreateFile(map + DATA_MAKE_NUM(DATADIR_MENT, 0xD));
    if (map != 5) {
        obj->mdlId[5] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x13));
    }
    else {
        obj->mdlId[5] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x14));
    }
    obj->mtnId[4] = Hu3DMotionIDGet(obj->mdlId[4]);
    obj->mtnId[5] = Hu3DMotionIDGet(obj->mdlId[5]);
    motionShiftIfChanged(obj, 4, 4, 0, 2);
    motionShiftIfChanged(obj, 5, 5, 0, 2);
    if (map != 5) {
        Hu3DModelPosSet(obj->mdlId[4], 0.0f, 80.0f, 460.0f);
        Hu3DModelHookSet(obj->mdlId[4], "partybox_fix2-effect_fook1", obj->mdlId[5]);
    }
    else {
        Hu3DModelPosSet(obj->mdlId[4], 1200.0f, 80.0f, 460.0f);
        Hu3DModelHookSet(obj->mdlId[4], "koopabox_fix-effect_fook2", obj->mdlId[5]);
    }
    Hu3DModelAttrSet(obj->mdlId[4], HU3D_ATTR_DISPOFF);
    Hu3DModelAttrSet(obj->mdlId[5], HU3D_ATTR_DISPOFF);
    Hu3DModelShadowSet(obj->mdlId[4]);
}

void MenuCreateBackdrop(OMOBJ *object)
{
    object->mdlId[1] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x61));
    object->mdlId[2] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x69));
    if (gameConfigs[0] < 0xA) {
        object->mdlId[3] = Hu3DModelCreateFile(gameConfigs[0] + DATA_MAKE_NUM(DATADIR_MENT, 0x63));
    }
    else {
        object->mdlId[3] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x64));
    }
    if ((gameConfigs[4] == 1) && ((gameConfigs[0] == 0) || (gameConfigs[0] == 0x14))) {
        object->mdlId[6] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x6C));
        Hu3DModelLayerSet(object->mdlId[6], 2);
        Hu3DModelPosSet(object->mdlId[6], 50.0f, 0.0f, -100.0f);
        object->mdlId[7] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x6D));
        Hu3DModelLayerSet(object->mdlId[7], 2);
        Hu3DModelPosSet(object->mdlId[7], 1150.0f, 0.0f, -100.0f);
    }
    object->mtnId[0] = Hu3DMotionIDGet(object->mdlId[1]);
    object->mtnId[1] = Hu3DMotionIDGet(object->mdlId[3]);
    object->mtnId[2] = Hu3DJointMotionFile(object->mdlId[2], DATA_MAKE_NUM(DATADIR_MENT, 0x6A));
    object->mtnId[3] = Hu3DJointMotionFile(object->mdlId[2], DATA_MAKE_NUM(DATADIR_MENT, 0x6B));
    if (gameConfigs[0] == 0xA) {
        motionShiftIfChanged(object, 2, 3, 0, 2);
    }
    else {
        motionShiftIfChanged(object, 2, 3, 0, 2);
    }
    motionShiftIfChanged(object, 3, 1, 0, 1);
    motionShiftIfChanged(object, 1, 0, 0, 2);
    Hu3DModelShadowMapObjSet(object->mdlId[1], "base_fix9-base");
}

void createBowserGnarlyMenu(OMOBJ *obj, s32 arg1, s32 arg2, s32 arg3)
{

    obj->mdlId[1] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x62));
    obj->mdlId[2] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x69));
    obj->mtnId[1] = Hu3DMotionIDGet(obj->mdlId[1]);
    obj->mtnId[2] = Hu3DJointMotionFile(obj->mdlId[2], DATA_MAKE_NUM(DATADIR_MENT, 0x6A));
    obj->mtnId[3] = Hu3DJointMotionFile(obj->mdlId[2], DATA_MAKE_NUM(DATADIR_MENT, 0x6B));
    if (gameConfigs[0] == 0xB) {
        motionShiftIfChanged(obj, 2, 3, 0, 2);
    }
    else {
        motionShiftIfChanged(obj, 2, 2, 0, 2);
        Hu3DModelAttrSet(obj->mdlId[2], HU3D_ATTR_DISPOFF);
    }
    motionShiftIfChanged(obj, 1, 1, 0, 1);
    Hu3DModelPosSet(obj->mdlId[1], 1200.0f, 0.0f, 0.0f);
    Hu3DModelPosSet(obj->mdlId[2], 1200.0f, 0.0f, 0.0f);
    if (gameConfigs[0] == 3) {
        obj->mdlId[4] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x12));
        obj->mdlId[5] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x14));
        obj->mtnId[4] = Hu3DMotionIDGet(obj->mdlId[4]);
        obj->mtnId[5] = Hu3DMotionIDGet(obj->mdlId[5]);
        motionShiftIfChanged(obj, 4, 4, 0, 2);
        motionShiftIfChanged(obj, 5, 5, 0, 2);
        Hu3DModelPosSet(obj->mdlId[4], 1200.0f, 80.0f, 460.0f);
        Hu3DModelHookSet(obj->mdlId[4], "koopabox_fix-effect_fook2", obj->mdlId[5]);
        Hu3DModelAttrSet(obj->mdlId[4], HU3D_ATTR_DISPOFF);
        Hu3DModelAttrSet(obj->mdlId[5], HU3D_ATTR_DISPOFF);
        Hu3DModelShadowSet(obj->mdlId[4]);
        Hu3DModelAttrReset(obj->mdlId[4], HU3D_ATTR_DISPOFF);
        Hu3DModelAttrReset(obj->mdlId[5], HU3D_ATTR_DISPOFF);
        board_configs[1].obj_callback = (MentDllUnkFunc)oscillateChest;
    }
    Hu3DModelShadowMapObjSet(obj->mdlId[1], "base_story-base");
    if (gameConfigs[0] == 0xB) {
        obj->mdlId[4] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x12));
        obj->mdlId[5] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x14));
        obj->mtnId[4] = Hu3DMotionIDGet(obj->mdlId[4]);
        obj->mtnId[5] = Hu3DMotionIDGet(obj->mdlId[5]);
        motionShiftIfChanged(obj, 4, 4, 0, 2);
        motionShiftIfChanged(obj, 5, 5, 0, 2);
        Hu3DModelPosSet(obj->mdlId[4], 1200.0f, 80.0f, 460.0f);
        Hu3DModelHookSet(obj->mdlId[4], "koopabox_fix-effect_fook2", obj->mdlId[5]);
        Hu3DModelAttrSet(obj->mdlId[4], HU3D_ATTR_DISPOFF);
        Hu3DModelAttrSet(obj->mdlId[5], HU3D_ATTR_DISPOFF);
        Hu3DModelShadowSet(obj->mdlId[4]);
    }
}

void createBoardMenu(void)
{
    OMOBJ *obj;
    s32 local_background_number;
    s32 mg_list;

    local_background_number = background_number;
    background_number = local_background_number + 1;
    obj = omAddObjEx(menuProc, 0x100, 0x10, 0x10, -1, NULL);
    board_configs[local_background_number].obj = obj;
    board_configs[local_background_number].obj_callback = NULL;
    if (local_background_number == 0) {
        MenuCreateBackdrop(obj);
        switch (gameConfigs[0]) {
            case 0:
                createChooseHumanCountGroupParty(&board_configs[local_background_number], 0, 0, 0);
                createChooseCharacterGroup(&board_configs[local_background_number], 0, 0, 0);
                createChosenCharactersOverlayGroup(&board_configs[local_background_number], 0, 0, 0);
                createChooseBoardGroup(&board_configs[local_background_number], 0, 0, 0);
                createChooseBoardSettingsGroup(&board_configs[local_background_number], 0, 0, 0);
                break;
            case 1:
                createChooseHumanCountGroupStory(&board_configs[local_background_number], 0, 0, 0);
                createChosenCharactersOverlayGroupStory(&board_configs[local_background_number], 0, 0, 0);
                createChooseBoardGroup(&board_configs[local_background_number], 0, 0, 0);
                break;
            case 10:
                if (GWSystem.mg_list == 3) {
                    GWSystem.mg_list = 0;
                }
                mg_list = GWSystem.mg_list;
                board_configs[0].board_settings[1] = mg_list;
                board_configs[0].board_settings[0] = GWPlayerCfg[0].diff;
                createChosenCharactersOverlayGroupStory(&board_configs[local_background_number], 0, 0, 0);
                createChooseBoardGroup(&board_configs[local_background_number], 0, 0, 0);
                break;
            case 5:
                createChooseHumanCountGroupParty(&board_configs[local_background_number], 0, 0, 0);
                createChooseCharacterGroup(&board_configs[local_background_number], 0, 0, 0);
                break;
        }
    }
    else {
        createBowserGnarlyMenu(obj, 0, 0, 0);
        if (gameConfigs[0] == 0xB) {
            createChosenCharactersOverlayGroupStory(board_configs, 0, 0, 0);
            createChooseBoardGroup(board_configs, 0, 0, 0);
        }
    }
    while (1) {
        MenuPrcVSleep();
        if (board_configs[local_background_number].obj_callback == NULL) {
            continue;
        }
        board_configs[local_background_number].obj_callback(obj, &board_configs[local_background_number]);
    }
}

void chooseBoardSillyGuyPos(OMOBJ *obj, MentDllUnkBss33ACStruct *silly_guy)
{
    float posZ;
    float rotY;

    posZ = 0.0f;
    rotY = 0.0f;
    posZ = Hu3DData[obj->mdlId[1]].pos.z;
    rotY = Hu3DData[obj->mdlId[1]].rot.y;
    if (silly_guy->unk_08 == 1) {
        if (posZ > 560.0f) {
            silly_guy->unk_08 = 2;
            motionShiftIfChanged(obj, 1, 1, 0xF, 1);
        }
        else {
            posZ += 5.0f;
            motionShiftIfChanged(obj, 1, 2, 0xF, 1);
            rotY = WeightedBlend(rotY, 0.0f, 10.0f);
        }
    }
    else if (silly_guy->unk_08 == 0) {
        if (posZ <= 460.0f) {
            silly_guy->unk_08 = 2;
            motionShiftIfChanged(obj, 1, 1, 0xF, 1);
        }
        else {
            posZ -= 5.0f;
            motionShiftIfChanged(obj, 1, 2, 0xF, 1);
            rotY = WeightedBlend(rotY, silly_guys_angle[silly_guy->idx], 10.0f);
        }
    }
    Hu3DData[obj->mdlId[1]].pos.z = posZ;
    Hu3DData[obj->mdlId[1]].rot.y = rotY;
}

void createSillyGuyModel(OMOBJ *object, s32 silly_guy_index)
{
    s32 characterModels[6] = {
        DATA_MAKE_NUM(DATADIR_MENT, 0x6E),
        DATA_MAKE_NUM(DATADIR_MENT, 0x75),
        DATA_MAKE_NUM(DATADIR_MENT, 0x7C),
        DATA_MAKE_NUM(DATADIR_MENT, 0x83),
        DATA_MAKE_NUM(DATADIR_MENT, 0x89),
        DATA_MAKE_NUM(DATADIR_MENT, 0x90),
    };
    s32 characterMotions[6][6] = {
        DATA_MAKE_NUM(DATADIR_MENT, 0x6F),
        DATA_MAKE_NUM(DATADIR_MENT, 0x70),
        DATA_MAKE_NUM(DATADIR_MENT, 0x71),
        DATA_MAKE_NUM(DATADIR_MENT, 0x72),
        DATA_MAKE_NUM(DATADIR_MENT, 0x73),
        DATA_MAKE_NUM(DATADIR_MENT, 0x74),
        DATA_MAKE_NUM(DATADIR_MENT, 0x76),
        DATA_MAKE_NUM(DATADIR_MENT, 0x77),
        DATA_MAKE_NUM(DATADIR_MENT, 0x78),
        DATA_MAKE_NUM(DATADIR_MENT, 0x79),
        DATA_MAKE_NUM(DATADIR_MENT, 0x7A),
        DATA_MAKE_NUM(DATADIR_MENT, 0x7B),
        DATA_MAKE_NUM(DATADIR_MENT, 0x7D),
        DATA_MAKE_NUM(DATADIR_MENT, 0x7E),
        DATA_MAKE_NUM(DATADIR_MENT, 0x7F),
        DATA_MAKE_NUM(DATADIR_MENT, 0x80),
        DATA_MAKE_NUM(DATADIR_MENT, 0x81),
        DATA_MAKE_NUM(DATADIR_MENT, 0x82),
        DATA_MAKE_NUM(DATADIR_MENT, 0x84),
        DATA_MAKE_NUM(DATADIR_MENT, 0x85),
        DATA_MAKE_NUM(DATADIR_MENT, 0x85),
        DATA_MAKE_NUM(DATADIR_MENT, 0x86),
        DATA_MAKE_NUM(DATADIR_MENT, 0x87),
        DATA_MAKE_NUM(DATADIR_MENT, 0x88),
        DATA_MAKE_NUM(DATADIR_MENT, 0x8A),
        DATA_MAKE_NUM(DATADIR_MENT, 0x8B),
        DATA_MAKE_NUM(DATADIR_MENT, 0x8C),
        DATA_MAKE_NUM(DATADIR_MENT, 0x8D),
        DATA_MAKE_NUM(DATADIR_MENT, 0x8E),
        DATA_MAKE_NUM(DATADIR_MENT, 0x8F),
        DATA_MAKE_NUM(DATADIR_MENT, 0x91),
        DATA_MAKE_NUM(DATADIR_MENT, 0x92),
        DATA_MAKE_NUM(DATADIR_MENT, 0x93),
        DATA_MAKE_NUM(DATADIR_MENT, 0x94),
        DATA_MAKE_NUM(DATADIR_MENT, 0x95),
        DATA_MAKE_NUM(DATADIR_MENT, 0x91),
    };
    object->mdlId[1] = Hu3DModelCreateFile(characterModels[silly_guy_index]);
    object->mtnId[1] = Hu3DJointMotionFile(object->mdlId[1], characterMotions[silly_guy_index][0]);
    object->mtnId[2] = Hu3DJointMotionFile(object->mdlId[1], characterMotions[silly_guy_index][1]);
    object->mtnId[3] = Hu3DJointMotionFile(object->mdlId[1], characterMotions[silly_guy_index][2]);
    object->mtnId[4] = Hu3DJointMotionFile(object->mdlId[1], characterMotions[silly_guy_index][3]);
    object->mtnId[5] = Hu3DJointMotionFile(object->mdlId[1], characterMotions[silly_guy_index][4]);
    object->mtnId[6] = Hu3DJointMotionFile(object->mdlId[1], characterMotions[silly_guy_index][5]);
    {
        s32 silly_guy_npcNo[6] = { 9, 14, 10, 11, 12, 13 };
        if (silly_guy_index != 3) {
            CharNpcDustSet(object->mdlId[1], object->mtnId[2], 0, silly_guy_npcNo[silly_guy_index]);
            CharNpcDustSet(object->mdlId[1], object->mtnId[3], 1, silly_guy_npcNo[silly_guy_index]);
        }
    }
    motionShiftIfChanged(object, 1, 1, 0, 1);
    Hu3DModelPosSet(object->mdlId[1], silly_guys_pos[silly_guy_index].x, silly_guys_pos[silly_guy_index].y, silly_guys_pos[silly_guy_index].z);
    Hu3DModelRotSet(object->mdlId[1], 0.0f, silly_guys_angle[silly_guy_index], 0.0f);
    if (gameConfigs[0] >= 2 && gameConfigs[0] < 0xA) {
        Hu3DModelPosSet(object->mdlId[1], silly_guys_pos[0].x, silly_guys_pos[0].y, silly_guys_pos[0].z);
        Hu3DModelRotSet(object->mdlId[1], 0.0f, 0.0f, 0.0f);
        if (gameConfigs[0] == 5) {
            Hu3DModelPosSet(object->mdlId[1], silly_guys_pos[0].x, 50.0f + silly_guys_pos[0].y, silly_guys_pos[0].z);
        }
    }
    if (gameConfigs[0] < 0xA) {
        Hu3DModelAttrSet(object->mdlId[1], HU3D_ATTR_DISPOFF);
    }
    Hu3DModelShadowSet(object->mdlId[1]);
    Hu3DModelLayerSet(object->mdlId[1], 2);
}

void createSillyGuy(void)
{
    OMOBJ *silly_guy_obj;
    s32 current_silly_guy_idx;

    current_silly_guy_idx = silly_guy_idx;
    silly_guy_idx = current_silly_guy_idx + 1;
    silly_guy_obj = omAddObjEx(menuProc, 0x100, 0x10, 0x10, -1, NULL);
    silly_guys[current_silly_guy_idx].obj = silly_guy_obj;
    silly_guys[current_silly_guy_idx].objCallback = NULL;
    silly_guys[current_silly_guy_idx].idx = current_silly_guy_idx;
    createSillyGuyModel(silly_guy_obj, current_silly_guy_idx);
    while (1) {
        MenuPrcVSleep();
        if (silly_guys[current_silly_guy_idx].objCallback != NULL) {
            silly_guys[current_silly_guy_idx].objCallback(silly_guy_obj, &silly_guys[current_silly_guy_idx]);
        }
        if (silly_guy_obj->work[0] != 1) {
            continue;
        }
        motionShiftTick(silly_guy_obj);
    }
}

void fn_1_13348(OMOBJ *arg0)
{
    arg0->mdlId[1] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x96));
    arg0->mtnId[1] = Hu3DJointMotionFile(arg0->mdlId[1], DATA_MAKE_NUM(DATADIR_MENT, 0x97));
    arg0->mtnId[2] = Hu3DJointMotionFile(arg0->mdlId[1], DATA_MAKE_NUM(DATADIR_MENT, 0x98));
    arg0->mtnId[3] = Hu3DJointMotionFile(arg0->mdlId[1], DATA_MAKE_NUM(DATADIR_MENT, 0x99));
    motionShiftIfChanged(arg0, 1, 3, 0, 1);
    Hu3DModelPosSet(arg0->mdlId[1], 1020.0f, 0.0f, 800.0f);
    Hu3DModelRotSet(arg0->mdlId[1], 0.0f, 60.0f, 0.0f);
    Hu3DModelShadowSet(arg0->mdlId[1]);
    Hu3DModelLayerSet(arg0->mdlId[1], 2);
}

void fn_1_134A8(void)
{
    OMOBJ *var_r31;

    var_r31 = omAddObjEx(menuProc, 0x100, 0x10, 0x10, -1, NULL);
    lbl_1_bss_3354.obj = var_r31;
    lbl_1_bss_3354.objCallback = NULL;
    fn_1_13348(var_r31);
    while (1) {
        MenuPrcVSleep();
        if (lbl_1_bss_3354.objCallback != NULL) {
            lbl_1_bss_3354.objCallback(var_r31, &lbl_1_bss_3354);
        }
        if (var_r31->work[0] != 1) {
            continue;
        }
        motionShiftTick(var_r31);
    }
}

void initPlayerData(OMOBJ *arg0, s32 player_index)
{
    MTPlayerConfig *player_ref;

    player_ref = &mtPlayerData[player_index];
    player_ref->character_highlighted = player_index;
    player_ref->group = 0;
    player_ref->iscom = player_ref->character_highlighted;
    if (player_ref->iscom > 0) {
        player_ref->iscom = 1;
    }
    player_ref->diff = 0;
    player_ref->character = player_ref->character_highlighted;
    player_ref->pad_idx = player_ref->character_highlighted;
}

void getPlayerData(OMOBJ *arg0, s32 player_index)
{
    MTPlayerConfig *player_ref;

    player_ref = &mtPlayerData[player_index];
    player_ref->character_highlighted = player_index;
    player_ref->group = GWPlayerCfg[player_ref->character_highlighted].group;
    player_ref->iscom = GWPlayerCfg[player_ref->character_highlighted].iscom;
    player_ref->diff = GWPlayerCfg[player_ref->character_highlighted].diff;
    player_ref->character = GWPlayerCfg[player_ref->character_highlighted].character;
    player_ref->pad_idx = GWPlayerCfg[player_ref->character_highlighted].pad_idx;
}

void createPlayerModel(OMOBJ *player_object, s32 player_index)
{
    MTPlayerConfig *player_ref;

    player_ref = &mtPlayerData[player_index];
    player_ref->character_highlighted = player_index;
    player_ref->group = GWPlayerCfg[player_ref->character_highlighted].group;
    player_ref->iscom = GWPlayerCfg[player_ref->character_highlighted].iscom;
    player_ref->diff = GWPlayerCfg[player_ref->character_highlighted].diff;
    player_ref->character = GWPlayerCfg[player_ref->character_highlighted].character;
    player_ref->pad_idx = GWPlayerCfg[player_ref->character_highlighted].pad_idx;
    player_object->mdlId[1] = CharModelCreate(player_ref->character, 1);
    player_object->mtnId[1] = CharMotionCreate(player_ref->character, DATA_MAKE_NUM(DATADIR_MARIOMOT, 0));
    player_object->mtnId[2] = Hu3DJointMotionFile(player_object->mdlId[1], player_ref->character + DATA_MAKE_NUM(DATADIR_MENT, 0x00));
    motionShiftIfChanged(player_object, 1, 2, 0, 1);
    Hu3DModelPosSet(player_object->mdlId[1], 1320.0f, 0.0f, 800.0f);
    Hu3DModelRotSet(player_object->mdlId[1], 0.0f, -60.0f, 0.0f);
    Hu3DModelShadowSet(player_object->mdlId[1]);
    Hu3DModelLayerSet(player_object->mdlId[1], 2);
}

void initPlayer(void)
{
    MTPlayerConfig *var_r28;
    OMOBJ *player_obj;
    s32 current_player_index;

    current_player_index = player_index;
    player_index = current_player_index + 1;
    player_obj = omAddObjEx(menuProc, 0x100, 0x10, 0x10, -1, NULL);
    mtPlayerData[current_player_index].obj = player_obj;
    mtPlayerData[current_player_index].proc_selection_handler = NULL;
    if (gameConfigs[0] <= 1) {
        initPlayerData(player_obj, current_player_index);
    }
    else if (gameConfigs[0] == 0xA) {
        getPlayerData(player_obj, 0);
    }
    else if (gameConfigs[0] == 0xB) {
        createPlayerModel(player_obj, 0);
    }
    else {
        initPlayerData(player_obj, current_player_index);
    }
    while (1) {
        MenuPrcVSleep();
        if (mtPlayerData[current_player_index].proc_selection_handler == NULL) {
            continue;
        }
        mtPlayerData[current_player_index].proc_selection_handler(player_obj, &mtPlayerData[current_player_index]);
    }
}

void MenuCameraFastIntro(void)
{
    MenuCamera *menu_camera_ref;

    menu_camera_ref = &menuCamera;
    menu_camera_ref->center.x = 0.0f;
    menu_camera_ref->center.y = 0.0f;
    menu_camera_ref->center.z = 0.0f;
    menu_camera_ref->rot.x = -10.0f;
    menu_camera_ref->rot.y = 0.0f;
    menu_camera_ref->rot.z = 0.0f;
    menu_camera_ref->zoom = 2900.0f;
}

void MenuCameraMoveThroughIntro(void)
{
    MenuCamera target_camera;
    MenuCamera *menu_camera_ref;

    menu_camera_ref = &menuCamera;
    target_camera.center.x = 0.0f;
    target_camera.center.y = 0.0f;
    target_camera.center.z = 0.0f;
    target_camera.rot.x = -10.0f;
    target_camera.rot.y = 0.0f;
    target_camera.rot.z = 0.0f;
    target_camera.zoom = 1150.0f;
    MenuCameraSinEaseFollow(menu_camera_ref, &target_camera, menu_camera_ref->frames++, 180.0f, 15.0f);
}

void MenuCameraSlowIntro(void)
{
    MenuCamera *menu_camera_ref;

    menu_camera_ref = &menuCamera;
    menu_camera_ref->center.x = 0.0f;
    menu_camera_ref->center.y = 215.0f;
    menu_camera_ref->center.z = 0.0f;
    menu_camera_ref->rot.x = 0.0f;
    menu_camera_ref->rot.y = 0.0f;
    menu_camera_ref->rot.z = 0.0f;
    menu_camera_ref->zoom = 1600.0f;
}

void MenuIntroBackOffAfterSillyGuys(void)
{
    MenuCamera target_camera;
    MenuCamera *menu_camera_ref;

    menu_camera_ref = &menuCamera;
    target_camera.center.x = 0.0f;
    target_camera.center.y = 215.0f;
    target_camera.center.z = 0.0f;
    target_camera.rot.x = 0.0f;
    target_camera.rot.y = 0.0f;
    target_camera.rot.z = 0.0f;
    target_camera.zoom = 1600.0f;
    MenuCameraSinEaseFollow(menu_camera_ref, &target_camera, menu_camera_ref->frames++, 60.0f, 10.0f);
}

void fn_1_14058(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    sp8.center.x = 0.0f;
    sp8.center.y = 215.0f;
    sp8.center.z = 0.0f;
    sp8.rot.x = 0.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 1600.0f;
    MenuCameraSinEaseFollow(var_r31, &sp8, var_r31->frames++, 10.0f, 5.0f);
}

void fn_1_14148(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    sp8.center.x = 1200.0f;
    sp8.center.y = 215.0f;
    sp8.center.z = 0.0f;
    sp8.rot.x = 0.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 1600.0f;
    MenuCameraSinEaseFollow(var_r31, &sp8, var_r31->frames++, 10.0f, 5.0f);
}

void fn_1_14238(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    sp8.center.x = 0.0f;
    sp8.center.y = 125.0f;
    sp8.center.z = 0.0f;
    sp8.rot.x = -5.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 1750.0f;
    MenuCameraSinEaseFollow(var_r31, &sp8, var_r31->frames++, 30.0f, 10.0f);
}

void fn_1_14328(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    sp8.center.x = 1200.0f;
    sp8.center.y = 125.0f;
    sp8.center.z = 0.0f;
    sp8.rot.x = -5.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 1750.0f;
    MenuCameraSinEaseFollow(var_r31, &sp8, var_r31->frames++, 30.0f, 10.0f);
}

void fn_1_14418(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;
    s32 var_r30;

    var_r30 = gameConfigs[2] / 5;
    var_r31 = &menuCamera;
    if (var_r31->frames == 0x32) {
        motionShift(board_configs[var_r30].obj, 4, 4, 0, 0);
        motionShift(board_configs[var_r30].obj, 5, 5, 0, 0);
        HuAudFXPlay(0x7D);
    }
    sp8.center.x = 0.0f;
    if (var_r30 == 1) {
        sp8.center.x = 1200.0f;
    }
    sp8.center.y = 100.0f;
    sp8.center.z = 460.0f;
    sp8.rot.x = 0.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 125.0f;
    MenuCameraSinEaseFollow(var_r31, &sp8, var_r31->frames, 180.0f, 10.0f);
    if (var_r31->frames++ >= 90) {
        if (var_r31->frames == 91) {
            if (var_r30 == 0) {
                lbl_1_bss_24[10] = HuAudFXPlay(0x22);
            }
            else {
                lbl_1_bss_24[10] = HuAudFXPlay(0x23);
            }
        }
        var_r31->rot.x = LerpClamped(var_r31->rot.x, -45.0f, var_r31->frames - 0x5A, 90.0f);
        Hu3DData[board_configs[var_r30].obj->mdlId[4]].rot.x = LerpClamped(0.0f, 45.0f, var_r31->frames - 0x5A, 90.0f);
    }
}

void fn_1_146D0(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    sp8.center.x = 0.0f;
    sp8.center.y = 215.0f;
    sp8.center.z = 0.0f;
    sp8.rot.x = 0.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 1600.0f;
    MenuCameraCosEaseFollow(var_r31, &sp8, var_r31->frames++, 30.0f, 10.0f);
}

void fn_1_147C0(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    sp8.center.x = 1200.0f;
    sp8.center.y = 215.0f;
    sp8.center.z = 0.0f;
    sp8.rot.x = 0.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 1600.0f;
    MenuCameraCosEaseFollow(var_r31, &sp8, var_r31->frames++, 30.0f, 10.0f);
}

void fn_1_148B0(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    sp8.center.x = 0.0f;
    sp8.center.y = 30.0f;
    sp8.center.z = 0.0f;
    sp8.rot.x = -5.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 1100.0f;
    MenuCameraSinEaseFollow(var_r31, &sp8, var_r31->frames++, 10.0f, 5.0f);
}

void fn_1_149A0(void)
{
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    var_r31->center.x = 0.0f;
    var_r31->center.y = 125.0f;
    var_r31->center.z = 0.0f;
    var_r31->rot.x = -5.0f;
    var_r31->rot.y = 0.0f;
    var_r31->rot.z = 0.0f;
    var_r31->zoom = 1750.0f;
}

void fn_1_14A2C(void)
{
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    var_r31->center.x = 1200.0f;
    var_r31->center.y = 125.0f;
    var_r31->center.z = 0.0f;
    var_r31->rot.x = -5.0f;
    var_r31->rot.y = 0.0f;
    var_r31->rot.z = 0.0f;
    var_r31->zoom = 1750.0f;
}

void fn_1_14AB8(void)
{
    MenuCamera sp8;
    MenuCamera *var_r31;

    var_r31 = &menuCamera;
    sp8.center.x = 0.0f;
    sp8.center.y = 125.0f;
    sp8.center.z = 460.0f;
    sp8.rot.x = 0.0f;
    sp8.rot.y = 0.0f;
    sp8.rot.z = 0.0f;
    sp8.zoom = 125.0f;
    MenuCameraCosEaseFollow(var_r31, &sp8, var_r31->frames++, 120.0f, 10.0f);
}

void hideChooseCharacters(void)
{
    s32 i;
    s32 choosecharacter_group;
    choosecharacter_group = board_configs[0].choosecharacter_group;
    for (i = 0; i < 0x35; i++) {
        HuSprAttrSet(choosecharacter_group, i, HUSPR_ATTR_DISPOFF);
    }
    for (i = 0; i < 8; i++) {
        HuSprBankSet(choosecharacter_group, i, 0);
        HuSprAttrReset(choosecharacter_group, i, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(choosecharacter_group, i + 8, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrReset(choosecharacter_group, 0x28, HUSPR_ATTR_DISPOFF);
    for (i = 0; i < 4; i++) {
        HuSprBankSet(choosecharacter_group, i + 0x10, 0);
        HuSprBankSet(choosecharacter_group, i + 0x14, 0);
        HuSprAttrReset(choosecharacter_group, i + 0x29, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(choosecharacter_group, i + 0x2D, HUSPR_ATTR_DISPOFF);
    }
    HuSprGrpPosSet(choosecharacter_group, 0.0f, -500.0f);
}

s32 characters_grid_pos[4][4] = {
    { 0xA8, 0x5A, 0x78, 0x33 },
    { 0xF8, 0xBE, 0x1CA, 0x33 },
    { 0x148, 0x5A, 0x78, 0xDE },
    { 0x198, 0xBE, 0x1CA, 0xDE },
};
s32 chosen_characters_overlay_pos[4][2] = {
    { 0x1B, 0x18 },
    { 0x49, 0x18 },
    { 0x77, 0x18 },
    { 0xA5, 0x18 },
};

#if VERSION_NTSC
float settings_pos[5][5] = {
    { 262.0f, 119.0f, 1.3f, 227.0f, 455.0f },
    { 314.0f, 168.0f, 0.75f, 265.0f, 416.0f },
    { 366.0f, 204.0f, 0.75f, 240.0f, 440.0f },
    { 418.0f, 240.0f, 0.75f, 300.0f, 381.0f },
    { -1.0f, 289.0f, 1.3f, 220.0f, 460.0f },
};
#else
float bowser_gnarly_settings_pos[5][5] = {
    { 262.0f, 119.0f, 1.3f, 227.0f, 455.0f },
    { 314.0f, 168.0f, 0.75f, 300.0f, 381.0f },
    { 366.0f, 204.0f, 0.75f, 240.0f, 440.0f },
    { 418.0f, 240.0f, 0.75f, 300.0f, 381.0f },
    { -1.0f, 289.0f, 1.3f, 220.0f, 460.0f },
};
#endif

s32 storySettingsPos[4][5] = {
    { 0xB4, 0x60, 0x87, 0, 0 },
    { 0xFC, 0xB8, 0xF6, 0xD4, 0x1D4 },
    { 0x144, 0x60, 0x11A, 0xE6, 0x1C2 },
    { 0x18C, 0xB8, 0, 0, 0 },
};
s32 chooseHumanCountPos[4][4] = {
    { 0xB1, 0x68, 0x78, 0x37 },
    { 0xFB, 0x68, 0x1CA, 0x37 },
    { 0x145, 0x68, 0x78, 0x8E },
    { 0x18F, 0x68, 0x1CA, 0x8E },
};

void pushDownCharacterSelection(void)
{
    float y;
    s32 i;
    s32 choosecharacter_group;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    for (i = 0; i < 4; i++) {
        if (mtPlayerData[i].iscom != 0) {
            HuSprAttrSet(choosecharacter_group, mtPlayerData[i].character_highlighted + 0x10, HUSPR_ATTR_DISPOFF);
            HuSprAttrSet(choosecharacter_group, mtPlayerData[i].character_highlighted + 0x14, HUSPR_ATTR_DISPOFF);
        }
        else {
            HuSprAttrReset(choosecharacter_group, mtPlayerData[i].character_highlighted + 0x10, HUSPR_ATTR_DISPOFF);
            HuSprAttrReset(choosecharacter_group, mtPlayerData[i].character_highlighted + 0x14, HUSPR_ATTR_DISPOFF);
        }
        HuSprPosSet(choosecharacter_group, mtPlayerData[i].character_highlighted + 0x10, characters_grid_pos[mtPlayerData[i].character % 4][0],
            characters_grid_pos[mtPlayerData[i].character / 4][1]);
        HuSprPosSet(choosecharacter_group, mtPlayerData[i].character_highlighted + 0x14, characters_grid_pos[mtPlayerData[i].character % 4][0],
            characters_grid_pos[mtPlayerData[i].character / 4][1]);
    }
    for (i = 0; i <= 60; i++) {
        MenuPrcVSleep();
        if (i <= 0x32) {
            y = SinEaseClamped(-500.0f, 10.0f, i, 50.0f);
        }
        else {
            y = CosEaseClamped(10.0f, 2.06f, i - 0x32, 10.0f);
        }
        HuSprGrpPosSet(board_configs[0].choosecharacter_group, 0.0f, y);
    }
}

void moveUpCharacterSelction(void)
{
    float y;
    s32 i;

    for (i = 0; i <= 0x3C; i++) {
        MenuPrcVSleep();
        if (i <= 0xA) {
            y = SinEaseClamped(2.06f, 10.0f, i, 10.0f);
        }
        else {
            y = CosEaseClamped(10.0f, -500.0f, i - 0xA, 50.0f);
        }
        HuSprGrpPosSet(board_configs[0].choosecharacter_group, 0.0f, y);
    }
}

void createChooseCharacterGroup(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3)
{
    s32 i;
    s32 choosecharacter_group;
    s32 sprite_idx;
    s32 j;
    s32 group_same;
    // TODO: Determine what sprite animation their refering to
    ANIMDATA *spr_animation_data1;
    ANIMDATA *spr_animation_data2;
    ANIMDATA *spr_animation_data3;
    ANIMDATA *spr_animation_data4;
    ANIMDATA *spr_animation_data5;

    choosecharacter_group = HuSprGrpCreate(0x35);
    game_config0->choosecharacter_group = choosecharacter_group;
    spr_animation_data1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x22));
    for (i = 0; i < 8; i++) {
        sprite_idx = HuSprCreate(HuSprAnimReadFile(i + DATA_MAKE_NUM(DATADIR_MENT, 0x1A)), 0x3E8, 0);
        HuSprGrpMemberSet(choosecharacter_group, i, sprite_idx);
        HuSprPosSet(choosecharacter_group, i, characters_grid_pos[i % 4][0], characters_grid_pos[i / 4][1]);
        sprite_idx = HuSprCreate(spr_animation_data1, 0x3F2, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 8, sprite_idx);
        HuSprTPLvlSet(choosecharacter_group, i + 8, 0.5f);
        HuSprPosSet(choosecharacter_group, i + 8, characters_grid_pos[i % 4][0] + 1, characters_grid_pos[i / 4][1] + 2);
    }
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(HuSprAnimReadFile(i + DATA_MAKE_NUM(DATADIR_MENT, 0x23)), 0, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x10, sprite_idx);
        sprite_idx = HuSprCreate(HuSprAnimReadFile(i + DATA_MAKE_NUM(DATADIR_MENT, 0x28)), 0xA, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x14, sprite_idx);
    }
    spr_animation_data1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x27));
    spr_animation_data2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x2C));
    spr_animation_data3 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x2D));
    spr_animation_data4 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x2F));
    spr_animation_data5 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x2E));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(spr_animation_data1, 0, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x18, sprite_idx);
        sprite_idx = HuSprCreate(spr_animation_data2, 0x1E, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x1C, sprite_idx);
        sprite_idx = HuSprCreate(spr_animation_data3, 0xA, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x20, sprite_idx);
        sprite_idx = HuSprCreate(spr_animation_data4, 0x14, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x24, sprite_idx);
        HuSprTPLvlSet(choosecharacter_group, i + 0x24, 0.5f);
        sprite_idx = HuSprCreate(spr_animation_data5, 0x14, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x31, sprite_idx);
    }
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x17)), 0x7D0, 0);
    HuSprGrpMemberSet(choosecharacter_group, 0x28, sprite_idx);
    HuSprTPLvlSet(choosecharacter_group, 0x28, 0.8f);
    sprPosSet_YPadded(choosecharacter_group, 0x28, 288.0f, 240.0f);
    spr_animation_data1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x18));
    spr_animation_data2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x19));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(spr_animation_data1, 0x7BC, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x29, sprite_idx);
        HuSprBankSet(choosecharacter_group, i + 0x29, i);
        sprPosSet_YPadded(choosecharacter_group, i + 0x29, characters_grid_pos[i][2], characters_grid_pos[i][3]);
        sprite_idx = HuSprCreate(spr_animation_data2, 0x7C6, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x2D, sprite_idx);
        HuSprBankSet(choosecharacter_group, i + 0x2D, i);
        HuSprTPLvlSet(choosecharacter_group, i + 0x2D, 0.5f);
        sprPosSet_YPadded(choosecharacter_group, i + 0x2D, characters_grid_pos[i][2] + 1, characters_grid_pos[i][3] + 6);
    }
    group_same = board_configs[0].choosecharacter_group;
    for (j = 0; j < 0x35; j++) {
        HuSprAttrSet(group_same, j, HUSPR_ATTR_DISPOFF);
    }
    for (j = 0; j < 8; j++) {
        HuSprBankSet(group_same, j, 0);
        HuSprAttrReset(group_same, j, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(group_same, j + 8, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrReset(group_same, 0x28, HUSPR_ATTR_DISPOFF);
    for (j = 0; j < 4; j++) {
        HuSprBankSet(group_same, j + 0x10, 0);
        HuSprBankSet(group_same, j + 0x14, 0);
        HuSprAttrReset(group_same, j + 0x29, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(group_same, j + 0x2D, HUSPR_ATTR_DISPOFF);
    }
    HuSprGrpPosSet(group_same, 0.0f, -500.0f);
}

// The place where 8 characters appear and we have to choose them
void character_selection_grid_init(void)
{
    float x;
    float y;
    s32 i;
    MTPlayerConfig *current_player;
    s32 group;
    s32 j;

    group = board_configs[0].choosecharacter_group;
    for (i = 0; i < 8; i++) {
        HuSprBankSet(group, i, 0);
    }
    for (i = 0; i < 4; i++) {
        HuSprBankSet(group, i + 0x10, 0);
        HuSprBankSet(group, i + 0x14, 0);
        HuSprBankSet(group, i + 0x18, 0);
        HuSprBankSet(group, i + 0x1C, 0);
        HuSprBankSet(group, i + 0x20, 0);
    }
    for (i = 0; i <= 0xA; i++) {
        MenuPrcVSleep();
        for (j = 0; j < 4; j++) {
            current_player = &mtPlayerData[j];
            x = SinEaseClamped(characters_grid_pos[current_player->character % 4][0], characters_grid_pos[current_player->character_highlighted % 4][0], i, 10.0f);
            y = SinEaseClamped(characters_grid_pos[current_player->character / 4][1], characters_grid_pos[current_player->character_highlighted / 4][1], i, 10.0f);
            // sets the initial position of a human player character selection square
            HuSprPosSet(group, current_player->character_highlighted + 0x10, x, y);
            HuSprPosSet(group, current_player->character_highlighted + 0x14, x, y);
        }
    }
    for (i = 0; i < 4; i++) {
        mtPlayerData[i].character = mtPlayerData[i].character_highlighted;
        mtPlayerData[i].unk_70[0] = 0;
    }
}

// Only when constant
void human_character_select(OMOBJ *arg0, MTPlayerConfig *player_config)
{
    float var_f31;
    float var_f30;
    s32 col;
    s32 row;
    s32 i;
    s32 col_change_dir;
    s32 row_change_dir;
    s32 choosecharacter_group;
    s32 original_col;
    s32 original_row;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    col_change_dir = 0;
    row_change_dir = 0;
    col = player_config->character % 4;
    original_col = col;
    row = player_config->character / 4;
    original_row = row;
    if (player_config->unk_70[0] == 0) {
        if ((HuPadBtnDown[player_config->pad_idx] & PAD_BUTTON_A) != 0) {
            player_config->unk_70[0] = 1;
            HuSprBankSet(choosecharacter_group, player_config->character, 1);
            HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x10, 1);
            HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x14, 1);
            HuAudFXPlay(player_config->character + 0x6D);
        }
        else {
            if ((HuPadStkX[player_config->pad_idx] >= 5) || ((HuPadBtn[player_config->pad_idx] & 2) != 0)) {
                col++;
                col_change_dir = 1;
                if (col > 3) {
                    col = 3;
                }
            }
            else if ((HuPadStkX[player_config->pad_idx] <= -5) || ((HuPadBtn[player_config->pad_idx] & 1) != 0)) {
                col--;
                col_change_dir = -1;
                if (col < 0) {
                    col = 0;
                }
            }
            if ((HuPadStkY[player_config->pad_idx] <= -5) || ((HuPadBtn[player_config->pad_idx] & 4) != 0)) {
                row++;
                row_change_dir = 1;
                if (row > 1) {
                    row = 1;
                }
            }
            else if ((HuPadStkY[player_config->pad_idx] >= 5) || ((HuPadBtn[player_config->pad_idx] & 8) != 0)) {
                row--;
                row_change_dir = -1;
                if (row < 0) {
                    row = 0;
                }
            }
        }
    }
    else if ((HuPadBtnDown[player_config->pad_idx] & PAD_BUTTON_B) != 0) {
        player_config->unk_70[0] = 0;
        HuSprBankSet(choosecharacter_group, player_config->character, 0);
        HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x10, 0);
        HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x14, 0);
        HuAudFXPlay(3);
    }
    if (player_config->character != (col + (row * 4))) {
        do {
            for (i = 0; i < 4; i++) {
                if ((i != player_config->character_highlighted) && (mtPlayerData[i].iscom == 0)
                    && ((col + (row * 4)) == mtPlayerData[i].character)) {
                    if ((col_change_dir == 0) || (row_change_dir == 0)) {
                        if (((col + col_change_dir) > 3) || ((col + col_change_dir) < 0)) {
                            col = original_col;
                        }
                        else {
                            col += col_change_dir;
                        }
                        if (((row + row_change_dir) > 1) || ((row + row_change_dir) < 0)) {
                            row = original_row;
                        }
                        else {
                            row += row_change_dir;
                        }
                    }
                    else if (((col + col_change_dir) <= 3) && ((col + col_change_dir) >= 0)) {
                        col += col_change_dir;
                    }
                    else if (((row + row_change_dir) <= 1) && ((row + row_change_dir) >= 0)) {
                        row += row_change_dir;
                    }
                    else {
                        col = original_col;
                        row = original_row;
                    }
                    break;
                }
            }
        } while (i != 4);
        if (player_config->character != (col + (row * 4))) {
            player_config->character = col + (row * 4);
            HuAudFXPlay(0);
            for (i = 0; i <= 0xA; i++) {
                MenuPrcVSleep();
                var_f31 = SinEaseClamped(characters_grid_pos[original_col][0], characters_grid_pos[col][0], i, 10.0f);
                var_f30 = SinEaseClamped(characters_grid_pos[original_row][1], characters_grid_pos[row][1], i, 10.0f);
                HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x10, var_f31, var_f30);
                HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x14, var_f31, var_f30);
            }
        }
    }
}

void fn_1_162A0(MTPlayerConfig *arg0)
{
    s32 choosecharacter_group;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    arg0->diff = 0;
    HuSprBankSet(choosecharacter_group, arg0->character_highlighted + 0x20, arg0->diff);
    HuSprPosSet(choosecharacter_group, arg0->character_highlighted + 0x20, characters_grid_pos[arg0->character % 4][0], characters_grid_pos[arg0->character / 4][1] + 0x23);
    HuSprPosSet(choosecharacter_group, arg0->character_highlighted + 0x24, characters_grid_pos[arg0->character % 4][0] + 1, characters_grid_pos[arg0->character / 4][1] + 0x25);
    HuSprAttrReset(choosecharacter_group, arg0->character_highlighted + 0x20, HUSPR_ATTR_DISPOFF);
    HuSprAttrReset(choosecharacter_group, arg0->character_highlighted + 0x24, HUSPR_ATTR_DISPOFF);
}

s32 fn_1_1648C(MTPlayerConfig *arg0)
{
    float var_f31;
    s32 var_r30;
    s32 choosecharacter_group;
    s32 var_r28;
    s32 var_r27;
    s32 var_r26;

    var_r28 = 0;
    choosecharacter_group = board_configs[0].choosecharacter_group;
    while (1) {
        MenuPrcVSleep();
        var_r28 = 0;
        var_r26 = arg0->diff;
        if (HuPadStkX[mtPlayerData->pad_idx] >= 5 || (HuPadBtn[mtPlayerData->pad_idx] & PAD_BUTTON_RIGHT)) {
            if (gameConfigs[5] == 1) {
                arg0->diff++;
                var_r28 = 1;
                if (arg0->diff > 3) {
                    arg0->diff = 0;
                }
            }
            else {
                arg0->diff++;
                var_r28 = 1;
                if (arg0->diff > 2) {
                    arg0->diff = 0;
                }
            }
        }
        else if (HuPadStkX[mtPlayerData->pad_idx] <= -5 || (HuPadBtn[mtPlayerData->pad_idx] & PAD_BUTTON_LEFT)) {
            if (gameConfigs[5] == 1) {
                arg0->diff--;
                var_r28 = -1;
                if (arg0->diff < 0) {
                    arg0->diff = 3;
                }
            }
            else {
                arg0->diff--;
                var_r28 = -1;
                if (arg0->diff < 0) {
                    arg0->diff = 2;
                }
            }
        }
        else if (HuPadBtnDown[0] & PAD_BUTTON_A) {
            var_r27 = 1;
            HuAudFXPlay(2);
            break;
        }
        else if (HuPadBtnDown[0] & PAD_BUTTON_B) {
            var_r27 = 0;
            HuAudFXPlay(3);
            break;
        }
        if (arg0->diff != var_r26) {
            HuAudFXPlay(0);
            for (var_r30 = 0; var_r30 <= 5; var_r30++) {
                MenuPrcVSleep();
                var_f31 = SinEaseClamped(characters_grid_pos[arg0->character % 4][0], characters_grid_pos[arg0->character % 4][0] + (var_r28 * 10), var_r30, 5);
                HuSprPosSet(choosecharacter_group, arg0->character_highlighted + 32, var_f31, characters_grid_pos[arg0->character / 4][1] + 35);
                HuSprPosSet(choosecharacter_group, arg0->character_highlighted + 36, 1 + var_f31, characters_grid_pos[arg0->character / 4][1] + 37);
            }
            HuSprBankSet(choosecharacter_group, arg0->character_highlighted + 32, arg0->diff);
            for (var_r30 = 0; var_r30 <= 5; var_r30++) {
                MenuPrcVSleep();
                var_f31 = SinEaseClamped(characters_grid_pos[arg0->character % 4][0] + (var_r28 * 10), characters_grid_pos[arg0->character % 4][0], var_r30, 5);
                HuSprPosSet(choosecharacter_group, arg0->character_highlighted + 32, var_f31, characters_grid_pos[arg0->character / 4][1] + 35);
                HuSprPosSet(choosecharacter_group, arg0->character_highlighted + 36, 1 + var_f31, characters_grid_pos[arg0->character / 4][1] + 37);
            }
        }
    }
    if (var_r27 == 1) {
        HuSprPosSet(choosecharacter_group, arg0->character_highlighted + 49, characters_grid_pos[arg0->character % 4][0], characters_grid_pos[arg0->character / 4][1] + 35);
        HuSprBankSet(choosecharacter_group, arg0->character_highlighted + 49, arg0->diff);
        HuSprAttrReset(choosecharacter_group, arg0->character_highlighted + 49, HUSPR_ATTR_DISPOFF);
        HuSprAttrSet(choosecharacter_group, arg0->character_highlighted + 32, HUSPR_ATTR_DISPOFF);
    }

    return var_r27;
}

void computer_character_select(OMOBJ *arg0, MTPlayerConfig *player_config)
{
    s32 sp8;
    float var_f31;
    float var_f30;
    s32 col;
    s32 row;
    s32 i;
    s32 choosecharacter_group;
    s32 col_change_dir;
    s32 row_change_dir;
    s32 original_col;
    s32 original_row;


    choosecharacter_group = board_configs[0].choosecharacter_group;
    col_change_dir = 0;
    row_change_dir = 0;
    sp8 = player_config->diff;
    original_col = col = player_config->character % 4;
    row = player_config->character / 4;
    original_row = row;
    if (player_config->unk_70[0] == 0) {
        if ((HuPadBtnDown[mtPlayerData->pad_idx] & PAD_BUTTON_A) != 0) {
            player_config->unk_70[0] = 1;
            HuSprBankSet(choosecharacter_group, player_config->character, 1);
            HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x18, 1);
            HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x1C, 1);
            HuAudFXPlay(player_config->character + 0x6D);
            fn_1_162A0(player_config);
        }
        else {
            if ((HuPadStkX[mtPlayerData->pad_idx] >= 5) || ((HuPadBtn[mtPlayerData->pad_idx] & 2) != 0)) {
                col++;
                col_change_dir = 1;
                if (col > 3) {
                    col = 3;
                }
            }
            else if ((HuPadStkX[mtPlayerData->pad_idx] <= -5) || ((HuPadBtn[mtPlayerData->pad_idx] & 1) != 0)) {
                col--;
                col_change_dir = -1;
                if (col < 0) {
                    col = 0;
                }
            }
            if ((HuPadStkY[mtPlayerData->pad_idx] <= -5) || ((HuPadBtn[mtPlayerData->pad_idx] & 4) != 0)) {
                row++;
                row_change_dir = 1;
                if (row > 1) {
                    row = 1;
                }
            }
            else if ((HuPadStkY[mtPlayerData->pad_idx] >= 5) || ((HuPadBtn[mtPlayerData->pad_idx] & 8) != 0)) {
                row--;
                row_change_dir = -1;
                if (row < 0) {
                    row = 0;
                }
            }
        }
    }
    else if ((HuPadBtnDown[mtPlayerData->pad_idx] & PAD_BUTTON_B) != 0) {
        player_config->unk_70[0] = player_config->unk_70[2] = 0;
        HuSprBankSet(choosecharacter_group, player_config->character, 0);
        HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x18, 0);
        HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x1C, 0);
        HuSprAttrSet(choosecharacter_group, player_config->character_highlighted + 0x20, 4);
        HuSprAttrSet(choosecharacter_group, player_config->character_highlighted + 0x24, 4);
        HuAudFXPlay(3);
    }
    if (player_config->character != (col + (row * 4))) {
        do {
            for (i = 0; i < 4; i++) {
                if ((i != player_config->character_highlighted) && (mtPlayerData[i].unk_70[0] == 1)
                    && ((col + (row * 4)) == mtPlayerData[i].character)) {
                    if ((col_change_dir == 0) || (row_change_dir == 0)) {
                        if (((col + col_change_dir) > 3) || ((col + col_change_dir) < 0)) {
                            col = original_col;
                        }
                        else {
                            col += col_change_dir;
                        }
                        if (((row + row_change_dir) > 1) || ((row + row_change_dir) < 0)) {
                            row = original_row;
                        }
                        else {
                            row += row_change_dir;
                        }
                    }
                    else if (((col + col_change_dir) <= 3) && ((col + col_change_dir) >= 0)) {
                        col += col_change_dir;
                    }
                    else if (((row + row_change_dir) <= 1) && ((row + row_change_dir) >= 0)) {
                        row += row_change_dir;
                    }
                    else {
                        col = original_col;
                        row = original_row;
                    }
                    break;
                }
            }
        } while (i != 4);
        if (player_config->character != (col + (row * 4))) {
            player_config->character = col + (row * 4);
            HuAudFXPlay(0);
            for (i = 0; i <= 0xA; i++) {
                MenuPrcVSleep();
                var_f31 = SinEaseClamped(characters_grid_pos[original_col][0], characters_grid_pos[col][0], i, 10.0f);
                var_f30 = SinEaseClamped(characters_grid_pos[original_row][1], characters_grid_pos[row][1], i, 10.0f);
                HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x18, var_f31, var_f30);
                HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x1C, var_f31, var_f30);
                HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x20, var_f31, 35.0f + var_f30);
                HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x24, 1.0f + var_f31, 37.0f + var_f30);
            }
        }
    }
    (void)col;
}

// Invoked when drawing a computer symbol during character selection I think?
void computer_character_selection_draw(MTPlayerConfig *player_config)
{
    s32 choosecharacter_group;
    s32 i;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    do {
        for (i = 0; i < 4; i++) {
            if ((i != player_config->character_highlighted) && (mtPlayerData[i].unk_70[0] == 1) && (player_config->character == mtPlayerData[i].character)) {
                player_config->character++;
                break;
            }
        }
    } while (i != 4);
    player_config->diff = 0;
    HuSprBankSet(choosecharacter_group, player_config->character, 0);
    HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x18, 0);
    HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x1C, 0);
    HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x20, player_config->diff);
    HuSprBankSet(choosecharacter_group, player_config->character_highlighted + 0x31, player_config->diff);
    HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x18, characters_grid_pos[player_config->character % 4][0], characters_grid_pos[player_config->character / 4][1]);
    HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x1C, characters_grid_pos[player_config->character % 4][0], characters_grid_pos[player_config->character / 4][1]);
    HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x20, characters_grid_pos[player_config->character % 4][0], characters_grid_pos[player_config->character / 4][1] + 0x23);
    HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x24, characters_grid_pos[player_config->character % 4][0] + 1, characters_grid_pos[player_config->character / 4][1] + 0x25);
    HuSprPosSet(choosecharacter_group, player_config->character_highlighted + 0x31, characters_grid_pos[player_config->character % 4][0] + 1, characters_grid_pos[player_config->character / 4][1] + 0x25);
    player_config->unk_70[2] = 0;
    HuSprAttrReset(choosecharacter_group, player_config->character_highlighted + 0x18, HUSPR_ATTR_DISPOFF);
    HuSprAttrReset(choosecharacter_group, player_config->character_highlighted + 0x1C, HUSPR_ATTR_DISPOFF);
    HuSprAttrSet(choosecharacter_group, player_config->character_highlighted + 0x20, HUSPR_ATTR_DISPOFF);
    HuSprAttrSet(choosecharacter_group, player_config->character_highlighted + 0x24, HUSPR_ATTR_DISPOFF);
    HuSprAttrSet(choosecharacter_group, player_config->character_highlighted + 0x31, HUSPR_ATTR_DISPOFF);
}

void removeComputerHighlight(MTPlayerConfig *player)
{
    s32 choosecharacter_group;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    HuSprBankSet(choosecharacter_group, player->character, 0);
    HuSprAttrSet(choosecharacter_group, player->character_highlighted + 0x18, 4);
    HuSprAttrSet(choosecharacter_group, player->character_highlighted + 0x1C, 4);
    HuSprAttrSet(choosecharacter_group, player->character_highlighted + 0x20, 4);
    HuSprAttrSet(choosecharacter_group, player->character_highlighted + 0x24, 4);
    HuSprAttrSet(choosecharacter_group, player->character_highlighted + 0x31, 4);
}

void resetChosenCharactersOverlay(void)
{
    MTPlayerConfig *player;
    s32 i;
    s32 chosen_characters_overlay_group;

    chosen_characters_overlay_group = board_configs[0].chosen_characters_overlay_group;
    for (i = 0; i < 0x11; i++) {
        HuSprAttrSet(chosen_characters_overlay_group, i, HUSPR_ATTR_DISPOFF);
    }
    for (i = 0; i < 4; i++) {
        player = &mtPlayerData[i];
        HuSprPosSet(chosen_characters_overlay_group, player->character, chosen_characters_overlay_pos[player->character_highlighted][0], chosen_characters_overlay_pos[player->character_highlighted][1]);
        HuSprAttrReset(chosen_characters_overlay_group, player->character, HUSPR_ATTR_DISPOFF);
        if (player->iscom == 0) {
            HuSprBankSet(chosen_characters_overlay_group, player->character_highlighted + 8, player->character_highlighted);
            HuSprPosSet(chosen_characters_overlay_group, player->character_highlighted + 8, chosen_characters_overlay_pos[player->character_highlighted][0], chosen_characters_overlay_pos[player->character_highlighted][1] + 0x19);
            HuSprAttrReset(chosen_characters_overlay_group, player->character_highlighted + 8, HUSPR_ATTR_DISPOFF);
        }
        else {
            HuSprBankSet(chosen_characters_overlay_group, player->character_highlighted + 0xC, player->diff);
            HuSprPosSet(chosen_characters_overlay_group, player->character_highlighted + 0xC, chosen_characters_overlay_pos[player->character_highlighted][0], chosen_characters_overlay_pos[player->character_highlighted][1] + 0x19);
            HuSprAttrReset(chosen_characters_overlay_group, player->character_highlighted + 0xC, HUSPR_ATTR_DISPOFF);
        }
    }
    HuSprAttrReset(chosen_characters_overlay_group, 0x10, HUSPR_ATTR_DISPOFF);
    HuSprGrpPosSet(chosen_characters_overlay_group, 16.0f, -500.0f);
}

void createChosenCharactersOverlayGroup(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3)
{
    MTPlayerConfig *player_ref;
    s32 i;
    s32 group_same;
    s32 j;
    s32 sprite_idx;
    s32 group;
    // TODO: Determine what sprite animations are being refered to
    ANIMDATA *sprite_animation1;
    ANIMDATA *sprite_animation2;

    group = HuSprGrpCreate(0x11);
    game_config0->chosen_characters_overlay_group = group;
    for (i = 0; i < 8; i++) {
        sprite_idx = HuSprCreate(HuSprAnimReadFile(i + DATA_MAKE_NUM(DATADIR_MENT, 0x31)), 0x3E8, 0);
        HuSprGrpMemberSet(group, i, sprite_idx);
    }
    sprite_animation1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3A));
    sprite_animation2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3B));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(sprite_animation1, 0x3DE, 0);
        HuSprGrpMemberSet(group, i + 8, sprite_idx);
        sprite_idx = HuSprCreate(sprite_animation2, 0x3DE, 0);
        HuSprGrpMemberSet(group, i + 0xC, sprite_idx);
    }
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x30)), 0x7D0, 0);
    HuSprGrpMemberSet(group, 0x10, sprite_idx);
    HuSprTPLvlSet(group, 0x10, 0.8f);
    HuSprPosSet(group, 0x10, 96.0f, 32.0f);
    group_same = board_configs[0].chosen_characters_overlay_group;
    for (j = 0; j < 0x11; j++) {
        HuSprAttrSet(group_same, j, HUSPR_ATTR_DISPOFF);
    }
    for (j = 0; j < 4; j++) {
        player_ref = &mtPlayerData[j];
        HuSprPosSet(group_same, player_ref->character, chosen_characters_overlay_pos[player_ref->character_highlighted][0], chosen_characters_overlay_pos[player_ref->character_highlighted][1]);
        HuSprAttrReset(group_same, player_ref->character, HUSPR_ATTR_DISPOFF);
        if (player_ref->iscom == 0) {
            HuSprBankSet(group_same, player_ref->character_highlighted + 8, player_ref->character_highlighted);
            HuSprPosSet(group_same, player_ref->character_highlighted + 8, chosen_characters_overlay_pos[player_ref->character_highlighted][0], chosen_characters_overlay_pos[player_ref->character_highlighted][1] + 0x19);
            HuSprAttrReset(group_same, player_ref->character_highlighted + 8, HUSPR_ATTR_DISPOFF);
        }
        else {
            HuSprBankSet(group_same, player_ref->character_highlighted + 0xC, player_ref->diff);
            HuSprPosSet(group_same, player_ref->character_highlighted + 0xC, chosen_characters_overlay_pos[player_ref->character_highlighted][0], chosen_characters_overlay_pos[player_ref->character_highlighted][1] + 0x19);
            HuSprAttrReset(group_same, player_ref->character_highlighted + 0xC, HUSPR_ATTR_DISPOFF);
        }
    }
    HuSprAttrReset(group_same, 0x10, HUSPR_ATTR_DISPOFF);
    HuSprGrpPosSet(group_same, 16.0f, -500.0f);
}

void pushDownChosenCharactersOverlay(void)
{
    float y;
    MTPlayerConfig *player;
    s32 chosen_characters_overlay_group;
    s32 i;
    s32 progress;

    chosen_characters_overlay_group = board_configs[0].chosen_characters_overlay_group;
    for (i = 0; i < 0x11; i++) {
        HuSprAttrSet(chosen_characters_overlay_group, i, HUSPR_ATTR_DISPOFF);
    }
    for (i = 0; i < 4; i++) {
        player = &mtPlayerData[i];
        HuSprPosSet(chosen_characters_overlay_group, player->character, chosen_characters_overlay_pos[player->character_highlighted][0], chosen_characters_overlay_pos[player->character_highlighted][1]);
        HuSprAttrReset(chosen_characters_overlay_group, player->character, HUSPR_ATTR_DISPOFF);
        if (player->iscom == 0) {
            HuSprBankSet(chosen_characters_overlay_group, player->character_highlighted + 8, player->character_highlighted);
            HuSprPosSet(chosen_characters_overlay_group, player->character_highlighted + 8, chosen_characters_overlay_pos[player->character_highlighted][0], chosen_characters_overlay_pos[player->character_highlighted][1] + 0x19);
            HuSprAttrReset(chosen_characters_overlay_group, player->character_highlighted + 8, HUSPR_ATTR_DISPOFF);
        }
        else {
            HuSprBankSet(chosen_characters_overlay_group, player->character_highlighted + 0xC, player->diff);
            HuSprPosSet(chosen_characters_overlay_group, player->character_highlighted + 0xC, chosen_characters_overlay_pos[player->character_highlighted][0], chosen_characters_overlay_pos[player->character_highlighted][1] + 0x19);
            HuSprAttrReset(chosen_characters_overlay_group, player->character_highlighted + 0xC, HUSPR_ATTR_DISPOFF);
        }
    }
    HuSprAttrReset(chosen_characters_overlay_group, 0x10, HUSPR_ATTR_DISPOFF);
    HuSprGrpPosSet(chosen_characters_overlay_group, 16.0f, -500.0f);
    for (progress = 0; progress <= 0x3C; progress++) {
        MenuPrcVSleep();
        if (progress <= 0x32) {
            y = SinEaseClamped(-500.0f, 50.0f, progress, 50.0f);
        }
        else {
            y = CosEaseClamped(50.0f, 40.0f, progress - 0x32, 10.0f);
        }
        HuSprGrpPosSet(board_configs[0].chosen_characters_overlay_group, 16.0f, y);
    }
}

void pushUpChosenCharactersOverlay(void)
{
    float var_f31;
    s32 var_r31;

    board_configs[0].handicaps[0] = -1;
    board_configs[0].obj_callback = NULL;
    for (var_r31 = 0; var_r31 <= 0x4B; var_r31++) {
        MenuPrcVSleep();
        if (var_r31 <= 0xA) {
            var_f31 = SinEaseClamped(40.0f, 50.0f, var_r31, 10.0f);
        }
        else {
            var_f31 = CosEaseClamped(50.0f, -500.0f, var_r31 - 0xA, 50.0f);
        }
        HuSprGrpPosSet(board_configs[0].chosen_characters_overlay_group, 16.0f, var_f31);
        if (var_r31 >= 0xF) {
            var_f31 = CosEaseClamped(210.0f, -500.0f, var_r31 - 0xF, 60.0f);
            HuSprGrpData[board_configs[0].chooseboard_group].pos.y = (s32)WeightedBlend(HuSprGrpData[board_configs[0].chooseboard_group].pos.y, var_f31, 10.0f);
        }
    }
}

void resetBoardOverviews(void)
{
    s32 i;
    s32 chooseboard_group;

    chooseboard_group = board_configs[0].chooseboard_group;
    for (i = 0; i < 7; i++) {
        HuSprAttrSet(chooseboard_group, i, 4);
    }
}

void createChooseBoardGroup(MentBoardMenuConfig *game_config, s32 arg1, s32 arg2, s32 arg3)
{
    s32 group;
    s32 sprite_idx;
    s32 i;
    s32 group_same;

    group = HuSprGrpCreate(7);
    game_config->chooseboard_group = group;
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3C)), 0x7DA, 0);
    HuSprGrpMemberSet(group, 0, sprite_idx);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3D)), 0x7C6, 0);
    HuSprGrpMemberSet(group, 1, sprite_idx);
    HuSprPosSet(group, 1, 0.0f, -80.0f);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3E)), 0x7D0, 0);
    HuSprGrpMemberSet(group, 2, sprite_idx);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3F)), 0x7D0, 0);
    HuSprGrpMemberSet(group, 3, sprite_idx);
    HuSprPosSet(group, 3, 0.0f, -80.0f);
    if (gameConfigs[0] != 0) {
        sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x5A)), 0x7BC, 0);
        HuSprGrpMemberSet(group, 4, sprite_idx);
        HuSprPosSet(group, 4, -140.0f, -86.0f);
        sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x5B)), 0x7BC, 0);
        HuSprGrpMemberSet(group, 5, sprite_idx);
        sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x5C)), 0x7D0, 0);
        HuSprGrpMemberSet(group, 6, sprite_idx);
        HuSprScaleSet(group, 6, 1.0f, 0.875f);
        HuSprTPLvlSet(group, 6, 0.5f);
    }
    HuSprExecLayerSet(0x40, 1);
    HuSprGrpDrawNoSet(group, 0x40);
    group_same = board_configs[0].chooseboard_group;
    for (i = 0; i < 7; i++) {
        HuSprAttrSet(group_same, i, 4);
    }
}

void fn_1_18A54(s32 arg0)
{
    float var_f31;
    float var_f30;
    s32 var_r31;

    var_r31 = board_configs[0].chooseboard_group;
    if (arg0 != 5) {
        HuSprBankSet(var_r31, 0, arg0);
        HuSprBankSet(var_r31, 1, arg0);
        HuSprAttrSet(var_r31, 2, HUSPR_ATTR_DISPOFF);
        HuSprAttrSet(var_r31, 3, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(var_r31, 0, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(var_r31, 1, HUSPR_ATTR_DISPOFF);
        if (gameConfigs[0] != 0) {
            if ((arg0 == 0 && _CheckFlag(FLAG_ID_MAKE(0, 2)) != 0) || (arg0 == 1 && _CheckFlag(FLAG_ID_MAKE(0, 3)) != 0)
                || (arg0 == 2 && _CheckFlag(FLAG_ID_MAKE(0, 4)) != 0) || (arg0 == 3 && (_CheckFlag(FLAG_ID_MAKE(0, 5)) != 0))
                || (arg0 == 4 && (_CheckFlag(FLAG_ID_MAKE(0, 6)) != 0))) {
                HuSprAttrReset(var_r31, 4, HUSPR_ATTR_DISPOFF);
                HuSprAttrReset(var_r31, 5, HUSPR_ATTR_DISPOFF);
                HuSprTPLvlSet(var_r31, 5, 0.8f);
                HuSprAttrReset(var_r31, 6, HUSPR_ATTR_DISPOFF);
                HuSprTPLvlSet(var_r31, 6, 0.5f);
            }
            else {
                HuSprAttrSet(var_r31, 4, 4);
                HuSprAttrSet(var_r31, 5, 4);
                HuSprAttrSet(var_r31, 6, 4);
            }
        }
    }
    else {
        HuSprAttrSet(var_r31, 0, HUSPR_ATTR_DISPOFF);
        HuSprAttrSet(var_r31, 1, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(var_r31, 2, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(var_r31, 3, HUSPR_ATTR_DISPOFF);
    }
    board_configs[0].handicaps[0] = arg0;
    board_configs[0].board_settings[1] = (rand8() % 10) * 0x24;
    HuSprGrpScaleSet(var_r31, 0.0f, 0.0f);
    for (arg0 = 0; arg0 <= 0xF; arg0++) {
        MenuPrcVSleep();
        if (arg0 <= 0xA) {
            var_f31 = SinEaseClamped(0.0f, 1.0f, arg0, 10.0f);
            var_f30 = 0.01f;
        }
        else {
            var_f31 = 1.0f;
            var_f30 = SinEaseClamped(0.01f, 1.0f, arg0 - 0xA, 5.0f);
        }
        HuSprGrpScaleSet(var_r31, var_f31, var_f30);
    }
}

void RescaleChooseBoardGrp(void)
{
    float x;
    float y;
    s32 progress;
    s32 i;
    s32 chooseboard_group;
    s32 chooseboard_group_same;

    chooseboard_group = board_configs[0].chooseboard_group;
    for (progress = 0; progress <= 0xF; progress++) {
        MenuPrcVSleep();
        if (progress <= 0xA) {
            x = 1.0f;
            y = SinEaseClamped(1.0f, 0.01f, progress, 10.0f);
        }
        else {
            x = SinEaseClamped(1.0f, 0.0f, progress - 0xA, 5.0f);
            y = 0.01f;
        }
        HuSprGrpScaleSet(chooseboard_group, x, y);
    }
    chooseboard_group_same = board_configs[0].chooseboard_group;
    for (i = 0; i < 7; i++) {
        HuSprAttrSet(chooseboard_group_same, i, 4);
    }
    board_configs[0].handicaps[0] = -1;
    board_configs[0].obj_callback = NULL;
}

void fn_1_18F74(OMOBJ *arg0, MentBoardMenuConfig *arg1)
{
    Vec sp14;
    Vec sp8;
    s32 var_r31;

    var_r31 = board_configs[0].chooseboard_group;
    if (board_configs[0].handicaps[0] != -1) {
        sp14.x = silly_guys_pos[board_configs[0].handicaps[0]].x;
        sp14.y = 0.0f;
        sp14.z = silly_guys_pos[board_configs[0].handicaps[0]].z;
        Hu3D3Dto2D(&sp14, 1, &sp8);
        sp8.y = SinOscillateClamped(210.0f, 220.0f, board_configs[0].board_settings[1]++, 360.0f);
        if (board_configs[0].board_settings[1] >= 0x168) {
            board_configs[0].board_settings[1] = 0;
        }
        HuSprGrpPosSet(var_r31, sp8.x, (s32)sp8.y);
    }
}

#if VERSION_PAL

#define POSX1 353
#define POSX2 325

#else

#define POSX1 321
#define POSX2 295

#endif

void hideBowserGnarlyBoardSettings(void)
{
    s32 i;
    s32 boardsettings_group;

    boardsettings_group = board_configs[0].boardsettings_group;
    for (i = 0; i < 0x5B; i++) {
        HuSprAttrSet(boardsettings_group, i, HUSPR_ATTR_DISPOFF);
    }
    if (gameConfigs[2] != 5) {
        HuSprBankSet(boardsettings_group, 0, gameConfigs[2]);
        HuSprAttrReset(boardsettings_group, 0, HUSPR_ATTR_DISPOFF);
    }
    else {
        HuSprAttrReset(boardsettings_group, 1, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrReset(boardsettings_group, 2, HUSPR_ATTR_DISPOFF);
    HuSprPosSet(boardsettings_group, 2, 288.0f, settings_pos[0][1]);
    HuSprScaleSet(boardsettings_group, 2, 1.0f, settings_pos[0][2]);
    for (i = 0; i < 2; i++) {
        HuSprAttrReset(boardsettings_group, i + 3, HUSPR_ATTR_DISPOFF);
        HuSprPosSet(boardsettings_group, i + 3, settings_pos[0][i + 3], settings_pos[0][1]);
    }
    for (i = 0; i < 4; i++) {
        HuSprAttrReset(boardsettings_group, mtPlayerData[i].character + 7, HUSPR_ATTR_DISPOFF);
        HuSprPosSet(boardsettings_group, mtPlayerData[i].character + 7, settings_pos[mtPlayerData[i].character_highlighted][0], settings_pos[0][1]);
        HuSprAttrReset(boardsettings_group, mtPlayerData[i].character_highlighted + 0xF, HUSPR_ATTR_DISPOFF);
        HuSprPosSet(
            boardsettings_group, mtPlayerData[i].character_highlighted + 0xF, 1.0f + settings_pos[mtPlayerData[i].character_highlighted][0], 2.0f + settings_pos[0][1]);
        HuSprAttrReset(boardsettings_group, mtPlayerData[i].character + 0x1F, HUSPR_ATTR_DISPOFF);
        HuSprPosSet(boardsettings_group, mtPlayerData[i].character + 0x1F, settings_pos[mtPlayerData[i].character_highlighted][0], settings_pos[4][1] - 9.0f);
        HuSprAttrReset(boardsettings_group, mtPlayerData[i].character_highlighted + 0x27, HUSPR_ATTR_DISPOFF);
        HuSprPosSet(
            boardsettings_group, mtPlayerData[i].character_highlighted + 0x27, 1.0f + settings_pos[mtPlayerData[i].character_highlighted][0], settings_pos[4][1] - 7.0f);
        if (mtPlayerData[i].iscom != 0) {
            HuSprAttrReset(boardsettings_group, i + 0x1B, HUSPR_ATTR_DISPOFF);
            HuSprBankSet(boardsettings_group, i + 0x1B, mtPlayerData[i].diff);
            HuSprPosSet(boardsettings_group, i + 0x1B, settings_pos[mtPlayerData[i].character_highlighted][0], 21.0f + settings_pos[0][1]);
        }
        else {
            HuSprAttrReset(boardsettings_group, i + 0x17, HUSPR_ATTR_DISPOFF);
            HuSprBankSet(boardsettings_group, i + 0x17, mtPlayerData[i].character_highlighted);
            HuSprPosSet(boardsettings_group, i + 0x17, settings_pos[mtPlayerData[i].character_highlighted][0] - 8.0f, settings_pos[0][1] - 21.0f);
        }
        HuSprPosSet(boardsettings_group, i + 0x2F, settings_pos[mtPlayerData[i].character_highlighted][0], 20.0f + settings_pos[4][1]);
        HuSprAttrReset(boardsettings_group, i + 0x2F, HUSPR_ATTR_DISPOFF);
        HuSprPosSet(boardsettings_group, i + 0x33, 10.0f + settings_pos[mtPlayerData[i].character_highlighted][0], 21.0f + settings_pos[4][1]);
        HuSprAttrReset(boardsettings_group, i + 0x33, HUSPR_ATTR_DISPOFF);
        HuSprBankSet(boardsettings_group, i + 0x33, 0);
        HuSprPosSet(boardsettings_group, mtPlayerData[i].character + 0x37, settings_pos[mtPlayerData[i].character_highlighted][0], settings_pos[4][1] - 7.0f);
        HuSprPosSet(boardsettings_group, i + 0x3F, settings_pos[mtPlayerData[i].character_highlighted][0], 23.0f + settings_pos[4][1]);
        HuSprPosSet(boardsettings_group, i + 0x43, 14.0f + settings_pos[mtPlayerData[i].character_highlighted][0], 24.0f + settings_pos[4][1]);
        HuSprBankSet(boardsettings_group, i + 0x43, 0);
    }
    HuSprPosSet(boardsettings_group, 0x47, 340.0f, settings_pos[0][1]);
    HuSprScaleSet(boardsettings_group, 0x47, 0.0f, 0.0f);
    HuSprAttrReset(boardsettings_group, 0x47, HUSPR_ATTR_DISPOFF);
    #if VERSION_NTSC
    HuSprAttrReset(boardsettings_group, 0x48, HUSPR_ATTR_DISPOFF);
    HuSprPosSet(boardsettings_group, 0x48, 366.0f, settings_pos[1][1]);
    HuSprAttrReset(boardsettings_group, 0x49, HUSPR_ATTR_DISPOFF);
    HuSprPosSet(boardsettings_group, 0x49, 367.0f, 2.0f + settings_pos[1][1]);
    #endif
    for (i = 0; i < 2; i++) {
        HuSprAttrReset(boardsettings_group, i + 0x4A, HUSPR_ATTR_DISPOFF);
        HuSprPosSet(boardsettings_group, i + 0x4A, POSX1 - (i * (POSX1-POSX2)), settings_pos[1][1]);
        HuSprBankSet(boardsettings_group, i + 0x4A, i);
        HuSprAttrReset(boardsettings_group, i + 0x4C, HUSPR_ATTR_DISPOFF);
        HuSprPosSet(boardsettings_group, i + 0x4C, POSX1+1 - (i * (POSX1-POSX2)), 2.0f + settings_pos[1][1]);
        HuSprBankSet(boardsettings_group, i + 0x4C, i);
    }
    HuSprAttrReset(boardsettings_group, 0x4E, HUSPR_ATTR_DISPOFF);
    HuSprPosSet(boardsettings_group, 0x4E, 340.0f, settings_pos[2][1]);
    HuSprBankSet(boardsettings_group, 0x4E, 0);
    HuSprAttrReset(boardsettings_group, 0x4F, HUSPR_ATTR_DISPOFF);
    HuSprPosSet(boardsettings_group, 0x4F, 341.0f, 2.0f + settings_pos[2][1]);
    HuSprBankSet(boardsettings_group, 0x4F, 0);
    HuSprAttrReset(boardsettings_group, 0x50, HUSPR_ATTR_DISPOFF);
    HuSprPosSet(boardsettings_group, 0x50, 340.0f, settings_pos[3][1]);
    HuSprBankSet(boardsettings_group, 0x50, 0);
    HuSprAttrReset(boardsettings_group, 0x51, HUSPR_ATTR_DISPOFF);
    HuSprPosSet(boardsettings_group, 0x51, 341.0f, 2.0f + settings_pos[3][1]);
    HuSprBankSet(boardsettings_group, 0x51, 0);
    HuSprAttrReset(boardsettings_group, 0x52, HUSPR_ATTR_DISPOFF);
    for (i = 0; i < 4; i++) {
        HuSprAttrReset(boardsettings_group, i + 0x53, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(boardsettings_group, i + 0x57, HUSPR_ATTR_DISPOFF);
    }
    for (i = 0; i < 5; i++) {
        HuWinPosSet(board_configs[0].settings_text_win[i], 100.0f, -500.0f + (settings_pos[i][1] - 21.0f));
    }
    for (i = 0; i < 5; i++) {
        board_configs[1].handicaps[i] = board_configs[1].board_settings[i] = 0;
    }
    board_configs[1].board_settings[1] = 0x14;
    HuSprBankSet(boardsettings_group, 0x4A, 0);
    HuSprBankSet(boardsettings_group, 0x4C, 0);
    HuSprBankSet(boardsettings_group, 0x4B, 2);
    HuSprBankSet(boardsettings_group, 0x4D, 2);
    HuSprGrpPosSet(boardsettings_group, 0.0f, -500.0f);
}

void configureBoardWithController(OMOBJ *board_menu_obj, MentBoardMenuConfig *board_config1)
{
    float arrows_start_pos[2];
    float value_and_arrow_x_bump;
    float value_new_scale;
    float ver_arrow_y_bump;
    s32 boardsettings_group;
    s32 progress;
    s32 i;
    s32 current_setting_section;
    s32 value_change_dir;

    value_change_dir = 0;  // By value I mean the values inside setting like for example, "20 turns"
    boardsettings_group = board_configs[0].boardsettings_group;
    {
        s32 players_combos_pos[4][4] = {
            { 0x106, 0x106, 0x106, 0x106 },
            { 0x13A, 0x133, 0x175, 0x175 },
            { 0x16E, 0x175, 0x133, 0x1A2 },
            { 0x1A2, 0x1A2, 0x1A2, 0x133 },
        };
        board_configs[0].handicaps[3] = 0;
        current_setting_section = board_config1->handicaps[0];
        arrows_start_pos[0] = HuSprData[HuSprGrpData[boardsettings_group].members[3]].pos.x;
        arrows_start_pos[1] = HuSprData[HuSprGrpData[boardsettings_group].members[4]].pos.x;
        if ((HuPadBtnDown[mtPlayerData[0].pad_idx] & PAD_BUTTON_A) != 0) {
            board_config1->handicaps[0]++;
            HuAudFXPlay(2);
            if (board_config1->handicaps[0] > 4) {
                board_configs[0].handicaps[1] = 1;
                board_config1->handicaps[0] = 4;
                hideHighlighter();
            }
            board_configs[0].handicaps[3] = 1;
        }
        else if ((HuPadBtnDown[mtPlayerData[0].pad_idx] & PAD_BUTTON_B) != 0) {
            board_config1->handicaps[0]--;
            HuAudFXPlay(3);
            if (board_config1->handicaps[0] < 0) {
                board_configs[0].handicaps[1] = -1;
                board_config1->handicaps[0] = 0;
                hideHighlighter();
            }
            board_configs[0].handicaps[3] = 1;
        }
        else if ((HuPadStkX[mtPlayerData[0].pad_idx] >= 5) || ((HuPadBtn[mtPlayerData[0].pad_idx] & PAD_BUTTON_RIGHT) != 0)) {
            value_change_dir = 1;
            board_configs[0].handicaps[3] = 1;
            HuAudFXPlay(0);
            switch (board_config1->handicaps[0]) {
                case 0:
                    board_config1->board_settings[0] += 1;
                    if (board_config1->board_settings[0] > 3) {
                        board_config1->board_settings[0] = 0;
                    }
                    break;
                case 1: // Turn selection in settings
                    board_config1->board_settings[1] += 1; // Increase turn
                    if (board_config1->board_settings[1] > 99) {
                        board_config1->board_settings[1] = 1;
                    }
                    break;
                case 2:
                    if (gameConfigs[6] == 1) {
                        board_config1->board_settings[2]++;
                        if (board_config1->board_settings[2] > 2) {
                            board_config1->board_settings[2] = 0;
                        }
                    }
                    else {
                        board_config1->board_settings[2]++;
                        if (board_config1->board_settings[2] > 1) {
                            board_config1->board_settings[2] = 0;
                        }
                    }
                    break;
                case 3:
                    board_config1->board_settings[3]++;
                    if (board_config1->board_settings[3] > 1) {
                        board_config1->board_settings[3] = 0;
                    }
                    break;
                case 4:
                    board_config1->board_settings[4] += 1;
                    if (board_config1->board_settings[4] > 3) {
                        board_config1->board_settings[4] = 0;
                    }
                    break;
            }
        }
        else if ((HuPadStkX[mtPlayerData[0].pad_idx] <= -5) || ((HuPadBtn[mtPlayerData[0].pad_idx] & PAD_BUTTON_LEFT) != 0)) {
            value_change_dir = -1;
            board_configs[0].handicaps[3] = 1;
            HuAudFXPlay(0);
            switch (board_config1->handicaps[0]) {
                case 0:
                    board_config1->board_settings[0] -= 1;
                    if (board_config1->board_settings[0] < 0) {
                        board_config1->board_settings[0] = 3;
                    }
                    break;
                case 1: // Turn selection in settings
                    board_config1->board_settings[1] -= 1; // Increase turn
                    if (board_config1->board_settings[1] < 1) {
                        board_config1->board_settings[1] = 99;
                    }
                    break;
                case 2:
                    if (gameConfigs[6] == 1) {
                        board_config1->board_settings[2]--;
                        if (board_config1->board_settings[2] < 0) {
                            board_config1->board_settings[2] = 2;
                        }
                    }
                    else {
                        board_config1->board_settings[2]--;
                        if (board_config1->board_settings[2] < 0) {
                            board_config1->board_settings[2] = 1;
                        }
                    }
                    break;
                case 3:
                    board_config1->board_settings[3]--;
                    if (board_config1->board_settings[3] < 0) {
                        board_config1->board_settings[3] = 1;
                    }
                    break;
                case 4:
                    board_config1->board_settings[4] -= 1;
                    if (board_config1->board_settings[4] < 0) {
                        board_config1->board_settings[4] = 3;
                    }
                    break;
            }
        }
        else if (board_config1->handicaps[0] == 4) {
            if ((HuPadStkY[mtPlayerData[0].pad_idx] >= 0x1E) || ((HuPadBtn[mtPlayerData[0].pad_idx] & PAD_BUTTON_UP) != 0)) {
                board_configs[0].handicaps[3] = 1;
                HuAudFXPlay(0);
                for (progress = 0; progress <= 0xA; progress++) {
                    MenuPrcVSleep();
                    if (progress <= 5) {
                        ver_arrow_y_bump = SinEaseClamped(0.0f, 10.0f, progress, 5.0f);
                    }
                    else {
                        ver_arrow_y_bump = SinEaseClamped(10.0f, 0.0f, progress - 5, 5.0f);
                    }
                    HuSprPosSet(boardsettings_group, 5, settings_pos[board_config1->board_settings[4]][0], (settings_pos[4][1] - 48.0f) - ver_arrow_y_bump);
                    if (progress == 3) {
                        board_config1->handicaps[board_config1->board_settings[4] + 1]--;
                        if (board_config1->handicaps[board_config1->board_settings[4] + 1] < 0) {
                            board_config1->handicaps[board_config1->board_settings[4] + 1] = 9;
                        }
                        // Set handicap number
                        HuSprBankSet(boardsettings_group, board_config1->board_settings[4] + 0x33, board_config1->handicaps[board_config1->board_settings[4] + 1]);
                        HuSprBankSet(boardsettings_group, board_config1->board_settings[4] + 0x43, board_config1->handicaps[board_config1->board_settings[4] + 1]);
                    }
                }
            }
            else if ((HuPadStkY[mtPlayerData[0].pad_idx] <= -0x1E) || ((HuPadBtn[mtPlayerData[0].pad_idx] & 4) != 0)) {
                board_configs[0].handicaps[3] = 1;
                HuAudFXPlay(0);
                for (progress = 0; progress <= 0xA; progress++) {
                    MenuPrcVSleep();
                    if (progress <= 5) {
                        ver_arrow_y_bump = SinEaseClamped(0.0f, 10.0f, progress, 5.0f);
                    }
                    else {
                        ver_arrow_y_bump = SinEaseClamped(10.0f, 0.0f, progress - 5, 5.0f);
                    }
                    HuSprPosSet(boardsettings_group, 6, settings_pos[board_config1->board_settings[4]][0], 49.0f + settings_pos[4][1] + ver_arrow_y_bump);
                    if (progress == 3) {
                        board_config1->handicaps[board_config1->board_settings[4] + 1]++;
                        if (board_config1->handicaps[board_config1->board_settings[4] + 1] > 9) {
                            board_config1->handicaps[board_config1->board_settings[4] + 1] = 0;
                        }
                        HuSprBankSet(boardsettings_group, board_config1->board_settings[4] + 0x33, board_config1->handicaps[board_config1->board_settings[4] + 1]);
                        HuSprBankSet(boardsettings_group, board_config1->board_settings[4] + 0x43, board_config1->handicaps[board_config1->board_settings[4] + 1]);
                    }
                }
            }
        }
        if (value_change_dir != 0) {
            for (progress = 0; progress <= 0xA; progress++) {
                MenuPrcVSleep();
                if (progress <= 5) {
                    value_and_arrow_x_bump = SinEaseClamped(0.0f, 10.0f, progress, 5.0f);
                    value_new_scale = SinEaseClamped(1.0f, 0.0f, progress, 5.0f);
                }
                else {
                    value_and_arrow_x_bump = SinEaseClamped(10.0f, 0.0f, progress - 5, 5.0f);
                    value_new_scale = SinEaseClamped(0.0f, 1.0f, progress - 5, 5.0f);
                }
                if (board_config1->handicaps[0] != 4) {
                    if (value_change_dir == 1) {
                        HuSprPosSet(boardsettings_group, 4, value_and_arrow_x_bump + settings_pos[board_config1->handicaps[0]][4], settings_pos[board_config1->handicaps[0]][1]);
                    }
                    else {
                        HuSprPosSet(boardsettings_group, 3, settings_pos[board_config1->handicaps[0]][3] - value_and_arrow_x_bump, settings_pos[board_config1->handicaps[0]][1]);
                    }
                }
                switch (board_config1->handicaps[0]) {
                    case 0:
                        current_setting_section = board_config1->board_settings[0] - value_change_dir;
                        if (current_setting_section > 3) {
                            current_setting_section = 0;
                        }
                        else if (current_setting_section < 0) {
                            current_setting_section = 3;
                        }
                        if (board_config1->board_settings[0] != 0) {
                            value_new_scale = SinEaseClamped(0.0f, 1.0f, progress, 10.0f);
                        }
                        else {
                            value_new_scale = SinEaseClamped(1.0f, 0.0f, progress, 10.0f);
                        }
                        HuSprScaleSet(boardsettings_group, 0x47, value_new_scale, value_new_scale);
                        for (i = 0; i < 4; i++) {
                            value_and_arrow_x_bump = SinEaseClamped(
                                players_combos_pos[mtPlayerData[i].character_highlighted][current_setting_section], players_combos_pos[mtPlayerData[i].character_highlighted][board_config1->board_settings[0]], progress, 10.0f);
                            HuSprPosSet(boardsettings_group, mtPlayerData[i].character + 7, value_and_arrow_x_bump, settings_pos[0][1]);
                            HuSprPosSet(boardsettings_group, mtPlayerData[i].character + 0xF, 1.0f + value_and_arrow_x_bump, 2.0f + settings_pos[0][1]);
                            if (mtPlayerData[i].iscom != 0) {
                                HuSprPosSet(boardsettings_group, i + 0x1B, value_and_arrow_x_bump, 21.0f + settings_pos[0][1]);
                            }
                            else {
                                HuSprPosSet(boardsettings_group, i + 0x17, value_and_arrow_x_bump - 8.0f, settings_pos[0][1] - 21.0f);
                            }
                        }
                        break;
                    case 1:
                        if (value_change_dir == 1) {
                            #if VERSION_NTSC
                            HuSprPosSet(boardsettings_group, 0x48, 366.0f + value_and_arrow_x_bump, settings_pos[1][1]);
                            HuSprPosSet(boardsettings_group, 0x49, 367.0f + value_and_arrow_x_bump, 2.0f + settings_pos[1][1]);
                            #endif
                            HuSprPosSet(boardsettings_group, 0x4A, POSX1 + value_and_arrow_x_bump, settings_pos[1][1]);
                            HuSprPosSet(boardsettings_group, 0x4C, POSX1+1 + value_and_arrow_x_bump, 2.0f + settings_pos[1][1]);
                            HuSprPosSet(boardsettings_group, 0x4B, POSX2 + value_and_arrow_x_bump, settings_pos[1][1]);
                            HuSprPosSet(boardsettings_group, 0x4D, POSX2+1 + value_and_arrow_x_bump, 2.0f + settings_pos[1][1]);
                        }
                        else {
                            #if VERSION_NTSC
                            HuSprPosSet(boardsettings_group, 0x48, 366.0f - value_and_arrow_x_bump, settings_pos[1][1]);
                            HuSprPosSet(boardsettings_group, 0x49, 367.0f - value_and_arrow_x_bump, 2.0f + settings_pos[1][1]);
                            #endif
                            HuSprPosSet(boardsettings_group, 0x4A, POSX1 - value_and_arrow_x_bump, settings_pos[1][1]);
                            HuSprPosSet(boardsettings_group, 0x4C, POSX1+1 - value_and_arrow_x_bump, 2.0f + settings_pos[1][1]);
                            HuSprPosSet(boardsettings_group, 0x4B, POSX2 - value_and_arrow_x_bump, settings_pos[1][1]);
                            HuSprPosSet(boardsettings_group, 0x4D, POSX2+1 - value_and_arrow_x_bump, 2.0f + settings_pos[1][1]);
                        }
                        #if VERSION_NTSC
                        HuSprScaleSet(boardsettings_group, 0x48, 1.0f, value_new_scale);
                        HuSprScaleSet(boardsettings_group, 0x49, 1.0f, value_new_scale);
                        #endif
                        HuSprScaleSet(boardsettings_group, 0x4A, 1.0f, value_new_scale);
                        HuSprScaleSet(boardsettings_group, 0x4C, 1.0f, value_new_scale);
                        HuSprScaleSet(boardsettings_group, 0x4B, 1.0f, value_new_scale);
                        HuSprScaleSet(boardsettings_group, 0x4D, 1.0f, value_new_scale);
                        break;
                    case 2:
                        if (value_change_dir == 1) {
                            HuSprPosSet(boardsettings_group, 0x4E, 340.0f + value_and_arrow_x_bump, settings_pos[2][1]);
                            HuSprPosSet(boardsettings_group, 0x4F, 341.0f + value_and_arrow_x_bump, 2.0f + settings_pos[2][1]);
                        }
                        else {
                            HuSprPosSet(boardsettings_group, 0x4E, 340.0f - value_and_arrow_x_bump, settings_pos[2][1]);
                            HuSprPosSet(boardsettings_group, 0x4F, 341.0f - value_and_arrow_x_bump, 2.0f + settings_pos[2][1]);
                        }
                        HuSprScaleSet(boardsettings_group, 0x4E, 1.0f, value_new_scale);
                        HuSprScaleSet(boardsettings_group, 0x4F, 1.0f, value_new_scale);
                        break;
                    case 3:
                        if (value_change_dir == 1) {
                            HuSprPosSet(boardsettings_group, 0x50, 340.0f + value_and_arrow_x_bump, settings_pos[3][1]);
                            HuSprPosSet(boardsettings_group, 0x51, 341.0f + value_and_arrow_x_bump, 2.0f + settings_pos[3][1]);
                        }
                        else {
                            HuSprPosSet(boardsettings_group, 0x50, 340.0f - value_and_arrow_x_bump, settings_pos[3][1]);
                            HuSprPosSet(boardsettings_group, 0x51, 341.0f - value_and_arrow_x_bump, 2.0f + settings_pos[3][1]);
                        }
                        HuSprScaleSet(boardsettings_group, 0x50, 1.0f, value_new_scale);
                        HuSprScaleSet(boardsettings_group, 0x51, 1.0f, value_new_scale);
                        break;
                    case 4:
                        value_and_arrow_x_bump = SinEaseClamped(arrows_start_pos[0], settings_pos[board_config1->board_settings[4]][0] - 32.0f, progress, 10.0f);
                        HuSprPosSet(boardsettings_group, 3, value_and_arrow_x_bump, settings_pos[4][1]);
                        value_and_arrow_x_bump = SinEaseClamped(arrows_start_pos[1], 33.0f + settings_pos[board_config1->board_settings[4]][0], progress, 10.0f);
                        HuSprPosSet(boardsettings_group, 4, value_and_arrow_x_bump, settings_pos[4][1]);
                        value_and_arrow_x_bump = SinEaseClamped(32.0f + arrows_start_pos[0], settings_pos[board_config1->board_settings[4]][0], progress, 10.0f);
                        HuSprPosSet(boardsettings_group, 5, value_and_arrow_x_bump, settings_pos[4][1] - 48.0f);
                        HuSprPosSet(boardsettings_group, 6, value_and_arrow_x_bump, 49.0f + settings_pos[4][1]);
                        if (progress == 3) {
                            for (i = 0; i < 4; i++) {
                                HuSprAttrSet(boardsettings_group, mtPlayerData[i].character + 0x37, HUSPR_ATTR_DISPOFF);
                                HuSprAttrSet(boardsettings_group, i + 0x3F, HUSPR_ATTR_DISPOFF);
                                HuSprAttrSet(boardsettings_group, i + 0x43, HUSPR_ATTR_DISPOFF);
                            }
                            HuSprAttrReset(boardsettings_group, mtPlayerData[board_config1->board_settings[4]].character + 0x37, HUSPR_ATTR_DISPOFF);
                            HuSprAttrReset(boardsettings_group, board_config1->board_settings[4] + 0x3F, HUSPR_ATTR_DISPOFF);
                            HuSprAttrReset(boardsettings_group, board_config1->board_settings[4] + 0x43, HUSPR_ATTR_DISPOFF);
                        }
                        break;
                }
                if (progress == 6) {
                    switch (board_config1->handicaps[0]) {
                        case 1: // Animate turn setting change
                            // Disable for now as numbers 6 through 9 are not supported
                            // HuSprBankSet(boardsettings_group, 0x4A, board_config1->board_settings[1] % 10);
                            // HuSprBankSet(boardsettings_group, 0x4C, board_config1->board_settings[1] % 10);
                            // HuSprBankSet(boardsettings_group, 0x4B, board_config1->board_settings[1] / 10);
                            // HuSprBankSet(boardsettings_group, 0x4D, board_config1->board_settings[1] / 10);
                            HuSprBankSet(boardsettings_group, 0x4A, 0);
                            HuSprBankSet(boardsettings_group, 0x4C, 0);
                            HuSprBankSet(boardsettings_group, 0x4B, 0);
                            HuSprBankSet(boardsettings_group, 0x4D, 0);
                            break;
                        case 2:
                            HuSprBankSet(boardsettings_group, 0x4E, board_config1->board_settings[2]);
                            HuSprBankSet(boardsettings_group, 0x4F, board_config1->board_settings[2]);
                            break;
                        case 3:
                            HuSprBankSet(boardsettings_group, 0x50, board_config1->board_settings[3]);
                            HuSprBankSet(boardsettings_group, 0x51, board_config1->board_settings[3]);
                            break;
                    }
                }
            }
        }
        else if (current_setting_section != board_config1->handicaps[0]) {
            for (progress = 0; progress <= 0xA; progress++) {
                MenuPrcVSleep();
                ver_arrow_y_bump = SinEaseClamped(settings_pos[current_setting_section][1], settings_pos[board_config1->handicaps[0]][1], progress, 10.0f);
                value_new_scale = SinEaseClamped(settings_pos[current_setting_section][2], settings_pos[board_config1->handicaps[0]][2], progress, 10.0f);
                HuSprPosSet(boardsettings_group, 2, 288.0f, ver_arrow_y_bump);
                HuSprScaleSet(boardsettings_group, 2, 1.0f, value_new_scale);
                if (board_config1->handicaps[0] != 4) {
                    value_and_arrow_x_bump = SinEaseClamped(arrows_start_pos[0], settings_pos[board_config1->handicaps[0]][3], progress, 10.0f);
                    HuSprPosSet(boardsettings_group, 3, value_and_arrow_x_bump, ver_arrow_y_bump);
                    value_and_arrow_x_bump = SinEaseClamped(arrows_start_pos[1], settings_pos[board_config1->handicaps[0]][4], progress, 10.0f);
                    HuSprPosSet(boardsettings_group, 4, value_and_arrow_x_bump, ver_arrow_y_bump);
                    value_new_scale = SinEaseClamped(1.0f, 0.0f, progress, 10.0f);
                    HuSprTPLvlSet(boardsettings_group, 5, value_new_scale);
                    HuSprTPLvlSet(boardsettings_group, 6, value_new_scale);
                    ver_arrow_y_bump = SinEaseClamped(0.0f, 20.0f, progress, 10.0f);
                    HuSprPosSet(boardsettings_group, 5, settings_pos[board_config1->board_settings[4]][0], (settings_pos[4][1] - ver_arrow_y_bump) - 48.0f);
                    HuSprPosSet(boardsettings_group, 6, settings_pos[board_config1->board_settings[4]][0], 49.0f + (settings_pos[4][1] + ver_arrow_y_bump));
                    for (i = 0; i < 4; i++) {
                        HuSprAttrSet(boardsettings_group, mtPlayerData[i].character + 0x37, HUSPR_ATTR_DISPOFF);
                        HuSprAttrSet(boardsettings_group, i + 0x3F, HUSPR_ATTR_DISPOFF);
                        HuSprAttrSet(boardsettings_group, i + 0x43, HUSPR_ATTR_DISPOFF);
                    }
                }
                else {
                    value_and_arrow_x_bump = SinEaseClamped(arrows_start_pos[0], settings_pos[board_config1->board_settings[4]][0] - 32.0f, progress, 10.0f);
                    HuSprPosSet(boardsettings_group, 3, value_and_arrow_x_bump, ver_arrow_y_bump);
                    value_and_arrow_x_bump = SinEaseClamped(arrows_start_pos[1], 33.0f + settings_pos[board_config1->board_settings[4]][0], progress, 10.0f);
                    HuSprPosSet(boardsettings_group, 4, value_and_arrow_x_bump, ver_arrow_y_bump);
                    HuSprAttrReset(boardsettings_group, 5, HUSPR_ATTR_DISPOFF);
                    HuSprAttrReset(boardsettings_group, 6, HUSPR_ATTR_DISPOFF);
                    value_new_scale = SinEaseClamped(0.0f, 1.0f, progress, 10.0f);
                    HuSprTPLvlSet(boardsettings_group, 5, value_new_scale);
                    HuSprTPLvlSet(boardsettings_group, 6, value_new_scale);
                    ver_arrow_y_bump = SinEaseClamped(20.0f, 0.0f, progress, 10.0f);
                    HuSprPosSet(boardsettings_group, 5, settings_pos[board_config1->board_settings[4]][0], (settings_pos[4][1] - ver_arrow_y_bump) - 48.0f);
                    HuSprPosSet(boardsettings_group, 6, settings_pos[board_config1->board_settings[4]][0], 49.0f + (settings_pos[4][1] + ver_arrow_y_bump));
                    if (progress == 3) {
                        HuSprAttrReset(boardsettings_group, mtPlayerData[board_config1->board_settings[4]].character + 0x37, HUSPR_ATTR_DISPOFF);
                        HuSprAttrReset(boardsettings_group, board_config1->board_settings[4] + 0x3F, HUSPR_ATTR_DISPOFF);
                        HuSprAttrReset(boardsettings_group, board_config1->board_settings[4] + 0x43, HUSPR_ATTR_DISPOFF);
                    }
                }
            }
            if (board_config1->handicaps[0] != 4) {
                HuSprAttrSet(boardsettings_group, 5, HUSPR_ATTR_DISPOFF);
                HuSprAttrSet(boardsettings_group, 6, HUSPR_ATTR_DISPOFF);
            }
        }
        board_configs[0].handicaps[3] = 0;
    }
}

#undef POSX2
#undef POSX1

void createChooseBoardSettingsGroup(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3)
{
    s32 i;
    s32 sprite_idx;
    s32 group;
    // TODO: Determine what sprite animations there are
    ANIMDATA *sprite_animation_1;
    ANIMDATA *sprite_animation_2;
    ANIMDATA *sprite_animation_3;

    s32 cornerProtectorPos[4][2] = {
        { 0x00000070, 0x00000060 },
        { 0x000001D2, 0x00000060 },
        { 0x00000070, 0x0000012D },
        { 0x000001D2, 0x0000012D },
    };
    group = HuSprGrpCreate(0x5B);
    game_config0->boardsettings_group = group;
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3D)), 0x7C6, 0);
    HuSprGrpMemberSet(group, 0, sprite_idx);
    HuSprPosSet(group, 0, 288.0f, 54.0f);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3F)), 0x7C6, 0);
    HuSprGrpMemberSet(group, 1, sprite_idx);
    HuSprPosSet(group, 1, 288.0f, 54.0f);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x41)), 0x7BC, 0);
    HuSprGrpMemberSet(group, 2, sprite_idx);
    sprite_animation_1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x42));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(sprite_animation_1, 0, 0);
        HuSprGrpMemberSet(group, i + 3, sprite_idx);
        HuSprBankSet(group, i + 3, i);
    }
    sprite_animation_3 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x39));
    for (i = 0; i < 8; i++) {
        sprite_animation_1 = HuSprAnimReadFile(i + DATA_MAKE_NUM(DATADIR_MENT, 0x31));
        sprite_animation_2 = HuSprAnimReadFile(i + DATA_MAKE_NUM(DATADIR_MENT, 0x4E));
        sprite_idx = HuSprCreate(sprite_animation_1, 0x3E8, 0);
        HuSprGrpMemberSet(group, i + 7, sprite_idx);
        sprite_idx = HuSprCreate(sprite_animation_3, 0x3F2, 0);
        HuSprGrpMemberSet(group, i + 0xF, sprite_idx);
        sprite_idx = HuSprCreate(sprite_animation_1, 0x3E8, 0);
        HuSprGrpMemberSet(group, i + 0x1F, sprite_idx);
        sprite_idx = HuSprCreate(sprite_animation_3, 0x3F2, 0);
        HuSprGrpMemberSet(group, i + 0x27, sprite_idx);
        sprite_idx = HuSprCreate(sprite_animation_2, 0x3DE, 0);
        HuSprGrpMemberSet(group, i + 0x37, sprite_idx);
    }
    sprite_animation_1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3A));
    sprite_animation_2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x3B));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(sprite_animation_1, 0x3DE, 0);
        HuSprGrpMemberSet(group, i + 0x17, sprite_idx);
        sprite_idx = HuSprCreate(sprite_animation_2, 0x3DE, 0);
        HuSprGrpMemberSet(group, i + 0x1B, sprite_idx);
    }
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x43)), 0x3DE, 0);
    HuSprGrpMemberSet(group, 0x47, sprite_idx);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x44)), 0x3E8, 0);
    HuSprGrpMemberSet(group, 0x48, sprite_idx);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x46)), 0x3F2, 0);
    HuSprGrpMemberSet(group, 0x49, sprite_idx);
    sprite_animation_1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x45));
    sprite_animation_2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x47));
    for (i = 0; i < 2; i++) {
        sprite_idx = HuSprCreate(sprite_animation_1, 0x3E8, 0);
        HuSprGrpMemberSet(group, i + 0x4A, sprite_idx);
        sprite_idx = HuSprCreate(sprite_animation_2, 0x3F2, 0);
        HuSprGrpMemberSet(group, i + 0x4C, sprite_idx);
    }
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x48)), 0x3E8, 0);
    HuSprGrpMemberSet(group, 0x4E, sprite_idx);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x49)), 0x3F2, 0);
    HuSprGrpMemberSet(group, 0x4F, sprite_idx);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x4A)), 0x3E8, 0);
    HuSprGrpMemberSet(group, 0x50, sprite_idx);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x4B)), 0x3F2, 0);
    HuSprGrpMemberSet(group, 0x51, sprite_idx);
    sprite_animation_1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x4C));
    sprite_animation_2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x4D));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(sprite_animation_1, 0x3DE, 0);
        HuSprGrpMemberSet(group, i + 0x2F, sprite_idx);
        sprite_idx = HuSprCreate(sprite_animation_2, 0x3D4, 0);
        HuSprGrpMemberSet(group, i + 0x33, sprite_idx);
    }
    sprite_animation_1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x56));
    sprite_animation_2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x57));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(sprite_animation_1, 0x3CA, 0);
        HuSprGrpMemberSet(group, i + 0x3F, sprite_idx);
        sprite_idx = HuSprCreate(sprite_animation_2, 0x3C0, 0);
        HuSprGrpMemberSet(group, i + 0x43, sprite_idx);
    }
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x40)), 0x7D0, 0);
    HuSprGrpMemberSet(group, 0x52, sprite_idx);
    HuSprTPLvlSet(group, 0x52, 0.8f);
    sprPosSet_YPadded(group, 0x52, 288.0f, 240.0f);
    sprite_animation_1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x18));
    sprite_animation_2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x19));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(sprite_animation_1, 0x7BC, 0);
        HuSprGrpMemberSet(group, i + 0x53, sprite_idx);
        HuSprBankSet(group, i + 0x53, i);
        sprPosSet_YPadded(group, i + 0x53, cornerProtectorPos[i][0], cornerProtectorPos[i][1]);
        sprite_idx = HuSprCreate(sprite_animation_2, 0x7C6, 0);
        HuSprGrpMemberSet(group, i + 0x57, sprite_idx);
        HuSprBankSet(group, i + 0x57, i);
        HuSprTPLvlSet(group, i + 0x57, 0.5f);
        sprPosSet_YPadded(group, i + 0x57, cornerProtectorPos[i][0] + 1, cornerProtectorPos[i][1] + 6);
    }
    for (i = 0; i < 5; i++) {
        game_config0->settings_text_win[i] = HuWinExCreateStyled(0.0f, 0.0f, 0xC8, 0xC8, -1, 1);
        HuWinBGTPLvlSet(game_config0->settings_text_win[i], 0.0f);
        HuWinMesSet(game_config0->settings_text_win[i], i + 0x1B0010);
        HuWinMesSpeedSet(game_config0->settings_text_win[i], 0);
        HuWinDispOn(game_config0->settings_text_win[i]);
        winData[game_config0->settings_text_win[i]].mess_shadow_color = 0;
    }
    hideBowserGnarlyBoardSettings();
}

void setupSettingSelectionCursor(void)
{
    s32 i;
    s32 boardsettings_group;
    MentBoardMenuConfig *board_config1;

    boardsettings_group = board_configs[0].boardsettings_group;
    board_config1 = &board_configs[1];
    HuSprAttrReset(boardsettings_group, 2, HUSPR_ATTR_DISPOFF);
    if (board_configs[1].handicaps[0] == 4) {
        for (i = 0; i < 4; i++) {
            HuSprAttrReset(boardsettings_group, i + 3, HUSPR_ATTR_DISPOFF);
        }
        HuSprAttrReset(boardsettings_group, mtPlayerData[board_config1->board_settings[4]].character + 0x37, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(boardsettings_group, board_config1->board_settings[4] + 0x3F, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(boardsettings_group, board_config1->board_settings[4] + 0x43, HUSPR_ATTR_DISPOFF);
        return;
    }
    for (i = 0; i < 2; i++) {
        HuSprAttrReset(boardsettings_group, i + 3, HUSPR_ATTR_DISPOFF);
    }
}

void hideHighlighter(void)
{
    MentBoardMenuConfig *board_config1;
    s32 boardsettings_group;
    s32 i;

    boardsettings_group = board_configs[0].boardsettings_group;
    board_config1 = &board_configs[1];
    HuSprAttrSet(boardsettings_group, 2, HUSPR_ATTR_DISPOFF);
    for (i = 0; i < 4; i++) {
        HuSprAttrSet(boardsettings_group, i + 3, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrSet(boardsettings_group, mtPlayerData[board_config1->board_settings[4]].character + 0x37, HUSPR_ATTR_DISPOFF);
    HuSprAttrSet(boardsettings_group, board_config1->board_settings[4] + 0x3F, HUSPR_ATTR_DISPOFF);
    HuSprAttrSet(boardsettings_group, board_config1->board_settings[4] + 0x43, HUSPR_ATTR_DISPOFF);
}

void pushDownBoardSettings(void)
{
    float y;
    s32 progress;
    MentBoardMenuConfig *board_config;
    s32 setting_idx;
    s32 boardsettings_group;
    s32 i;

    hideBowserGnarlyBoardSettings();
    boardsettings_group = board_configs[0].boardsettings_group;
    board_config = &board_configs[1];
    HuSprAttrSet(boardsettings_group, 2, 4);
    for (i = 0; i < 4; i++) {
        HuSprAttrSet(boardsettings_group, i + 3, 4);
    }
    HuSprAttrSet(boardsettings_group, mtPlayerData[board_config->board_settings[4]].character + 0x37, 4);
    HuSprAttrSet(boardsettings_group, board_config->board_settings[4] + 0x3F, 4);
    HuSprAttrSet(boardsettings_group, board_config->board_settings[4] + 0x43, 4);
    for (progress = 0; progress <= 60; progress++) {
        MenuPrcVSleep();
        if (progress <= 0x32) {
            y = SinEaseClamped(-500.0f, 10.0f, progress, 50.0f);
        }
        else {
            y = CosEaseClamped(10.0f, 0.0f, progress - 0x32, 10.0f);
        }
        HuSprGrpPosSet(board_configs[0].boardsettings_group, 0.0f, y);
        for (setting_idx = 0; setting_idx < 5; setting_idx++) {
            HuWinPosSet(board_configs->settings_text_win[setting_idx], 100.0f, settings_pos[setting_idx][1] - 21.0f + y);
        }
    }
}

void pushUpBoardSettings(void)
{
    float y;
    s32 progress;
    MentBoardMenuConfig *board_config1;
    s32 setting_idx;
    s32 boardsettings_group;
    s32 i;

    boardsettings_group = board_configs[0].boardsettings_group;
    board_config1 = &board_configs[1];
    HuSprAttrSet(boardsettings_group, 2, 4);
    for (i = 0; i < 4; i++) {
        HuSprAttrSet(boardsettings_group, i + 3, 4);
    }
    HuSprAttrSet(boardsettings_group, mtPlayerData[board_config1->board_settings[4]].character + 0x37, 4);
    HuSprAttrSet(boardsettings_group, board_config1->board_settings[4] + 0x3F, 4);
    HuSprAttrSet(boardsettings_group, board_config1->board_settings[4] + 0x43, 4);
    for (progress = 0; progress <= 0x3C; progress++) {
        MenuPrcVSleep();
        if (progress <= 0xA) {
            y = SinEaseClamped(0.0f, 10.0f, progress, 10.0f);
        }
        else {
            y = CosEaseClamped(10.0f, -500.0f, progress - 0xA, 50.0f);
        }
        HuSprGrpPosSet(board_configs[0].boardsettings_group, 0.0f, y);
        for (setting_idx = 0; setting_idx < 5; setting_idx++) {
            HuWinPosSet(board_configs->settings_text_win[setting_idx], 100.0f, settings_pos[setting_idx][1] - 21.0f + y);
        }
    }
}

void hideStorySettings(void)
{
    s32 i;
    s32 choosecharacter_group;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    for (i = 0; i < 0x22; i++) {
        HuSprAttrSet(choosecharacter_group, i, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrReset(choosecharacter_group, 0, HUSPR_ATTR_DISPOFF);
    for (i = 0; i < 4; i++) {
        HuSprAttrReset(choosecharacter_group, i + 1, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(choosecharacter_group, i + 5, HUSPR_ATTR_DISPOFF);
    }
    HuSprPosSet(choosecharacter_group, 0x1B, 288.0f, storySettingsPos[1][2]);
    HuSprScaleSet(choosecharacter_group, 0x1B, 1.0f, 0.75f);
    HuSprPosSet(choosecharacter_group, 0x1C, storySettingsPos[1][3], storySettingsPos[1][2]);
    HuSprPosSet(choosecharacter_group, 0x1D, storySettingsPos[1][4], storySettingsPos[1][2]);
    for (i = 0; i < 8; i++) {
        HuSprBankSet(choosecharacter_group, i + 9, 0);
        HuSprAttrReset(choosecharacter_group, i + 9, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(choosecharacter_group, i + 0x11, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrReset(choosecharacter_group, 0x19, HUSPR_ATTR_DISPOFF);
    HuSprAttrReset(choosecharacter_group, 0x1A, HUSPR_ATTR_DISPOFF);
    HuSprBankSet(choosecharacter_group, 0x1E, 0);
    HuSprAttrReset(choosecharacter_group, 0x1E, HUSPR_ATTR_DISPOFF);
    HuSprBankSet(choosecharacter_group, 0x1F, 0);
    HuSprAttrReset(choosecharacter_group, 0x1F, HUSPR_ATTR_DISPOFF);
    HuSprBankSet(choosecharacter_group, 0x20, 0);
    HuSprAttrReset(choosecharacter_group, 0x20, HUSPR_ATTR_DISPOFF);
    HuSprBankSet(choosecharacter_group, 0x21, 0);
    HuSprAttrReset(choosecharacter_group, 0x21, HUSPR_ATTR_DISPOFF);
    for (i = 0; i < 3; i++) {
        HuWinPosSet(board_configs->settings_text_win[i], 100.0f, storySettingsPos[i][2] - 0x209);
    }
    board_configs[0].handicaps[0] = 1;
    board_configs[0].board_settings[0] = board_configs[0].board_settings[0] = 0;
    HuSprGrpPosSet(choosecharacter_group, 0.0f, -500.0f);
}

void initCharacterSelectionPos(void)
{
    float y;
    float highlighter_pos_x;
    float highlighter_pos_y;
    s32 choosecharacter_group;
    s32 i;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    for (i = 0; i < 8; i++) {
        HuSprBankSet(choosecharacter_group, i + 9, 0);
    }
    HuSprBankSet(choosecharacter_group, 0x19, 0);
    HuSprBankSet(choosecharacter_group, 0x1A, 0);
    HuSprPosSet(choosecharacter_group, 0x1B, 288.0f, storySettingsPos[1][2]);
    HuSprScaleSet(choosecharacter_group, 0x1B, 1.0f, 0.75f);
    HuSprPosSet(choosecharacter_group, 0x1C, storySettingsPos[1][3], storySettingsPos[1][2]);
    HuSprPosSet(choosecharacter_group, 0x1D, storySettingsPos[1][4], storySettingsPos[1][2]);
    for (i = 0; i <= 0xA; i++) {
        MenuPrcVSleep();
        if (i <= 5) {
            y = SinEaseClamped(1.0f, 0.0f, i, 5.0f);
        }
        else {
            y = SinEaseClamped(0.0f, 1.0f, i - 5, 5.0f);
        }
        if (mtPlayerData->character != 0) {
            highlighter_pos_x = SinEaseClamped(storySettingsPos[mtPlayerData->character % 4][0], storySettingsPos[0][0], i, 10.0f);
            highlighter_pos_y = SinEaseClamped(storySettingsPos[mtPlayerData->character / 4][1], storySettingsPos[0][1], i, 10.0f);
            HuSprPosSet(choosecharacter_group, 0x19, highlighter_pos_x, highlighter_pos_y);
            HuSprPosSet(choosecharacter_group, 0x1A, highlighter_pos_x, highlighter_pos_y);
        }
        if (board_configs[0].board_settings[0] != 0) {
            HuSprScaleSet(choosecharacter_group, 0x1E, 1.0f, y);
            HuSprScaleSet(choosecharacter_group, 0x1F, 1.0f, y);
        }
        if (board_configs[0].board_settings[1] != 0) {
            HuSprScaleSet(choosecharacter_group, 0x20, 1.0f, y);
            HuSprScaleSet(choosecharacter_group, 0x21, 1.0f, y);
        }
        if (i == 6) {
            HuSprBankSet(choosecharacter_group, 0x1E, 0);
            HuSprBankSet(choosecharacter_group, 0x1F, 0);
            HuSprBankSet(choosecharacter_group, 0x20, 0);
            HuSprBankSet(choosecharacter_group, 0x21, 0);
        }
    }
    mtPlayerData[0].unk_70[0] = mtPlayerData[0].character = 0;
    board_configs[0].handicaps[0] = 1;
    board_configs[0].board_settings[0] = board_configs[0].board_settings[1] = 0;
}
// Not sure what this does
void createChooseHumanCountGroupStory(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3)
{
    s32 i;
    s32 choosecharacter_group;
    s32 sprite_idx;
    ANIMDATA *anim1;
    ANIMDATA *anim2;

    s32 chooseHumanCountPos2[4][2] = {
        { 0x00000070, 0x00000037 },
        { 0x000001D2, 0x00000037 },
        { 0x00000070, 0x0000011A },
        { 0x000001D2, 0x0000011A },
    };
    game_config0->choosecharacter_group = choosecharacter_group = (s16)HuSprGrpCreate(0x22);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x58)), 0x7D0, 0);
    HuSprGrpMemberSet(choosecharacter_group, 0, sprite_idx);
    HuSprTPLvlSet(choosecharacter_group, 0, 0.8f);
    sprPosSet_YPadded(choosecharacter_group, 0, 288.0f, 240.0f);
    anim1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x18));
    anim2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x19));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(anim1, 0x7BC, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 1, sprite_idx);
        HuSprBankSet(choosecharacter_group, i + 1, i);
        sprPosSet_YPadded(choosecharacter_group, i + 1, chooseHumanCountPos2[i][0], chooseHumanCountPos2[i][1]);
        sprite_idx = HuSprCreate(anim2, 0x7C6, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 5, sprite_idx);
        HuSprBankSet(choosecharacter_group, i + 5, i);
        HuSprTPLvlSet(choosecharacter_group, i + 5, 0.5f);
        sprPosSet_YPadded(choosecharacter_group, i + 5, chooseHumanCountPos2[i][0] + 1, chooseHumanCountPos2[i][1] + 6);
    }
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x41)), 0x7BC, 0);
    HuSprGrpMemberSet(choosecharacter_group, 0x1B, sprite_idx);
    anim1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x42));
    for (i = 0; i < 2; i++) {
        sprite_idx = HuSprCreate(anim1, 0, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x1C, sprite_idx);
        HuSprBankSet(choosecharacter_group, i + 0x1C, i);
    }
    anim1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x22));
    for (i = 0; i < 8; i++) {
        sprite_idx = HuSprCreate(HuSprAnimReadFile(i + DATA_MAKE_NUM(DATADIR_MENT, 0x1A)), 0x3E8, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 9, sprite_idx);
        HuSprPosSet(choosecharacter_group, i + 9, storySettingsPos[i % 4][0], storySettingsPos[i / 4][1]);
        sprite_idx = HuSprCreate(anim1, 0x3F2, 0);
        HuSprGrpMemberSet(choosecharacter_group, i + 0x11, sprite_idx);
        HuSprTPLvlSet(choosecharacter_group, i + 0x11, 0.5f);
        HuSprPosSet(choosecharacter_group, i + 0x11, storySettingsPos[i % 4][0] + 1, storySettingsPos[i / 4][1] + 2);
    }
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x23)), 0, 0);
    HuSprGrpMemberSet(choosecharacter_group, 0x19, sprite_idx);
    HuSprPosSet(choosecharacter_group, 0x19, storySettingsPos[0][0], storySettingsPos[0][1]);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x28)), 0xA, 0);
    HuSprGrpMemberSet(choosecharacter_group, 0x1A, sprite_idx);
    HuSprPosSet(choosecharacter_group, 0x1A, storySettingsPos[0][0], storySettingsPos[0][1]);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x5D)), 0x3E8, 0);
    HuSprGrpMemberSet(choosecharacter_group, 0x1E, sprite_idx);
    HuSprPosSet(choosecharacter_group, 0x1E, 340.0f, storySettingsPos[1][2]);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x5E)), 0x3F2, 0);
    HuSprGrpMemberSet(choosecharacter_group, 0x1F, sprite_idx);
    HuSprTPLvlSet(choosecharacter_group, 0x1F, 0.5f);
    HuSprPosSet(choosecharacter_group, 0x1F, 341.0f, storySettingsPos[1][2] + 2);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x48)), 0x3E8, 0);
    HuSprGrpMemberSet(choosecharacter_group, 0x20, sprite_idx);
    HuSprPosSet(choosecharacter_group, 0x20, 340.0f, storySettingsPos[2][2]);
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x49)), 0x3F2, 0);
    HuSprGrpMemberSet(choosecharacter_group, 0x21, sprite_idx);
    HuSprTPLvlSet(choosecharacter_group, 0x21, 0.5f);
    HuSprPosSet(choosecharacter_group, 0x21, 341.0f, storySettingsPos[2][2] + 2);
    for (i = 0; i < 3; i++) {
        game_config0->settings_text_win[i] = HuWinExCreateStyled(0.0f, 0.0f, 0xC8, 0xC8, -1, 1);
        HuWinBGTPLvlSet(game_config0->settings_text_win[i], 0.0f);
        HuWinMesSet(game_config0->settings_text_win[i], i + 0x1B002D);
        HuWinMesSpeedSet(game_config0->settings_text_win[i], 0);
        HuWinDispOn(game_config0->settings_text_win[i]);
        if (i == 0) {
            HuWinDispOff(game_config0->settings_text_win[i]);
        }
        winData[game_config0->settings_text_win[i]].mess_shadow_color = 0;
    }
    hideStorySettings();
}

void fn_1_1DED8()
{
    s32 choosecharacter_group;
    s32 var_r31;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    HuSprAttrReset(choosecharacter_group, 0x1B, HUSPR_ATTR_DISPOFF);
    for (var_r31 = 0; var_r31 < 2; var_r31++) {
        HuSprAttrReset(choosecharacter_group, var_r31 + 0x1C, HUSPR_ATTR_DISPOFF);
    }
}

void fn_1_1DF48(void)
{
    s32 choosecharacter_group;
    s32 var_r31;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    HuSprAttrSet(choosecharacter_group, 0x1B, 4);
    for (var_r31 = 0; var_r31 < 2; var_r31++) {
        HuSprAttrSet(choosecharacter_group, var_r31 + 0x1C, 4);
    }
}

void pushDownStorySettings(void)
{
    float y;
    s32 progress;
    s32 j;
    s32 i;
    s32 choosecharacter_group;

    hideStorySettings();
    choosecharacter_group = board_configs[0].choosecharacter_group;
    HuSprAttrSet(choosecharacter_group, 0x1B, 4);
    for (i = 0; i < 2; i++) {
        HuSprAttrSet(choosecharacter_group, i + 0x1C, 4);
    }
    for (progress = 0; progress <= 0x3C; progress++) {
        MenuPrcVSleep();
        if (progress <= 0x32) {
            y = SinEaseClamped(-500.0f, 10.0f, progress, 50.0f);
        }
        else {
            y = CosEaseClamped(10.0f, 0.0f, progress - 0x32, 10.0f);
        }
        HuSprGrpPosSet(board_configs[0].choosecharacter_group, 0.0f, y);
        for (j = 0; j < 3; j++) {
            HuWinPosSet(board_configs[0].settings_text_win[j], 100.0f, storySettingsPos[j][2] - 0x15 + y);
        }
    }
}

void fn_1_1E1B4(void)
{
    float var_f31;
    s32 var_r31;
    MentBoardMenuConfig *var_r30;
    s32 var_r29;
    s32 var_r28;
    s32 var_r27;

    var_r28 = board_configs[0].boardsettings_group;
    var_r30 = &board_configs[1];
    HuSprAttrSet(var_r28, 2, 4);
    for (var_r27 = 0; var_r27 < 4; var_r27++) {
        HuSprAttrSet(var_r28, var_r27 + 3, 4);
    }
    HuSprAttrSet(var_r28, mtPlayerData[var_r30->board_settings[4]].character + 0x37, 4);
    HuSprAttrSet(var_r28, var_r30->board_settings[4] + 0x3F, 4);
    HuSprAttrSet(var_r28, var_r30->board_settings[4] + 0x43, 4);
    for (var_r31 = 0; var_r31 <= 0x3C; var_r31++) {
        MenuPrcVSleep();
        if (var_r31 <= 0xA) {
            var_f31 = SinEaseClamped(0.0f, 10.0f, var_r31, 10.0f);
        }
        else {
            var_f31 = CosEaseClamped(10.0f, -500.0f, var_r31 - 0xA, 50.0f);
        }
        HuSprGrpPosSet(board_configs[0].choosecharacter_group, 0.0f, var_f31);
        for (var_r29 = 0; var_r29 < 3; var_r29++) {
            HuWinPosSet(board_configs->settings_text_win[var_r29], 100.0f, storySettingsPos[var_r29][2] - 0x15 + var_f31);
        }
    }
}

void selectStoryCharacterCallback(OMOBJ *arg0, MTPlayerConfig *player)
{
    float x;
    float y;
    s32 col;
    s32 row;
    s32 choosecharacter_group;
    s32 progress;
    s32 col_change_dir;
    s32 row_change_dir;
    s32 original_col;
    s32 original_row;

    choosecharacter_group = board_configs[0].choosecharacter_group;
    col_change_dir = 0;
    row_change_dir = 0;
    col = player->character % 4;
    original_col = col;
    row = player->character / 4;
    original_row = row;
    if (player->unk_70[0] == 0) {
        if (HuPadBtnDown[player->pad_idx] & PAD_BUTTON_A) {
            player->unk_70[0] = 1;
            HuSprBankSet(choosecharacter_group, player->character + 9, 1);
            HuSprBankSet(choosecharacter_group, 0x19, 1);
            HuSprBankSet(choosecharacter_group, 0x1A, 1);
            HuAudFXPlay(player->character + 0x6D);
        }
        else {
            if ((HuPadStkX[player->pad_idx] >= 5) || ((HuPadBtn[player->pad_idx] & 2) != 0)) {
                col++;
                col_change_dir = 1;
                if (col > 3) {
                    col = 3;
                }
            }
            else if ((HuPadStkX[player->pad_idx] <= -5) || ((HuPadBtn[player->pad_idx] & 1) != 0)) {
                col--;
                col_change_dir = -1;
                if (col < 0) {
                    col = 0;
                }
            }
            if ((HuPadStkY[player->pad_idx] <= -5) || ((HuPadBtn[player->pad_idx] & 4) != 0)) {
                row++;
                row_change_dir = 1;
                if (row > 1) {
                    row = 1;
                }
            }
            else if ((HuPadStkY[player->pad_idx] >= 5) || ((HuPadBtn[player->pad_idx] & 8) != 0)) {
                row--;
                row_change_dir = -1;
                if (row < 0) {
                    row = 0;
                }
            }
        }
    }
    else if ((HuPadBtnDown[player->pad_idx] & 0x200) != 0) {
        player->unk_70[0] = 0;
        HuSprBankSet(choosecharacter_group, player->character + 9, 0);
        HuSprBankSet(choosecharacter_group, 0x19, 0);
        HuSprBankSet(choosecharacter_group, 0x1A, 0);
        HuAudFXPlay(3);
    }
    if (player->character != (col + (row * 4))) {
        player->character = col + (row * 4);
        HuAudFXPlay(0);
        for (progress = 0; progress <= 0xA; progress++) {
            MenuPrcVSleep();
            x = SinEaseClamped(storySettingsPos[original_col][0], storySettingsPos[col][0], progress, 10.0f);
            y = SinEaseClamped(storySettingsPos[original_row][1], storySettingsPos[row][1], progress, 10.0f);
            HuSprPosSet(choosecharacter_group, 0x19, x, y);
            HuSprPosSet(choosecharacter_group, 0x1A, x, y);
        }
    }
}

void selectStorySettingValesCallback(OMOBJ *arg0, MentBoardMenuConfig *board_config0)
{
    float x;
    float value_y_scale;
    float highlighter_y_pos;
    s32 choosecharacter_group;
    s32 progress;
    s32 value_change_dir;
    s32 setting_section;
    s32 i;
    s32 j;
    s32 choosecharacter_group_same;
    s32 choosecharacter_group_same2;

    choosecharacter_group = board_config0->choosecharacter_group;
    value_change_dir = 0;
    setting_section = board_config0->handicaps[0];
    board_configs[0].handicaps[3] = 0;
    if (HuPadBtnDown[mtPlayerData->pad_idx] & PAD_BUTTON_A) {
        board_configs[0].handicaps[3] = 1;
        board_config0->handicaps[0]++;
        if (board_config0->handicaps[0] > 2) {
            board_config0->handicaps[0] = 2;
            board_configs[1].handicaps[1] = 1;
            choosecharacter_group_same = board_configs[0].choosecharacter_group;
            HuSprAttrSet(choosecharacter_group_same, 0x1B, 4);
            for (i = 0; i < 2; i++) {
                HuSprAttrSet(choosecharacter_group_same, i + 0x1C, 4);
            }
        }
        HuAudFXPlay(2);
    }
    else if (HuPadBtnDown[mtPlayerData->pad_idx] & PAD_BUTTON_B) {
        board_configs[0].handicaps[3] = 1;
        board_config0->handicaps[0]--;
        if (board_config0->handicaps[0] < 1) {
            board_config0->handicaps[0] = 1;
            board_configs[1].handicaps[1] = -1;
            choosecharacter_group_same2 = board_configs[0].choosecharacter_group;
            HuSprAttrSet(choosecharacter_group_same2, 0x1B, 4);
            for (j = 0; j < 2; j++) {
                HuSprAttrSet(choosecharacter_group_same2, j + 0x1C, 4);
            }
        }
        HuAudFXPlay(3);
    }
    else if ((HuPadStkX[mtPlayerData->pad_idx] >= 5) || (HuPadBtn[mtPlayerData->pad_idx] & PAD_BUTTON_RIGHT)) {
        value_change_dir = 1;
        board_configs[0].handicaps[3] = 1;
        switch (board_config0->handicaps[0]) {
            case 1:
                if (gameConfigs[5] == 1) {
                    board_config0->board_settings[0] += 1;
                    if (board_config0->board_settings[0] > 4) {
                        board_config0->board_settings[0] = 0;
                    }
                }
                else {
                    board_config0->board_settings[0] += 1;
                    if (board_config0->board_settings[0] > 4) {
                        board_config0->board_settings[0] = 0;
                    }
                    if (board_config0->board_settings[0] == 3) {
                        board_config0->board_settings[0] = 4;
                    }
                }
                break;
            case 2:
                if (gameConfigs[6] == 1) {
                    board_config0->board_settings[1]++;
                    if (board_config0->board_settings[1] > 2) {
                        board_config0->board_settings[1] = 0;
                    }
                }
                else {
                    board_config0->board_settings[1]++;
                    if (board_config0->board_settings[1] > 1) {
                        board_config0->board_settings[1] = 0;
                    }
                }
                break;
        }
        HuAudFXPlay(0);
    }
    else if ((HuPadStkX[mtPlayerData->pad_idx] <= -5) || ((HuPadBtn[mtPlayerData->pad_idx] & 1) != 0)) {
        value_change_dir = -1;
        board_configs[0].handicaps[3] = 1;
        switch (board_config0->handicaps[0]) {
            case 1:
                if (gameConfigs[5] == 1) {
                    board_config0->board_settings[0] -= 1;
                    if (board_config0->board_settings[0] < 0) {
                        board_config0->board_settings[0] = 4;
                    }
                }
                else {
                    board_config0->board_settings[0] -= 1;
                    if (board_config0->board_settings[0] < 0) {
                        board_config0->board_settings[0] = 4;
                    }
                    if (board_config0->board_settings[0] == 3) {
                        board_config0->board_settings[0] = 2;
                    }
                }
                break;
            case 2:
                if (gameConfigs[6] == 1) {
                    board_config0->board_settings[1]--;
                    if (board_config0->board_settings[1] < 0) {
                        board_config0->board_settings[1] = 2;
                    }
                }
                else {
                    board_config0->board_settings[1]--;
                    if (board_config0->board_settings[1] < 0) {
                        board_config0->board_settings[1] = 1;
                    }
                }
                break;
        }
        HuAudFXPlay(0);
    }
    if (setting_section != board_config0->handicaps[0]) {
        for (progress = 0; progress < 0xB; progress++) {
            MenuPrcVSleep();
            highlighter_y_pos = SinEaseClamped(storySettingsPos[setting_section][2], storySettingsPos[board_config0->handicaps[0]][2], progress, 10.0f);
            HuSprPosSet(choosecharacter_group, 0x1B, 288.0f, highlighter_y_pos);
            x = SinEaseClamped(storySettingsPos[setting_section][3], storySettingsPos[board_config0->handicaps[0]][3], progress, 10.0f);
            HuSprPosSet(choosecharacter_group, 0x1C, x, highlighter_y_pos);
            x = SinEaseClamped(storySettingsPos[setting_section][4], storySettingsPos[board_config0->handicaps[0]][4], progress, 10.0f);
            HuSprPosSet(choosecharacter_group, 0x1D, x, highlighter_y_pos);
        }
    }
    else if (value_change_dir != 0) {
        for (progress = 0; progress <= 0xA; progress++) {
            MenuPrcVSleep();
            if (progress <= 5) {
                x = SinEaseClamped(0.0f, 10.0f, progress, 5.0f);
                value_y_scale = SinEaseClamped(1.0f, 0.0f, progress, 5.0f);
            }
            else {
                x = SinEaseClamped(10.0f, 0.0f, progress - 5, 5.0f);
                value_y_scale = SinEaseClamped(0.0f, 1.0f, progress - 5, 5.0f);
            }
            if (value_change_dir == 1) {
                HuSprPosSet(choosecharacter_group, 0x1D, x + storySettingsPos[board_config0->handicaps[0]][4], storySettingsPos[board_config0->handicaps[0]][2]);
            }
            else {
                HuSprPosSet(choosecharacter_group, 0x1C, storySettingsPos[board_config0->handicaps[0]][3] - x, storySettingsPos[board_config0->handicaps[0]][2]);
            }
            switch (board_config0->handicaps[0]) {
                case 1:
                    if (value_change_dir == 1) {
                        HuSprPosSet(choosecharacter_group, 0x1E, 340.0f + x, storySettingsPos[1][2]);
                        HuSprPosSet(choosecharacter_group, 0x1F, 340.0f + x, storySettingsPos[1][2] + 2);
                    }
                    else {
                        HuSprPosSet(choosecharacter_group, 0x1E, 341.0f - x, storySettingsPos[1][2]);
                        HuSprPosSet(choosecharacter_group, 0x1F, 341.0f - x, storySettingsPos[1][2] + 2);
                    }
                    HuSprScaleSet(choosecharacter_group, 0x1E, 1.0f, value_y_scale);
                    HuSprScaleSet(choosecharacter_group, 0x1F, 1.0f, value_y_scale);
                    break;
                case 2:
                    if (value_change_dir == 1) {
                        HuSprPosSet(choosecharacter_group, 0x20, 340.0f + x, storySettingsPos[2][2]);
                        HuSprPosSet(choosecharacter_group, 0x21, 340.0f + x, storySettingsPos[2][2] + 2);
                    }
                    else {
                        HuSprPosSet(choosecharacter_group, 0x20, 341.0f - x, storySettingsPos[2][2]);
                        HuSprPosSet(choosecharacter_group, 0x21, 341.0f - x, storySettingsPos[2][2] + 2);
                    }
                    HuSprScaleSet(choosecharacter_group, 0x20, 1.0f, value_y_scale);
                    HuSprScaleSet(choosecharacter_group, 0x21, 1.0f, value_y_scale);
                    break;
            }
            if (progress == 6) {
                switch (board_config0->handicaps[0]) {
                    case 1:
                        HuSprBankSet(choosecharacter_group, 0x1E, board_config0->board_settings[0]);
                        HuSprBankSet(choosecharacter_group, 0x1F, board_config0->board_settings[0]);
                        break;
                    case 2:
                        HuSprBankSet(choosecharacter_group, 0x20, board_config0->board_settings[1]);
                        HuSprBankSet(choosecharacter_group, 0x21, board_config0->board_settings[1]);
                        break;
                }
            }
        }
    }
    board_configs[0].handicaps[3] = 0;
}

void fn_1_1F5F0(void)
{
    s32 var_r31;
    s32 var_r30;

    var_r31 = board_configs[0].chosen_characters_overlay_group;
    for (var_r30 = 0; var_r30 < 9; var_r30++) {
        HuSprAttrSet(var_r31, var_r30, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrReset(var_r31, 0, HUSPR_ATTR_DISPOFF);
    HuSprBankSet(var_r31, 0, board_configs[0].board_settings[0]);
    HuSprAttrReset(var_r31, mtPlayerData->character + 1, HUSPR_ATTR_DISPOFF);
    HuSprGrpPosSet(var_r31, 16.0f, -500.0f);
}
// Not sure what this does
void createChosenCharactersOverlayGroupStory(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3)
{
    s32 i;
    s32 group_same;
    s32 group;
    s32 j;
    s32 sprite_idx;

    group = HuSprGrpCreate(9);
    game_config0->chosen_characters_overlay_group = group;
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x59)), 0x7D0, 0);
    HuSprGrpMemberSet(group, 0, sprite_idx);
    sprPosSet_YPadded(group, 0, 75.0f, 25.0f);
    for (i = 0; i < 8; i++) {
        sprite_idx = HuSprCreate(HuSprAnimReadFile(i + DATA_MAKE_NUM(DATADIR_MENT, 0x31)), 0x3E8, 0);
        HuSprGrpMemberSet(group, i + 1, sprite_idx);
        HuSprPosSet(group, i + 1, 24.0f, 25.0f);
    }
    group_same = board_configs[0].chosen_characters_overlay_group;
    for (j = 0; j < 9; j++) {
        HuSprAttrSet(group_same, j, 4);
    }
    HuSprAttrReset(group_same, 0, HUSPR_ATTR_DISPOFF);
    HuSprBankSet(group_same, 0, board_configs[0].board_settings[0]);
    HuSprAttrReset(group_same, mtPlayerData->character + 1, HUSPR_ATTR_DISPOFF);
    HuSprGrpPosSet(group_same, 16.0f, -500.0f);
}

void fn_1_1F868(void)
{
    float var_f31;
    s32 var_r31;
    s32 var_r30;
    s32 var_r29;

    var_r30 = board_configs[0].chosen_characters_overlay_group;
    for (var_r29 = 0; var_r29 < 9; var_r29++) {
        HuSprAttrSet(var_r30, var_r29, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrReset(var_r30, 0, HUSPR_ATTR_DISPOFF);
    HuSprBankSet(var_r30, 0, board_configs[0].board_settings[0]);
    HuSprAttrReset(var_r30, mtPlayerData->character + 1, HUSPR_ATTR_DISPOFF);
    HuSprGrpPosSet(var_r30, 16.0f, -500.0f);
    for (var_r31 = 0; var_r31 <= 0x3C; var_r31++) {
        MenuPrcVSleep();
        if (var_r31 <= 0x32) {
            var_f31 = SinEaseClamped(-500.0f, 50.0f, var_r31, 50.0f);
        }
        else {
            var_f31 = CosEaseClamped(50.0f, 40.0f, var_r31 - 0x32, 10.0f);
        }
        HuSprGrpPosSet(board_configs[0].chosen_characters_overlay_group, 16.0f, var_f31);
    }
}

void fn_1_1FA34(void)
{
    float var_f31;
    s32 var_r31;

    board_configs[0].handicaps[0] = -1;
    board_configs[0].obj_callback = NULL;
    for (var_r31 = 0; var_r31 <= 0x4B; var_r31++) {
        MenuPrcVSleep();
        if (var_r31 <= 0xA) {
            var_f31 = SinEaseClamped(40.0f, 50.0f, var_r31, 10.0f);
        }
        else {
            var_f31 = CosEaseClamped(50.0f, -500.0f, var_r31 - 0xA, 50.0f);
        }
        HuSprGrpPosSet(board_configs[0].chosen_characters_overlay_group, 16.0f, var_f31);
        if (var_r31 >= 0xF) {
            var_f31 = CosEaseClamped(210.0f, -500.0f, var_r31 - 0xF, 60.0f);
            HuSprGrpData[board_configs[0].chooseboard_group].pos.y = (s32)WeightedBlend(HuSprGrpData[board_configs[0].chooseboard_group].pos.y, var_f31, 10.0f);
        }
    }
}

void fn_1_1FC54(void)
{
    float var_f31;
    s32 var_r31;

    board_configs[0].handicaps[0] = -1;
    board_configs[0].obj_callback = NULL;
    for (var_r31 = 0; var_r31 <= 0x28; var_r31++) {
        MenuPrcVSleep();
        var_f31 = CosEaseClamped(40.0f, -500.0f, var_r31, 40.0f);
        HuSprGrpPosSet(board_configs[0].chosen_characters_overlay_group, 16.0f, var_f31);
        var_f31 = CosEaseClamped(210.0f, -500.0f, var_r31, 40.0f);
        HuSprGrpData[board_configs[0].chooseboard_group].pos.y = (s32)WeightedBlend(HuSprGrpData[board_configs[0].chooseboard_group].pos.y, var_f31, 10.0f);
    }
}

void fn_1_1FE08(void)
{
    s32 sp8;
    s32 var_r31;
    s32 var_r30;

    sp8 = 0;
    var_r30 = board_configs[0].humancount_group;
    for (var_r31 = 0; var_r31 < 0x13; var_r31++) {
        HuSprAttrSet(var_r30, var_r31, 4);
    }
    for (var_r31 = 0; var_r31 < 4; var_r31++) {
        HuSprBankSet(var_r30, var_r31, 0);
        mtPlayerData[var_r31].unk_70[3] = mtPlayerData[var_r31].iscom = 0;
        HuSprAttrReset(var_r30, var_r31, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(var_r30, var_r31 + 4, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrReset(var_r30, 0xA, HUSPR_ATTR_DISPOFF);
    for (var_r31 = 0; var_r31 < 4; var_r31++) {
        HuSprAttrReset(var_r30, var_r31 + 0xB, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(var_r30, var_r31 + 0xF, HUSPR_ATTR_DISPOFF);
    }
    HuSprGrpPosSet(var_r30, 0.0f, -500.0f);
}

void fn_1_1FF4C(OMOBJ *arg0, MentBoardMenuConfig *arg1)
{
    s32 sp8;
    float var_f31;
    float var_f30;
    s32 var_r31;
    s32 var_r30;
    s32 var_r29;
    s32 var_r28;
    s32 var_r27;
    s32 var_r26;
    s32 var_r25;

    sp8 = 0;
    var_r28 = board_configs[0].humancount_group;
    var_r29 = var_r27 = var_r26 = var_r25 = 0;
    board_configs[0].board_settings[0] = 0;
    var_r29 = 1;
    for (var_r30 = 1; var_r30 < 4; var_r30++) {
        if (mtPlayerData[var_r30].iscom == 0) {
            var_r29++;
        }
    }
    var_r27 = 1;
    for (var_r30 = 1; var_r30 < 4; var_r30++) {
        if (HuPadStatGet(var_r30) != -1) {
            var_r27++;
        }
    }
    if (var_r27 >= 2) {
        fn_1_208F4();
        if ((HuPadStkX[mtPlayerData->pad_idx] >= 5) || ((HuPadBtn[mtPlayerData->pad_idx] & 2) != 0)) {
            var_r29++;
            var_r26 = 1;
        }
        else if ((HuPadStkX[mtPlayerData->pad_idx] <= -5) || ((HuPadBtn[mtPlayerData->pad_idx] & 1) != 0)) {
            var_r29--;
            var_r26 = -1;
        }
    }
    else {
        fn_1_20A24();
    }
    if (var_r29 < 1) {
        var_r29 = var_r27;
    }
    else if (var_r29 > var_r27) {
        if (var_r26 == 0) {
            var_r29 = var_r27;
            var_r25 = 1;
        }
        else {
            var_r29 = 1;
        }
    }
    if ((var_r26 != 0) || (var_r25 != 0)) {
        board_configs[0].board_settings[0] = 1;
        HuAudFXPlay(0);
        for (var_r30 = 0; var_r30 <= 0xA; var_r30++) {
            if (var_r30 <= 5) {
                var_f30 = SinEaseClamped(0.0f, 10.0f, var_r30, 5.0f);
                var_f31 = SinEaseClamped(1.0f, 0.0f, var_r30, 5.0f);
            }
            else {
                var_f30 = SinEaseClamped(10.0f, 0.0f, var_r30 - 5, 5.0f);
                var_f31 = SinEaseClamped(0.0f, 1.0f, var_r30 - 5, 5.0f);
            }
            if (var_r26 == 1) {
                HuSprPosSet(var_r28, 9, (chooseHumanCountPos[3][0] + 0x32) + var_f30, chooseHumanCountPos[0][1]);
            }
            else if (var_r26 == -1) {
                HuSprPosSet(var_r28, 8, (chooseHumanCountPos[0][0] - 0x32) - var_f30, chooseHumanCountPos[0][1]);
            }
            for (var_r31 = 0; var_r31 < 4; var_r31++) {
                if ((var_r31 < var_r29) && (mtPlayerData[var_r31].iscom == 1)) {
                    mtPlayerData[var_r31].unk_70[3] = 0;
                    HuSprScaleSet(var_r28, var_r31, var_f31, 1.0f);
                    HuSprScaleSet(var_r28, var_r31 + 4, var_f31, 1.0f);
                    if (var_r30 == 5) {
                        HuSprBankSet(var_r28, var_r31, 0);
                    }
                }
                else if ((var_r31 >= var_r29) && (mtPlayerData[var_r31].iscom == 0)) {
                    mtPlayerData[var_r31].unk_70[3] = 1;
                    HuSprScaleSet(var_r28, var_r31, var_f31, 1.0f);
                    HuSprScaleSet(var_r28, var_r31 + 4, var_f31, 1.0f);
                    if (var_r30 == 5) {
                        HuSprBankSet(var_r28, var_r31, 1);
                    }
                }
            }
            MenuPrcVSleep();
        }
        for (var_r31 = 0; var_r31 < 4; var_r31++) {
            if (var_r31 < var_r29) {
                mtPlayerData[var_r31].iscom = 0;
            }
            else {
                mtPlayerData[var_r31].iscom = 1;
            }
        }
    }
    board_configs[0].board_settings[0] = 0;
}

void moveHumancountGroup(s32 arg0)
{
    s32 spC;
    float y;
    s32 i;
    s32 group;
    s32 progress;
    s32 group_same;

    group_same = board_configs[0].humancount_group;
    if (arg0 == 0) {
        spC = 0;
        group = board_configs[0].humancount_group;
        for (i = 0; i < 0x13; i++) {
            HuSprAttrSet(group, i, HUSPR_ATTR_DISPOFF);
        }
        for (i = 0; i < 4; i++) {
            HuSprBankSet(group, i, 0);
            mtPlayerData[i].unk_70[3] = mtPlayerData[i].iscom = 0;
            HuSprAttrReset(group, i, HUSPR_ATTR_DISPOFF);
            HuSprAttrReset(group, i + 4, HUSPR_ATTR_DISPOFF);
        }
        HuSprAttrReset(group, 0xA, HUSPR_ATTR_DISPOFF);
        for (i = 0; i < 4; i++) {
            HuSprAttrReset(group, i + 0xB, HUSPR_ATTR_DISPOFF);
            HuSprAttrReset(group, i + 0xF, HUSPR_ATTR_DISPOFF);
        }
        HuSprGrpPosSet(group, 0.0f, -500.0f);
    }
    for (progress = 0; progress <= 0x3C; progress++) {
        MenuPrcVSleep();
        if (progress <= 0x32) {
            y = SinEaseClamped(-500.0f, 10.0f, progress, 50.0f);
        }
        else {
            y = CosEaseClamped(10.0f, 2.06f, progress - 0x32, 10.0f);
        }
        HuSprGrpPosSet(group_same, 0.0f, y);
    }
}

void human_count_menu_exit(void)
{
    s32 spC;
    s32 sp8;
    float y;
    s32 i;
    s32 group;

    spC = 0;
    sp8 = 0;
    group = board_configs[0].humancount_group;
    for (i = 0; i <= 0x3C; i++) {
        MenuPrcVSleep();
        if (i <= 0xA) {
            y = SinEaseClamped(2.06f, 10.0f, i, 10.0f);
        }
        else {
            y = CosEaseClamped(10.0f, -500.0f, i - 0xA, 50.0f);
        }
        HuSprGrpPosSet(group, 0.0f, y);
    }
}

void fn_1_208F4(void)
{
    s32 var_r31;

    var_r31 = board_configs[0].humancount_group;
    HuSprAttrReset(var_r31, 8, HUSPR_ATTR_DISPOFF);
    HuSprPosSet(var_r31, 8, chooseHumanCountPos[0][0] - 0x32, chooseHumanCountPos[0][1]);
    HuSprAttrReset(var_r31, 9, HUSPR_ATTR_DISPOFF);
    HuSprPosSet(var_r31, 9, chooseHumanCountPos[3][0] + 0x32, chooseHumanCountPos[0][1]);
}

void fn_1_20A24(void)
{
    s32 var_r31;

    var_r31 = board_configs[0].humancount_group;
    HuSprAttrSet(var_r31, 8, 4);
    HuSprPosSet(var_r31, 8, chooseHumanCountPos[0][0] - 0x32, chooseHumanCountPos[0][1]);
    HuSprAttrSet(var_r31, 9, 4);
    HuSprPosSet(var_r31, 9, chooseHumanCountPos[3][0] + 0x32, chooseHumanCountPos[0][1]);
}

// first arg will always be game_configs[0]
void createChooseHumanCountGroupParty(MentBoardMenuConfig *game_config0, s32 arg1, s32 arg2, s32 arg3)
{
    s32 sp8;
    s32 i;
    s32 j;
    s32 human_count_group;
    s32 sprite_idx;
    s32 group_same;  // refers to the same group as s32 group
    ANIMDATA *spr_animation_data1;
    ANIMDATA *spr_animation_data2;

    human_count_group = HuSprGrpCreate(0x13);
    game_config0->humancount_group = human_count_group;
    spr_animation_data1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x22));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x16)), 0x3E8, 0);
        HuSprGrpMemberSet(human_count_group, i, sprite_idx);
        HuSprPosSet(human_count_group, i, chooseHumanCountPos[i % 4][0], chooseHumanCountPos[0][1]);
        sprite_idx = HuSprCreate(spr_animation_data1, 0x3F2, 0);
        HuSprGrpMemberSet(human_count_group, i + 4, sprite_idx);
        HuSprTPLvlSet(human_count_group, i + 4, 0.5f);
        HuSprPosSet(human_count_group, i + 4, chooseHumanCountPos[i % 4][0] + 1, chooseHumanCountPos[0][1] + 2);
    }
    spr_animation_data1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x42));
    for (i = 0; i < 2; i++) {
        sprite_idx = HuSprCreate(spr_animation_data1, 0, 0);
        HuSprGrpMemberSet(human_count_group, i + 8, sprite_idx);
        HuSprBankSet(human_count_group, i + 8, i);
    }
    sprite_idx = HuSprCreate(HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x15)), 0x7D0, 0);
    HuSprGrpMemberSet(human_count_group, 0xA, sprite_idx);
    HuSprTPLvlSet(human_count_group, 0xA, 0.8f);
    sprPosSet_YPadded(human_count_group, 0xA, 288.0f, 240.0f);
    spr_animation_data1 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x18));
    spr_animation_data2 = HuSprAnimReadFile(DATA_MAKE_NUM(DATADIR_MENT, 0x19));
    for (i = 0; i < 4; i++) {
        sprite_idx = HuSprCreate(spr_animation_data1, 0x7BC, 0);
        HuSprGrpMemberSet(human_count_group, i + 0xB, sprite_idx);
        HuSprBankSet(human_count_group, i + 0xB, i);
        sprPosSet_YPadded(human_count_group, i + 0xB, chooseHumanCountPos[i][2], chooseHumanCountPos[i][3]);
        sprite_idx = HuSprCreate(spr_animation_data2, 0x7C6, 0);
        HuSprGrpMemberSet(human_count_group, i + 0xF, sprite_idx);
        HuSprBankSet(human_count_group, i + 0xF, i);
        HuSprTPLvlSet(human_count_group, i + 0xF, 0.5f);
        sprPosSet_YPadded(human_count_group, i + 0xF, chooseHumanCountPos[i][2] + 1, chooseHumanCountPos[i][3] + 6);
    }
    sp8 = 0;
    group_same = board_configs[0].humancount_group;
    for (j = 0; j < 0x13; j++) {
        HuSprAttrSet(group_same, j, HUSPR_ATTR_DISPOFF);
    }
    for (j = 0; j < 4; j++) {
        HuSprBankSet(group_same, j, 0);
        mtPlayerData[j].unk_70[3] = mtPlayerData[j].iscom = 0;
        HuSprAttrReset(group_same, j, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(group_same, j + 4, HUSPR_ATTR_DISPOFF);
    }
    HuSprAttrReset(group_same, 0xA, HUSPR_ATTR_DISPOFF);
    for (j = 0; j < 4; j++) {
        HuSprAttrReset(group_same, j + 0xB, HUSPR_ATTR_DISPOFF);
        HuSprAttrReset(group_same, j + 0xF, HUSPR_ATTR_DISPOFF);
    }
    HuSprGrpPosSet(group_same, 0.0f, -500.0f);
}

void MenuMoveBubbles(OMOBJ *object)
{
    float y;
    s32 i;
    MenuCamera *menu_camera_ref;

    menu_camera_ref = &menuCamera;
    for (i = 1; i < 0xA; i++) {
        if ((menu_camera_ref->zoom <= 1200.0f) || (bubbles[i].unk_1C == -200.0f)) {
            if (bubbles[i].unk_1C >= -100.0f) {
                bubbles[i].unk_00 = 0;
                bubbles[i].unk_1C = -200.0f;
                bubbles[i].unk_24 = -99.0f;
                bubbles[i].unk_04 = rand8() + 0xB4;
            }
            y = CosEaseClamped(bubbles[i].unk_10, 1000.0f, bubbles[i].unk_00++, bubbles[i].unk_04);
            Hu3DData[object->mdlId[i]].pos.y = WeightedBlend(Hu3DData[object->mdlId[i]].pos.y, y, 30.0f);
            Hu3DData[object->mdlId[i]].pos.x
                = WeightedBlend(Hu3DData[object->mdlId[i]].pos.x, bubbles[i].unk_18 + Hu3DData[object->mdlId[i]].pos.x, 5.0f);
        }
    }
    for (i = 0xA; i < 0x8C; i++) {
        Hu3DData[object->mdlId[i]].pos.y = SinOscillateClamped(
            bubbles[i].unk_10, bubbles[i].unk_1C, bubbles[i].unk_00++, bubbles[i].unk_04);
        if (bubbles[i].unk_00 >= bubbles[i].unk_04) {
            bubbles[i].unk_00 = 0;
        }
    }
    for (i = 0xA; i < 0x8C; i++) {
        if ((bubbles[i].unk_24 == 0.0f) && (bubbles[i].unk_14 > (menu_camera_ref->zoom - 400.0f))
            && (bubbles[i].unk_0C < (200.0f + menu_camera_ref->center.x)) && (bubbles[i].unk_0C > (menu_camera_ref->center.x - 200.0f))) {
            if (menu_camera_ref->center.x >= bubbles[i].unk_0C) {
                bubbles[i].unk_24 = -300.0f;
            }
            else {
                bubbles[i].unk_24 = 300.0f;
            }
        }
        if (bubbles[i].unk_24 != -99.0f) {
            Hu3DData[object->mdlId[i]].pos.x
                = WeightedBlend(Hu3DData[object->mdlId[i]].pos.x, bubbles[i].unk_0C + bubbles[i].unk_24, 30.0f);
        }
    }
    if (object->work[0] == 1) {
        for (i = 0xA; i < 0x8C; i++) {
            Hu3DModelAttrSet(object->mdlId[i], HU3D_ATTR_DISPOFF);
        }
    }
}

void MenuCreateBubbles(OMOBJ *object)
{
    float x;
    float y;
    float z;
    s32 i;
    s32 bubbleIdx;
    s32 total_iterations;

    object->mdlId[1] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x08));
    object->mdlId[2] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x09));
    object->mdlId[3] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x0A));
    object->mdlId[4] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 0x0B));
    Hu3DModelLayerSet(object->mdlId[1], 3);
    Hu3DModelLayerSet(object->mdlId[2], 3);
    Hu3DModelLayerSet(object->mdlId[3], 3);
    Hu3DModelLayerSet(object->mdlId[4], 3);
    for (i = 5; i < 0x8C; i++) {
        object->mdlId[i] = Hu3DModelLink(object->mdlId[rand8() % 4 + 1]);
    }
    for (i = 1; i < 0xA; i++) {
        total_iterations = 0;
        do {
            Hu3DData[object->mdlId[i]].pos.x = x = 50.0f + rand8();
            if ((i % 2) != 0) {
                bubbles[i].unk_0C = Hu3DData[object->mdlId[i]].pos.x *= -1.0f;
            }
            Hu3DData[object->mdlId[i]].pos.y = y = 50.0f + rand8();
            Hu3DData[object->mdlId[i]].pos.z = z = 750.0f + (i * 5);
            for (bubbleIdx = 1; bubbleIdx < 0xA; bubbleIdx++) {
                if ((i != bubbleIdx) && (bubbles[bubbleIdx].unk_08 == 1)) {
                    x = Hu3DData[object->mdlId[i]].pos.x - Hu3DData[object->mdlId[bubbleIdx]].pos.x;
                    y = Hu3DData[object->mdlId[i]].pos.y - Hu3DData[object->mdlId[bubbleIdx]].pos.y;
                    z = Hu3DData[object->mdlId[i]].pos.z - Hu3DData[object->mdlId[bubbleIdx]].pos.z;
                    if (sqrtf((x * x) + (y * y)) <= 100.0f) {
                        break;
                    }
                }
            }
            total_iterations++;
        } while (bubbleIdx != 0xA && total_iterations < 0x32);
        bubbles[i].unk_08 = 1;
        bubbles[i].unk_0C = bubbles[i].unk_18 = Hu3DData[object->mdlId[i]].pos.x;
        bubbles[i].unk_10 = bubbles[i].unk_1C = Hu3DData[object->mdlId[i]].pos.y;
        bubbles[i].unk_14 = bubbles[i].unk_20 = Hu3DData[object->mdlId[i]].pos.z;
        bubbles[i].unk_18 = 0.025f * bubbles[i].unk_0C;
        Hu3DModelScaleSet(object->mdlId[i], 0.5f, 0.5f, 0.5f);
        Hu3DModelLayerSet(object->mdlId[i], 3);
    }
    for (i = 0xA; i < 0x8C; i++) {
        total_iterations = 0;
        do {
            Hu3DData[object->mdlId[i]].pos.x = x = rand8() * 2;
            if ((i % 2) != 0) {
                Hu3DData[object->mdlId[i]].pos.x *= -1.0f;
            }

            Hu3DData[object->mdlId[i]].pos.y = y = (rand8() * 2) + 0x7D;
            Hu3DData[object->mdlId[i]].pos.z = z = 800.0f + (i * 0xA);
            for (bubbleIdx = 0xA; bubbleIdx < 0x8C; bubbleIdx++) {
                if ((i != bubbleIdx) && (bubbles[bubbleIdx].unk_08 == 1)) {
                    x = Hu3DData[object->mdlId[i]].pos.x - Hu3DData[object->mdlId[bubbleIdx]].pos.x;
                    y = Hu3DData[object->mdlId[i]].pos.y - Hu3DData[object->mdlId[bubbleIdx]].pos.y;
                    z = Hu3DData[object->mdlId[i]].pos.z - Hu3DData[object->mdlId[bubbleIdx]].pos.z;
                    if (sqrtf((z * z) + ((x * x) + (y * y))) <= 100.0f) {
                        break;
                    }
                }
            }
            total_iterations++;
        } while (bubbleIdx != 0x8C && total_iterations < 0x32);

        bubbles[i].unk_08 = 1;
        Hu3DModelLayerSet(object->mdlId[i], 3);
        bubbles[i].unk_0C = bubbles[i].unk_18 = Hu3DData[object->mdlId[i]].pos.x;
        bubbles[i].unk_10 = bubbles[i].unk_1C = Hu3DData[object->mdlId[i]].pos.y;
        bubbles[i].unk_14 = bubbles[i].unk_20 = Hu3DData[object->mdlId[i]].pos.z;
        if ((rand8() % 2) == 0) {
            bubbles[i].unk_18 += rand8() % 30;
        }
        else {
            bubbles[i].unk_18 -= rand8() % 30;
        }
        bubbles[i].unk_1C += (rand8() % 30) + 0xA;
        if ((rand8() % 2) == 0) {
            bubbles[i].unk_20 += rand8() % 30;
        }
        else {
            bubbles[i].unk_20 -= rand8() % 30;
        }
        bubbles[i].unk_24 = bubbles[i].unk_28 = bubbles[i].unk_2C = 0.0f;
        bubbles[i].unk_04 = ((rand8() * 2) % 360) + 0x168;
        bubbles[i].unk_00 = rand8();
    }
    object->objFunc = MenuMoveBubbles;
}

void dropBirthdayConfetti(OMOBJ *object)
{
    s32 i;

    if (object->work[0] == 0) {
        return;
    }
    for (i = 0; i < 0xC8; i++) {
        switch (confetti[i].state) {
            case 0:
                Hu3DData[object->mdlId[i]].pos.x = rand8() + rand8() % 50;
                if ((i % 2) == 0) {
                    Hu3DData[object->mdlId[i]].pos.x *= -1.0f;
                }
                Hu3DData[object->mdlId[i]].pos.y = 500.0f + rand8() + rand8();
                Hu3DData[object->mdlId[i]].pos.z = 700.0f + rand8() % 128;
                confetti[i].ground_timer = 1.0f;
                confetti[i].air_change_dir_timer = (rand8() % 30) + 0x1E;
                confetti[i].x_speed = 0.01f * (25.0f + (rand8() % 50));
                if ((rand8() % 2) == 0) {
                    confetti[i].x_speed *= -1.0f;
                }
                confetti[i].land_rot_dir = rand8() % 2;
                confetti[i].z_speed = 0.01f * ((rand8() % 100) - 0x32);
                confetti[i].rot_speed = (rand8() % 5) + 5;
                if ((rand8() % 2) == 0) {
                    confetti[i].rot_speed *= -1;
                }
                confetti[i].state = 1;
                break;
            case 1:
                if (Hu3DData[object->mdlId[i]].pos.y <= 0.5f) {
                    Hu3DData[object->mdlId[i]].pos.y = 0.5f;
                    if (confetti[i].land_rot_dir > 0.0f) {
                        Hu3DData[object->mdlId[i]].rot.x = WeightedBlend(Hu3DData[object->mdlId[i]].rot.x, 180.0f, 5.0f);
                    }
                    else {
                        Hu3DData[object->mdlId[i]].rot.x = WeightedBlend(Hu3DData[object->mdlId[i]].rot.x, 0.0f, 5.0f);
                    }
                    Hu3DData[object->mdlId[i]].rot.z = WeightedBlend(Hu3DData[object->mdlId[i]].rot.z, 0.0f, 5.0f);
                    confetti[i].ground_timer -= 0.025f;
                    if (confetti[i].ground_timer <= 0.0f) {
                        confetti[i].ground_timer = 0.0f;
                        confetti[i].state = 0;
                        if (object->work[0] == 3) {
                            confetti[i].state = 2;
                        }
                    }
                }
                else {
                    if (confetti[i].air_change_dir_timer-- <= 0) {
                        confetti[i].air_change_dir_timer = (rand8() % 30) + 0x1E;
                        confetti[i].x_speed = 0.01f * (25.0f + (rand8() % 50));
                        if ((rand8() % 2) == 0) {
                            confetti[i].x_speed *= -1.0f;
                        }
                        confetti[i].land_rot_dir = rand8() % 2;
                        confetti[i].z_speed = 0.01f * ((rand8() % 100) - 0x32);
                        confetti[i].rot_speed = (rand8() % 5) + 5;
                        if ((rand8() % 2) == 0) {
                            confetti[i].rot_speed *= -1;
                        }
                    }
                    Hu3DModelAttrReset(object->mdlId[i], HU3D_ATTR_DISPOFF);
                    Hu3DData[object->mdlId[i]].pos.x += confetti[i].x_speed;
                    Hu3DData[object->mdlId[i]].pos.y -= (i % 3) + 2;
                    Hu3DData[object->mdlId[i]].pos.z += confetti[i].z_speed;
                    Hu3DData[object->mdlId[i]].rot.x += confetti[i].rot_speed;
                    Hu3DData[object->mdlId[i]].rot.y += confetti[i].rot_speed;
                    Hu3DData[object->mdlId[i]].rot.z += confetti[i].rot_speed;
                    if (Hu3DData[object->mdlId[i]].rot.x >= 360.0f) {
                        Hu3DData[object->mdlId[i]].rot.x -= 360.0f;
                    }
                    else if (Hu3DData[object->mdlId[i]].rot.x <= 0.0f) {
                        Hu3DData[object->mdlId[i]].rot.x += 360.0f;
                    }
                    if (Hu3DData[object->mdlId[i]].rot.y >= 360.0f) {
                        Hu3DData[object->mdlId[i]].rot.y -= 360.0f;
                    }
                    else if (Hu3DData[object->mdlId[i]].rot.y <= 0.0f) {
                        Hu3DData[object->mdlId[i]].rot.y += 360.0f;
                    }
                    if (Hu3DData[object->mdlId[i]].rot.z >= 360.0f) {
                        Hu3DData[object->mdlId[i]].rot.z -= 360.0f;
                    }
                    else if (Hu3DData[object->mdlId[i]].rot.z <= 0.0f) {
                        Hu3DData[object->mdlId[i]].rot.z += 360.0f;
                    }
                }
                break;
            case 2:
                Hu3DModelAttrSet(object->mdlId[i], HU3D_ATTR_DISPOFF);
                break;
        }
    }
}

void createBirthdayConfetti(OMOBJ *object)
{
    float var_f30; // unused
    float confetti_size;
    s32 i;
    s32 confetti_color;
    s32 var_r28; // unused

    object->mdlId[0] = Hu3DModelCreateFile(DATA_MAKE_NUM(DATADIR_MENT, 12));
    for (i = 0; i < 0xC8; i++) {
        object->mdlId[i] = Hu3DModelLink(object->mdlId[0]);
    }
    for (i = 0; i < 0xC8; i++) {
        Hu3DModelAttrSet(object->mdlId[i], HU3D_ATTR_DISPOFF);
        Hu3DData[object->mdlId[i]].pos.x = rand8() + rand8();
        if (rand8() == 0) {
            Hu3DData[object->mdlId[i]].pos.x *= -1.0f;
        }
        Hu3DData[object->mdlId[i]].pos.y = 500.0f + rand8() + rand8();
        Hu3DData[object->mdlId[i]].pos.z = 700.0f + (rand8() % 128);
        confetti_size = 1.0f + (0.1f * (rand8() % 5));
        Hu3DModelScaleSet(object->mdlId[i], confetti_size, confetti_size, confetti_size);
        confetti[i].x_speed = 0.01f * (25.0f + (rand8() % 50));
        if ((rand8() % 2) == 0) {
            confetti[i].x_speed *= -1.0f;
        }
        confetti[i].land_rot_dir = rand8() % 2;
        confetti[i].z_speed = 0.01f * ((rand8() % 100) - 0x32);
        confetti[i].rot_speed = (rand8() % 5) + 5;
        if ((rand8() % 2) == 0) {
            confetti[i].rot_speed *= -1;
        }
        confetti_color = rand8() % 4;
        switch (confetti_color) {
            case 0:
                Hu3DModelAmbSet(object->mdlId[i], 1.0f, 1.0f, 1.0f);
                break;
            case 1:
                Hu3DModelAmbSet(object->mdlId[i], 1.0f, 0.0f, 0.0f);
                break;
            case 2:
                Hu3DModelAmbSet(object->mdlId[i], 0.0f, 1.0f, 0.0f);
                break;
            case 3:
                Hu3DModelAmbSet(object->mdlId[i], 0.0f, 0.0f, 1.0f);
                break;
        }
        confetti[i].state = 0;
    }
    object->objFunc = dropBirthdayConfetti;
}
