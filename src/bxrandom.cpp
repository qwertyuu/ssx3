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

INCLUDE_ASM("bxrandom", BXrand__Fv);
#ifdef SKIP_ASM
void cBxPseudoRng_NextInt(const uint[]);                         /* extern */

void BXrand() 
{
    cBxPseudoRng_NextInt(D_004FF018);
}
#endif


INCLUDE_ASM("bxrandom", AIrand);
#ifdef SKIP_ASM
extern const uint D_004FF030[];

unsigned int AIrand()
{
	return cBxPseudoRng_NextInt(D_004FF030);
}
#endif

INCLUDE_ASM("bxrandom", AIrandf__Fff);
#ifdef SKIP_ASM
unsigned int AIrand();

float AIrandf(float a, float b)
{
	unsigned int r = (AIrand() & 0x7FFFFFFF) | 0x3F800000;
	float f = *((float*)&r) - 1.0f; // random 0.0 ? f < 1.0
	return a + (b - a) * f;
}
#endif

INCLUDE_ASM("bxrandom", func_00317890__Fff);
#ifdef SKIP_ASM
float func_00317890(float param_1, float param_2)
{
	float fVar1;

	fVar1 = AIrandf(-1.0, 1.0);
	return param_1 + param_2 * fVar1;
}
#endif

INCLUDE_ASM("bxrandom", func_003178E0);
#ifdef SKIP_ASM
void func_003E6574(void*, const void*, int);

void func_003178E0(void* dst)
{
	func_003E6574(dst, D_004FF030, 0x18);
}
#endif

INCLUDE_ASM("bxrandom", func_00317908);
#ifdef SKIP_ASM
void func_00317908(const void* src)
{
	func_003E6574(D_004FF030, src, 0x18);
}
#endif

INCLUDE_ASM("bxrandom", func_00317930);
#ifdef SKIP_ASM
extern const uint D_0048DCB0[];

void func_00317930(void* dst)
{
	func_003E6574(dst, D_0048DCB0, 0x18);
}
#endif

INCLUDE_ASM("bxrandom", cBxPseudoRng_Seed);
#ifdef SKIP_ASM
void cBxPseudoRng_Seed(uint* state, uint seed)
{
	unsigned long long s = (unsigned long long)seed;
	s += 0xF22D0E56ULL;
	state[0] = (uint)s;
	s += 0x96041893ULL;
	state[1] = (uint)s;
	s += 0x3DF3B646ULL;
	state[2] = (uint)s;
	s += 0x40DDE76DULL;
	state[3] = (uint)s;
	s += 0x97327AE1ULL;
	state[4] = (uint)s;
	s += 0xD1A9FBE7ULL;
	state[5] = (uint)s;
}
#endif

INCLUDE_ASM("bxrandom", cBxPseudoRng_NextInt);
#ifdef SKIP_ASM
uint cBxPseudoRng_NextInt(uint* state)
{
	uint t, carry;

	t = state[5] + state[4];
	carry = (t < state[5]);
	if (!carry) {
		carry = (t < state[4]);
	}
	state[4] = t;

	t = t + state[3] + carry;
	carry = (t < state[3]);
	state[3] = t;

	t = t + state[2] + carry;
	carry = (t < state[2]);
	state[2] = t;

	t = t + state[1] + carry;
	carry = (t < state[1]);
	state[1] = t;

	t = t + state[0] + carry;
	state[0] = t;

	state[5]++;
	if (state[5] == 0) {
		state[4]++;
		if (state[4] == 0) {
			state[3]++;
			if (state[3] == 0) {
				state[2]++;
				if (state[2] == 0) {
					state[1]++;
					if (state[1] == 0) {
						state[0]++;
						t = state[0];
					}
				}
			}
		}
	}

	return t;
}
#endif
