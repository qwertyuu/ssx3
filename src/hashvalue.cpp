#include "common.h"

int GetHashValue32(char* param_1);
int tHashName32_getHashValue(uint*, char*);


//100%
//https://decomp.me/scratch/YWMcE
INCLUDE_ASM("hashvalue", tHashName32_getHashValue__FPUiPc);
#ifdef SKIP_ASM
int tHashName32_getHashValue(uint* out, char* str) {
    uint hash = 0;
    uint top;

    while (*str) {
        hash = (hash << 4) + *str++;
        top = hash & 0xF0000000;
        if (top != 0) {
            hash = (hash ^ (top >> 23)) ^ top;
        }
    }

    *out = hash;
    return *out;
}
#endif

//100%
//https://decomp.me/scratch/VD967
INCLUDE_ASM("hashvalue", GetHashValue32__FPc);
#ifdef SKIP_ASM
int GetHashValue32(char* param_1) {
    char hash[4];
    return tHashName32_getHashValue((uint*)hash, param_1);
}
#endif

//99.17%
//https://decomp.me/scratch/ZrcdK
INCLUDE_ASM("hashvalue", GetHashValue64__FPc);
#ifdef SKIP_ASM
ulong tHashName64_getHashValue(ulong* out, char* str) {
    //char md5[96];
    ulong hash[2];

    //md5_init(md5);
    //md5_append(md5, str, strlen(str));
    //md5_finish(md5, hash);
    *out = hash[0];
    return hash[0];
}
#endif


//100%
//https://decomp.me/scratch/I4NLb
INCLUDE_ASM("hashvalue", tHashName64_getHashValue__FPUlPc);
#ifdef SKIP_ASM
ulong GetHashValue64(char* str) {
    char hash[8];
    return tHashName64_getHashValue((ulong*)hash, str);
}
#endif

INCLUDE_ASM("hashvalue", func_00317710);
#ifdef SKIP_ASM
void md5_init(char*);
void md5_append(char*, char*, int);
void md5_finish(char*, void*);
int strlen(const char*);

ulong* func_00317710(ulong* out, ulong* hashBuf, char* str) {
	char md5State[0x60];
	ulong digest[4];

	md5_init(md5State);
	md5_append(md5State, str, strlen(str));
	md5_finish(md5State, digest);

	hashBuf[0] = digest[0];
	hashBuf[1] = digest[1];
	hashBuf[2] = digest[2];
	hashBuf[3] = digest[3];
	out[0] = hashBuf[0];
	out[1] = hashBuf[1];
	out[2] = hashBuf[2];
	out[3] = hashBuf[3];

	return out;
}
#endif

INCLUDE_ASM("hashvalue", func_00317798);
#ifdef SKIP_ASM
ulong* func_00317798(ulong* out, char* str) {
	ulong hashBuf[4];
	func_00317710(out, hashBuf, str);
	return out;
}
#endif
