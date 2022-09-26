// SONIC ROBO BLAST 2
//-----------------------------------------------------------------------------
// Copyright (C) 1993-1996 by id Software, Inc.
// Copyright (C) 1998-2000 by DooM Legacy Team.
// Copyright (C) 2012-2016 by Matthew "Kaito Sinclaire" Walsh.
// Copyright (C) 1999-2020 by Sonic Team Junior.
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  m_random.c
/// \brief RNG for client effects and PRNG for game actions

#include "doomdef.h"
#include "doomtype.h"
#include "doomstat.h" // totalplaytime

#include "m_random.h"
#include "m_fixed.h"



// ---------------------------
// RNG functions (not synched)
// ---------------------------

/** Provides a random fixed point number. Distribution is uniform.
  * As with all M_Random functions, not synched in netgames.
  *
  * \return A random fixed point number from [0,1).
  */
fixed_t M_RandomFixed(void)
{
#if RAND_MAX < 65535
	// Compensate for insufficient randomness.
	fixed_t rndv = (rand()&1)<<15;
	return rand()^rndv;
#else
	return (rand() & 0xFFFF);
#endif
}

/** Provides a random byte. Distribution is uniform.
  * As with all M_Random functions, not synched in netgames.
  *
  * \return A random integer from [0, 255].
  */
UINT8 M_RandomByte(void)
{
	return (rand() & 0xFF);
}

/** Provides a random integer for picking random elements from an array.
  * Distribution is uniform.
  * As with all M_Random functions, not synched in netgames.
  *
  * \param a Number of items in array.
  * \return A random integer from [0,a).
  */
INT32 M_RandomKey(INT32 a)
{
	return (INT32)((rand()/((unsigned)RAND_MAX+1.0f))*a);
}

/** Provides a random integer in a given range.
  * Distribution is uniform.
  * As with all M_Random functions, not synched in netgames.
  *
  * \param a Lower bound.
  * \param b Upper bound.
  * \return A random integer from [a,b].
  */
INT32 M_RandomRange(INT32 a, INT32 b)
{
	return (INT32)((rand()/((unsigned)RAND_MAX+1.0f))*(b-a+1))+a;
}



// ------------------------
// PRNG functions (synched)
// ------------------------

#define DEFAULT_SEED (0xBADE4404)

typedef struct
{
	UINT32 seed[PRNUMCLASS]; // Holds each block's current seed.
	UINT32 init[PRNUMCLASS]; // Holds the INITIAL seed value. Used for demos, possibly other debugging
} rng_t;

static rng_t rng; // The entire PRNG state

/** Provides a random fixed point number.
  * This is a variant of an xorshift PRNG; state fits in a 32 bit integer structure.
  *
  * \return A random fixed point number from [0,1).
  */
ATTRINLINE static fixed_t FUNCINLINE __internal_prng__(pr_class_t pr_class)
{
	rng.seed[pr_class] ^= rng.seed[pr_class] >> 13;
	rng.seed[pr_class] ^= rng.seed[pr_class] >> 11;
	rng.seed[pr_class] ^= rng.seed[pr_class] << 21;
	return ( (rng.seed[pr_class] * 36548569) >> 4) & (FRACUNIT-1);
}

/** Provides a random fixed point number. Distribution is uniform.
  * Literally a wrapper for the internal PRNG function.
  *
  * \return A random fixed point number from [0,1).
  */
