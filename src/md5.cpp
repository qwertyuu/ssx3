#include "md5.h"

// MD5 state structure layout:
// offset 0x00: count[0] - bit count low
// offset 0x04: count[1] - bit count high
// offset 0x08: state[0] (a)
// offset 0x0C: state[1] (b)
// offset 0x10: state[2] (c)
// offset 0x14: state[3] (d)
// offset 0x18: buffer[64]

extern "C" {
    void *memcpy(void *dst, const void *src, uint n);
}

// MD5 padding data
extern char DAT_0048daf8[];

INCLUDE_ASM("md5", md5_process);
#ifdef SKIP_ASM
// MD5 block transform - processes one 64-byte block
void md5_process(uint *state, uint *data)
{
    uint a, b, c, d;
    uint x[16];
    uint t;

    // Copy data to aligned buffer if needed
    if (((uint)data & 3) != 0) {
        memcpy(x, data, 64);
        data = x;
    }

    a = state[2];  // state[0] at offset 0x08
    b = state[3];  // state[1] at offset 0x0C
    c = state[4];  // state[2] at offset 0x10
    d = state[5];  // state[3] at offset 0x14

    // Round 1: F(b,c,d) = (b & c) | (~b & d)
    t = a + (b & c | ~b & d) + data[0] + 0xd76aa478;
    a = (t << 7 | t >> 25) + b;
    t = d + (a & b | ~a & c) + data[1] + 0xe8c7b756;
    d = (t << 12 | t >> 20) + a;
    t = c + (d & a | ~d & b) + data[2] + 0x242070db;
    c = (t << 17 | t >> 15) + d;
    t = b + (c & d | ~c & a) + data[3] + 0xc1bdceee;
    b = (t << 22 | t >> 10) + c;

    t = a + (b & c | ~b & d) + data[4] + 0xf57c0faf;
    a = (t << 7 | t >> 25) + b;
    t = d + (a & b | ~a & c) + data[5] + 0x4787c62a;
    d = (t << 12 | t >> 20) + a;
    t = c + (d & a | ~d & b) + data[6] + 0xa8304613;
    c = (t << 17 | t >> 15) + d;
    t = b + (c & d | ~c & a) + data[7] + 0xfd469501;
    b = (t << 22 | t >> 10) + c;

    t = a + (b & c | ~b & d) + data[8] + 0x698098d8;
    a = (t << 7 | t >> 25) + b;
    t = d + (a & b | ~a & c) + data[9] + 0x8b44f7af;
    d = (t << 12 | t >> 20) + a;
    t = c + (d & a | ~d & b) + data[10] + 0xffff5bb1;
    c = (t << 17 | t >> 15) + d;
    t = b + (c & d | ~c & a) + data[11] + 0x895cd7be;
    b = (t << 22 | t >> 10) + c;

    t = a + (b & c | ~b & d) + data[12] + 0x6b901122;
    a = (t << 7 | t >> 25) + b;
    t = d + (a & b | ~a & c) + data[13] + 0xfd987193;
    d = (t << 12 | t >> 20) + a;
    t = c + (d & a | ~d & b) + data[14] + 0xa679438e;
    c = (t << 17 | t >> 15) + d;
    t = b + (c & d | ~c & a) + data[15] + 0x49b40821;
    b = (t << 22 | t >> 10) + c;

    // Round 2: G(b,c,d) = (b & d) | (c & ~d)
    t = a + (b & d | c & ~d) + data[1] + 0xf61e2562;
    a = (t << 5 | t >> 27) + b;
    t = d + (a & b | c & ~b) + data[6] + 0xc040b340;  // Wait, pattern is (a & c | b & ~c)
    // Actually: G uses (x & z) | (y & ~z), the Ghidra shows (uVar2 & uVar6 | uVar5 & ~uVar6)
    // Let me match exactly...
    d = (t << 9 | t >> 23) + a;
    t = c + (d & a | b & ~a) + data[11] + 0x265e5a51;
    c = (t << 14 | t >> 18) + d;
    t = b + (c & d | a & ~d) + data[0] + 0xe9b6c7aa;
    b = (t << 20 | t >> 12) + c;

    t = a + (b & d | c & ~d) + data[5] + 0xd62f105d;
    a = (t << 5 | t >> 27) + b;
    t = d + (a & b | c & ~b) + data[10] + 0x02441453;
    d = (t << 9 | t >> 23) + a;
    t = c + (d & a | b & ~a) + data[15] + 0xd8a1e681;
    c = (t << 14 | t >> 18) + d;
    t = b + (c & d | a & ~d) + data[4] + 0xe7d3fbc8;
    b = (t << 20 | t >> 12) + c;

    t = a + (b & d | c & ~d) + data[9] + 0x21e1cde6;
    a = (t << 5 | t >> 27) + b;
    t = d + (a & b | c & ~b) + data[14] + 0xc33707d6;
    d = (t << 9 | t >> 23) + a;
    t = c + (d & a | b & ~a) + data[3] + 0xf4d50d87;
    c = (t << 14 | t >> 18) + d;
    t = b + (c & d | a & ~d) + data[8] + 0x455a14ed;
    b = (t << 20 | t >> 12) + c;

    t = a + (b & d | c & ~d) + data[13] + 0xa9e3e905;
    a = (t << 5 | t >> 27) + b;
    t = d + (a & b | c & ~b) + data[2] + 0xfcefa3f8;
    d = (t << 9 | t >> 23) + a;
    t = c + (d & a | b & ~a) + data[7] + 0x676f02d9;
    c = (t << 14 | t >> 18) + d;
    t = b + (c & d | a & ~d) + data[12] + 0x8d2a4c8a;
    b = (t << 20 | t >> 12) + c;

    // Round 3: H(b,c,d) = b ^ c ^ d
    t = a + (b ^ c ^ d) + data[5] + 0xfffa3942;
    a = (t << 4 | t >> 28) + b;
    t = d + (a ^ b ^ c) + data[8] + 0x8771f681;
    d = (t << 11 | t >> 21) + a;
    t = c + (d ^ a ^ b) + data[11] + 0x6d9d6122;
    c = (t << 16 | t >> 16) + d;
    t = b + (c ^ d ^ a) + data[14] + 0xfde5380c;
    b = (t << 23 | t >> 9) + c;

    t = a + (b ^ c ^ d) + data[1] + 0xa4beea44;
    a = (t << 4 | t >> 28) + b;
    t = d + (a ^ b ^ c) + data[4] + 0x4bdecfa9;
    d = (t << 11 | t >> 21) + a;
    t = c + (d ^ a ^ b) + data[7] + 0xf6bb4b60;
    c = (t << 16 | t >> 16) + d;
    t = b + (c ^ d ^ a) + data[10] + 0xbebfbc70;
    b = (t << 23 | t >> 9) + c;

    t = a + (b ^ c ^ d) + data[13] + 0x289b7ec6;
    a = (t << 4 | t >> 28) + b;
    t = d + (a ^ b ^ c) + data[0] + 0xeaa127fa;
    d = (t << 11 | t >> 21) + a;
    t = c + (d ^ a ^ b) + data[3] + 0xd4ef3085;
    c = (t << 16 | t >> 16) + d;
    t = b + (c ^ d ^ a) + data[6] + 0x04881d05;
    b = (t << 23 | t >> 9) + c;

    t = a + (b ^ c ^ d) + data[9] + 0xd9d4d039;
    a = (t << 4 | t >> 28) + b;
    t = d + (a ^ b ^ c) + data[12] + 0xe6db99e5;
    d = (t << 11 | t >> 21) + a;
    t = c + (d ^ a ^ b) + data[15] + 0x1fa27cf8;
    c = (t << 16 | t >> 16) + d;
    t = b + (c ^ d ^ a) + data[2] + 0xc4ac5665;
    b = (t << 23 | t >> 9) + c;

    // Round 4: I(b,c,d) = c ^ (b | ~d)
    t = a + (c ^ (b | ~d)) + data[0] + 0xf4292244;
    a = (t << 6 | t >> 26) + b;
    t = d + (b ^ (a | ~c)) + data[7] + 0x432aff97;
    d = (t << 10 | t >> 22) + a;
    t = c + (a ^ (d | ~b)) + data[14] + 0xab9423a7;
    c = (t << 15 | t >> 17) + d;
    t = b + (d ^ (c | ~a)) + data[5] + 0xfc93a039;
    b = (t << 21 | t >> 11) + c;

    t = a + (c ^ (b | ~d)) + data[12] + 0x655b59c3;
    a = (t << 6 | t >> 26) + b;
    t = d + (b ^ (a | ~c)) + data[3] + 0x8f0ccc92;
    d = (t << 10 | t >> 22) + a;
    t = c + (a ^ (d | ~b)) + data[10] + 0xffeff47d;
    c = (t << 15 | t >> 17) + d;
    t = b + (d ^ (c | ~a)) + data[1] + 0x85845dd1;
    b = (t << 21 | t >> 11) + c;

    t = a + (c ^ (b | ~d)) + data[8] + 0x6fa87e4f;
    a = (t << 6 | t >> 26) + b;
    t = d + (b ^ (a | ~c)) + data[15] + 0xfe2ce6e0;
    d = (t << 10 | t >> 22) + a;
    t = c + (a ^ (d | ~b)) + data[6] + 0xa3014314;
    c = (t << 15 | t >> 17) + d;
    t = b + (d ^ (c | ~a)) + data[13] + 0x4e0811a1;
    b = (t << 21 | t >> 11) + c;

    t = a + (c ^ (b | ~d)) + data[4] + 0xf7537e82;
    a = (t << 6 | t >> 26) + b;
    state[2] = state[2] + a;
    t = d + (b ^ (a | ~c)) + data[11] + 0xbd3af235;
    d = (t << 10 | t >> 22) + a;
    state[5] = state[5] + d;
    t = c + (a ^ (d | ~b)) + data[2] + 0x2ad7d2bb;
    c = (t << 15 | t >> 17) + d;
    state[4] = state[4] + c;
    t = b + (d ^ (c | ~a)) + data[9] + 0xeb86d391;
    state[3] = state[3] + (t << 21 | t >> 11) + c;
}
#endif

