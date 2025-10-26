#include "common.h"
void cBxPseudoRng_Seed(uint* param_1, uint seedAddend);
void BXsrand(uint seed);
extern const uint D_004FF018[];

//100%
//https://decomp.me/scratch/ViFdz
INCLUDE_ASM("bxrandom", BXsrand__FUi);
#ifdef SKIP_ASM
void BXsrand(uint seed)
{
	cBxPseudoRng_Seed((uint*)D_004FF018, seed);
	return;
}
#endif

INCLUDE_ASM("bxrandom", BXrand);

INCLUDE_ASM("bxrandom", AIrand);

INCLUDE_ASM("bxrandom", AIrandf);

INCLUDE_ASM("bxrandom", func_00317890__Fff);
#ifdef SKIP_ASM
float AIrandf(float arg0, float arg1);

float func_00317890(float param_1, float param_2)
{
	float fVar1;

	fVar1 = AIrandf(-1.0, 1.0);
	return param_1 + param_2 * fVar1;
}
#endif

INCLUDE_ASM("bxrandom", func_003178E0);

INCLUDE_ASM("bxrandom", func_00317908);

INCLUDE_ASM("bxrandom", func_00317930);

INCLUDE_ASM("bxrandom", cBxPseudoRng_Seed);

INCLUDE_ASM("bxrandom", cBxPseudoRng_NextInt);
