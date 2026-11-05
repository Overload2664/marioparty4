#ifndef _BOARD_SHOP_H
#define _BOARD_SHOP_H

#include "dolphin/types.h"
#include "game/board/item.h"
#include "game/board/player.h"

void BoardShopHostSet(s16 arg0);
s16 BoardShopHostGet(void);
void BoardShopInit(void);
void BoardShopKill(void);
void BoardShopExec(s32 player, s32 space);
void BoardShopTutorialExec(s32 arg0);

// Extra
#define MAX_SHOP_ITEMS 8
#define MESS_SMALL_SPACE "\20"
// 0B 1E 02 XX XX XX XX XX XX XX XX XX 1E 08 0C 0C 0C 0C 0C 0C 0C 0E 13 3E YY YY 00
static const char *ITEM_BANK_NAMES[] = {
    "Mini" MESS_SMALL_SPACE "Mushroom",
    "Mega" MESS_SMALL_SPACE "Mushroom",
    "Super" MESS_SMALL_SPACE "Mini" MESS_SMALL_SPACE "Mushroom",
    "Super" MESS_SMALL_SPACE "Mega" MESS_SMALL_SPACE "Mushroom",
    "MiniMega" MESS_SMALL_SPACE "Hammer",
    "Warp" MESS_SMALL_SPACE "Pipe",
    "Swap" MESS_SMALL_SPACE "Card",
    "Sparky" MESS_SMALL_SPACE "Sticker",
    "Gaddlight",
    "Chomp" MESS_SMALL_SPACE "Call",
    "Bowser" MESS_SMALL_SPACE "Suit",
    "Boo's" MESS_SMALL_SPACE "Crystal" MESS_SMALL_SPACE "Ball",
    "Magic" MESS_SMALL_SPACE "Lamp",
    "Item" MESS_SMALL_SPACE "Bag",
    "Bowser" MESS_SMALL_SPACE "Phone",
    "Mushroom",
    "Golden" MESS_SMALL_SPACE "Mushroom",
};

static u32 ITEM_SHOP_PROBS_DEFAULT[BOARD_ITEMS_END] = {
    0x14, 0x14, 0x0F, 0x0F, 0x05,
    0x07, 0x05, 0x05, 0x00, 0x05,
    0x00, 0x03, 0x00, 0x00, 0x14,
    0x0F, 0x14
};

static u32 ITEM_SPACE_PROBS_DEFAULT[BOARD_ITEMS_END] = {
    0x4, 0x14, 0x0F, 0x0F, 0x05,
    0x07, 0x05, 0x05, 0x00, 0x05,
    0x00, 0x03, 0x00, 0x00, 0x20,
    0x0F, 0x14
};

void GetShopItemsEx(u32 itemSize, u32* probs, s32* activeItemTbl);

#endif