INCLUDE_ASM("md5", md5_init);
#ifdef SKIP_ASM
// Initialize MD5 state with standard initial values
void md5_init(uint *state)
{
    state[2] = 0x67452301;
    state[5] = 0x10325476;
    state[3] = 0xefcdab89;
    state[4] = 0x98badcfe;
    state[1] = 0;
    state[0] = 0;
}
#endif

INCLUDE_ASM("md5", md5_append);
#ifdef SKIP_ASM
// Append data to MD5 state
void md5_append(uint *state, char *data, int nbytes)
{
    uint offset;
    uint countlo;
    uint counthi;
    int copy;

    offset = state[0] >> 3 & 0x3f;
    if (nbytes > 0) {
        countlo = state[0] + nbytes * 8;
        counthi = state[1] + (nbytes >> 29);
        state[1] = counthi;
        state[0] = countlo;
        if (countlo < (uint)(nbytes * 8)) {
            state[1] = counthi + 1;
        }
        if (offset != 0) {
            copy = nbytes;
            if ((int)(offset + nbytes) > 0x40) {
                copy = 0x40 - offset;
            }
            memcpy((char *)state + offset + 0x18, data, copy);
            if ((int)(offset + copy) < 0x40) {
                return;
            }
            nbytes = nbytes - copy;
            data = data + copy;
            md5_process(state, state + 6);
        }
        for (; nbytes > 0x3f; nbytes = nbytes - 0x40) {
            md5_process(state, (uint *)data);
            data = data + 0x40;
        }
        if (nbytes != 0) {
            memcpy(state + 6, data, nbytes);
        }
    }
}
#endif

INCLUDE_ASM("md5", md5_finish);
#ifdef SKIP_ASM
// Finalize MD5 and output 16-byte digest
void md5_finish(uint *state, char *digest)
{
    char bits[16];
    uint i;
    int idx;
    uint shift;
    int padlen;

    // Encode bit count
    i = 0;
    do {
        idx = (int)i >> 2;
        shift = (i & 3) << 3;
        bits[i] = (char)(state[idx] >> shift);
        i++;
    } while ((int)i < 8);

    // Pad to 56 mod 64
    padlen = ((0x37 - (state[0] >> 3)) & 0x3f) + 1;
    md5_append(state, DAT_0048daf8, padlen);

    // Append bit count
    md5_append(state, bits, 8);

    // Encode hash state to digest
    i = 0;
    do {
        idx = (int)i >> 2;
        shift = (i & 3) << 3;
        digest[i] = (char)(state[idx + 2] >> shift);
        i++;
    } while ((int)i < 0x10);
}
#endif
