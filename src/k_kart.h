// SONIC ROBO BLAST 2 KART ~ ZarroTsu
//-----------------------------------------------------------------------------
/// \file  k_kart.h
/// \brief SRB2kart stuff.

#ifndef __K_KART__
#define __K_KART__

#include "doomdef.h"
#include "d_player.h" // Need for player_t

UINT8 colortranslations[MAXSKINCOLORS][16];
extern const char *KartColor_Names[MAXSKINCOLORS];
void K_StarmanColormap(UINT8 *dest_colormap, UINT8 skincolor);
void K_GenerateKartColormap(UINT8 *dest_colormap, INT32 skinnum, UINT8 color);
UINT8 K_GetKartColorByName(const char *name);

void K_RegisterKartStuff(void);

void K_KartBouncing(mobj_t *mobj1, mobj_t *mobj2, boolean bounce, boolean solid);
void K_LakituChecker(player_t *player);
void K_KartMoveAnimation(player_t *player);
void K_KartPlayerThink(player_t *player, ticcmd_t *cmd);
void K_KartPlayerAfterThink(player_t *player);
void K_SpinPlayer(player_t *player, mobj_t *source);
void K_SquishPlayer(player_t *player, mobj_t *source);
void K_ExplodePlayer(player_t *player, mobj_t *source);
void K_StealBalloon(player_t *player, player_t *victim, boolean force);
void K_SpawnKartExplosion(fixed_t x, fixed_t y, fixed_t z, fixed_t radius, INT32 number, mobjtype_t type, angle_t rotangle, boolean spawncenter, boolean ghostit, mobj_t *source);
void K_SpawnBobombExplosion(mobj_t *source);
void K_SpawnDriftTrail(player_t *player);
void K_DoMushroom(player_t *player, boolean doPFlag, boolean startboost);
void K_DoBouncePad(mobj_t *mo, fixed_t vertispeed);
boolean K_CheckPlayersRespawnColliding(INT32 playernum, fixed_t x, fixed_t y);
INT16 K_GetKartTurnValue(player_t *player, INT16 turnvalue);
void K_MomentumToFacing(player_t *player);
fixed_t K_GetKartSpeed(player_t *player, boolean doboostpower);
fixed_t K_GetKartAccel(player_t *player);
UINT16 K_GetKartFlashing(void);
fixed_t K_3dKartMovement(player_t *player, boolean onground, fixed_t forwardmove);
void K_MoveKartPlayer(player_t *player, boolean onground);
void K_CheckBalloons(void);

INT32 K_calcSplitFlags(INT32 snapflags);
void K_LoadKartHUDGraphics(void);
fixed_t K_FindCheckX(fixed_t px, fixed_t py, angle_t ang, fixed_t mx, fixed_t my);
void K_drawKartHUD(void);
void K_LoadIconGraphics(char *facestr, INT32 skinnum);
void K_ReloadSkinIconGraphics(void);

// =========================================================================
#endif  // __K_KART__