#ifndef DEBUGRANDOM
fixed_t P_RandomFixed(pr_class_t pr_class)
{
#else
fixed_t P_RandomFixedD(const char *rfile, INT32 rline, pr_class_t pr_class)
{
	CONS_Printf("P_RandomFixed(%u) at: %sp %d\n", pr_class, rfile, rline);
#endif
	return __internal_prng__(pr_class);
}

/** Provides a random byte. Distribution is uniform.
  * If you're curious, (&0xFF00) >> 8 gives the same result
  * as a fixed point multiplication by 256.
  *
  * \return Random integer from [0, 255].
  * \sa __internal_prng__
  */
#ifndef DEBUGRANDOM
UINT8 P_RandomByte(pr_class_t pr_class)
{
#else
UINT8 P_RandomByteD(const char *rfile, INT32 rline, pr_class_t pr_class)
{
	CONS_Printf("P_RandomByte(%u) at: %sp %d\n", pr_class, rfile, rline);
#endif
	return (UINT8)((__internal_prng__(pr_class) & 0xFF00) >> 8);
}

/** Provides a random integer for picking random elements from an array.
  * Distribution is uniform.
  * NOTE: Maximum range is 65536.
  *
  * \param a Number of items in array.
  * \return A random integer from [0,a).
  * \sa __internal_prng__
  */
#ifndef DEBUGRANDOM
INT32 P_RandomKey(pr_class_t pr_class, INT32 a)
{
#else
INT32 P_RandomKeyD(const char *rfile, INT32 rline, pr_class_t pr_class, INT32 a)
{
	CONS_Printf("P_RandomKey(%u) at: %sp %d\n", pr_class, rfile, rline);
#endif
	return (INT32)(((INT64)__internal_prng__(pr_class) * a) >> FRACBITS);
}

/** Provides a random integer in a given range.
  * Distribution is uniform.
  * NOTE: Maximum range is 65536.
  *
  * \param a Lower bound.
  * \param b Upper bound.
  * \return A random integer from [a,b].
  * \sa __internal_prng__
  */
#ifndef DEBUGRANDOM
INT32 P_RandomRange(pr_class_t pr_class, INT32 a, INT32 b)
{
#else
INT32 P_RandomRangeD(const char *rfile, INT32 rline, pr_class_t pr_class, INT32 a, INT32 b)
{
	CONS_Printf("P_RandomRange(%u) at: %sp %d\n", pr_class, rfile, rline);
#endif
	return (INT32)(((INT64)__internal_prng__(pr_class) * (b - a + 1)) >> FRACBITS) + a;
}



// ----------------------
// PRNG seeds & debugging
// ----------------------

/** Peeks to see what the next result from the PRNG will be.
  * Used for debugging.
  *
  * \return A 'random' fixed point number from [0,1).
  * \sa __internal_prng__
  */
fixed_t P_RandomPeek(pr_class_t pr_class)
{
	UINT32 r = rng.seed[pr_class];
	fixed_t ret = __internal_prng__(pr_class);
	rng.seed[pr_class] = r;
	return ret;
}

/** Gets the current random seed.  Used by netgame savegames.
  *
  * \return Current random seed.
  * \sa P_SetRandSeed
  */
#ifndef DEBUGRANDOM
UINT32 P_GetRandSeed(pr_class_t pr_class)
{
#else
UINT32 P_GetRandSeedD(const char *rfile, INT32 rline, pr_class_t pr_class)
{
	CONS_Printf("P_GetRandSeed(%u) at: %sp %d\n", pr_class, rfile, rline);
#endif
	return rng.seed[pr_class];
}

/** Gets the initial random seed.  Used by demos.
  *
  * \return Initial random seed.
  * \sa P_SetRandSeed
  */
#ifndef DEBUGRANDOM
UINT32 P_GetInitSeed(pr_class_t pr_class)
{
#else
UINT32 P_GetInitSeedD(const char *rfile, INT32 rline, pr_class_t pr_class)
{
	CONS_Printf("P_GetInitSeed(%u) at: %sp %d\n", pr_class, rfile, rline);
#endif
	return rng.init[pr_class];
}

/** Sets the random seed.
  * Used at the beginning of a game.
  *
  * \param pr_class RNG class to adjust.
  * \param seed New random seed.
  * \sa P_GetRandSeed
  */
#ifndef DEBUGRANDOM
void P_SetRandSeed(pr_class_t pr_class, UINT32 seed)
{
#else
void P_SetRandSeedD(const char *rfile, INT32 rline, pr_class_t pr_class, UINT32 seed)
{
	CONS_Printf("P_SetRandSeed(%u) at: %sp %d\n", pr_class, rfile, rline);
#endif
	// xorshift requires a nonzero seed
	// this should never happen, but just in case it DOES, we check
	if (!seed) seed = DEFAULT_SEED;
	rng.seed[pr_class] = rng.init[pr_class] = seed;
}

/** Sets both the initial seed and the current seed.
  * Used for netgame sync.
  *
  * \param pr_class RNG class to adjust.
  * \param init Sent initial seed.
  * \param seed Sent current seed.
  * \sa P_SetRandSeed
  */
#ifndef DEBUGRANDOM
void P_SetRandSeedNet(pr_class_t pr_class, UINT32 init, UINT32 seed)
{
#else
void P_SetRandSeedNetD(const char *rfile, INT32 rline, pr_class_t pr_class, UINT32 init, UINT32 seed)
{
	CONS_Printf("P_SetRandSeedNet(%u) at: %sp %d\n", pr_class, rfile, rline);
#endif
	if (!init) init = DEFAULT_SEED;
	rng.init[pr_class] = init;

	if (!seed) seed = DEFAULT_SEED;
	rng.seed[pr_class] = seed;
}

/** Initializes random seeds for all classes.
  * Used at the beginning of a game.
  *
  * \param rindex New random index.
  * \sa P_SetRandSeed
  */
void P_ClearRandom(UINT32 seed)
{
	size_t i;

	if (!seed) seed = DEFAULT_SEED;

	for (i = 0; i < PRNUMCLASS; i++)
	{
		P_SetRandSeed(i, seed);

		// Different XOR from __internal_prng__
		// so that it's not as predictable.
		seed ^= seed >> 13;
		seed ^= seed << 25;
		seed ^= seed >> 11;
	}
}

/** Gets a randomized seed for setting the random seed.
  *
  * \sa P_GetRandSeed
  */
UINT32 M_RandomizedSeed(void)
{
	return ((totalplaytime & 0xFFFF) << 16) | M_RandomFixed();
}
