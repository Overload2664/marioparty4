#ifndef _BOARD_START_H
#define _BOARD_START_H

#include "dolphin/types.h"

extern s8 playerOrderNew[4];

void BoardStartExec(void);
// Set playerOrderNew then apply OrderPlayers
void OrderPlayers(void);

#endif
