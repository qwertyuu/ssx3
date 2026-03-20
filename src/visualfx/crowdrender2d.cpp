#include "visualfx/crowdrender2d.h"
#include <stdint.h>

//100%
//https://decomp.me/scratch/pOTwa

INCLUDE_ASM("visualfx/crowdrender2d", cCrowdRender2D_cCrowdRender2D__Fi);
#ifdef SKIP_ASM
int cCrowdRender2D_cCrowdRender2D(int param_1) {
	cCrowdRender2D_init();
	return param_1;
}
#endif

//100%
//https://decomp.me/scratch/zsTkS
INCLUDE_ASM("visualfx/crowdrender2d", cCrowdRender2D__cCrowdRender2D__FPii);
#ifdef SKIP_ASM
void cCrowdRender2D__cCrowdRender2D(int* param_1, int param_2)
{
	cCrowdRender2D_purge(param_1);

	if ((param_2 & 1) != 0) {
		operator_delete(param_1);
	}

	return;
}
#endif

INCLUDE_ASM("visualfx/crowdrender2d", cCrowdRender2D_init__Fv);
#ifdef SKIP_ASM
void cCrowdRender2D_init()
{

}
#endif

INCLUDE_ASM("visualfx/crowdrender2d", cCrowdRender2D_purge__FPi);
#ifdef SKIP_ASM
int cCrowdRender2D_purge(int *param_1)
{
	return *param_1;
}
#endif

//100%
//https://decomp.me/scratch/Rbiv4
INCLUDE_ASM("visualfx/crowdrender2d", cCrowdRender2D_constructCrowdAnim2D__FPv);
#ifdef SKIP_ASM

void* cMemMan_alloc(int a, const char* b, uint32_t c, int d);

void* cCrowdRender2D_constructCrowdAnim2D(void* param) {
	void* memory = cMemMan_alloc(0x20, D_004875D8, 0x20000000, 0);
	return cCrowdAnim2D_cCrowdAnim2D(memory, param);
}
#endif

INCLUDE_ASM("visualfx/crowdrender2d", cCrowdAnim2D_cCrowdAnim2D__FPvT0);
#ifdef SKIP_ASM
void* cCrowdAnim2D_cCrowdAnim2D(void* memory, void* param_1)
{

}
#endif

INCLUDE_ASM("visualfx/crowdrender2d", cCrowdAnim2D_advanceFrame);
#ifdef SKIP_ASM
int cCrowdAnim2D_advanceFrame(int unused, int frame, int numFrames, int* mode)
{
	switch (*mode) {
	case 0: // forward clamp
		frame++;
		if (frame >= numFrames) {
			frame = numFrames - 1;
		}
		break;
	case 1: // backward clamp
		frame--;
		if (frame < 0) {
			frame = 0;
		}
		break;
	case 2: // ping-pong forward
		if (numFrames == 1) break;
		frame++;
		if (frame >= numFrames) {
			frame = numFrames - 2;
			*mode = 3;
		}
		break;
	case 3: // ping-pong backward
		if (numFrames == 1) break;
		frame--;
		if (frame < 0) {
			frame = 1;
			*mode = 2;
		}
		break;
	case 4: // loop forward
		if (numFrames == 1) break;
		frame++;
		if (frame >= numFrames) {
			frame = 0;
		}
		break;
	}
	return frame;
}
#endif

INCLUDE_ASM("visualfx/crowdrender2d", cCrowdAnim2D_update);
#ifdef SKIP_ASM
// External data structures
extern int iGp00002a74;        // timer/clock object pointer
extern int DAT_00445e20[];     // crowd animation group table offsets
extern int DAT_00445d3c[];     // crowd anim frame count per group (stride 0x36 ints = 0xD8 bytes)
extern int DAT_00445dd0[];     // crowd anim frame data table (stride 0x36)

// cCrowdAnim2D layout:
// [0] = animation group index
// [1] = current sequence index
// [2] = direction (0=forward, 1=backward)
// [3] = current frame
// [4] = current frame data value
// [5] = frame interval (ticks between frames)
// [6] = last tick timestamp

// Forward declaration
int cCrowdAnim2D_advanceFrame(int *anim, int frame, int numFrames, int *direction);

void cCrowdAnim2D_update(int *anim)
{
    int nextSeq;
    int frame;
    int *seqEntry;
    int seqDir;
    int numFrames;

    // Check if enough time has elapsed
    if (*(int *)(iGp00002a74 + 0x1c) - anim[6] < anim[5]) {
        return;
    }

    // Update timestamp
    anim[6] = *(int *)(iGp00002a74 + 0x1c);

    // Get current sequence entry: seqEntry points to {nextSeq, direction, mode}
    seqEntry = (int *)((int)anim[1] * 0xc + *(int *)((int)&DAT_00445e20 + anim[0] * 4) * 0xc + 0x445e10);
    nextSeq = *seqEntry;

    if (nextSeq == anim[1]) {
        // Same sequence - just advance frame
        frame = anim[3];
        numFrames = *(int *)(nextSeq * 0xd8 + 0x445d38 + 4);
        frame = cCrowdAnim2D_advanceFrame(anim, frame, numFrames, anim + 2);
        anim[3] = frame;
    }
    else {
        seqDir = seqEntry[2];

        if (seqDir == 1) {
            // Mode 1: backward start
            if (anim[2] == 1) {
                anim[2] = 0;
            }
            frame = anim[3];
            numFrames = *(int *)((int)&DAT_00445d3c + anim[1] * 0xd8);
            if (frame == numFrames - 1) {
                // At last frame - transition to next sequence
                anim[1] = nextSeq;
                seqDir = seqEntry[1];
                anim[2] = seqDir;
                if (seqDir == 1) {
                    anim[3] = *(int *)((int)&DAT_00445d3c + nextSeq * 0xd8) - 1;
                } else {
                    anim[3] = 0;
                    goto set_frame_data;
                }
            } else {
                numFrames = *(int *)(anim[1] * 0xd8 + 0x445d38 + 4);
                frame = cCrowdAnim2D_advanceFrame(anim, frame, numFrames, anim + 2);
                anim[3] = frame;
            }
        }
        else if (seqDir == 0) {
            // Mode 0: forward play
            if (anim[2] == 0) {
                anim[2] = 1;
            }
            frame = anim[3];
            if (frame == 0) {
                // At first frame - transition to next sequence
                anim[1] = nextSeq;
                seqDir = seqEntry[1];
                anim[2] = seqDir;
                if (seqDir == 1) {
                    anim[3] = *(int *)((int)&DAT_00445d3c + nextSeq * 0xd8) - 1;
                } else {
                    anim[3] = 0;
                    goto set_frame_data;
                }
            } else {
                numFrames = *(int *)(anim[1] * 0xd8 + 0x445d38 + 4);
                frame = cCrowdAnim2D_advanceFrame(anim, frame, numFrames, anim + 2);
                anim[3] = frame;
            }
        }
        else if (seqDir == 2) {
            // Mode 2: jump to next sequence
            anim[1] = nextSeq;
            frame = anim[3];
            anim[2] = seqEntry[1];
            numFrames = *(int *)(nextSeq * 0xd8 + 0x445d38 + 4);
            frame = cCrowdAnim2D_advanceFrame(anim, frame, numFrames, anim + 2);
            anim[3] = frame;
        }
        else {
            // Default mode: just set frame data from current state
            goto set_frame_data;
        }
    }

set_frame_data:
    nextSeq = anim[1];
    anim[4] = (&DAT_00445dd0)[nextSeq * 0x36 + anim[3]];
}
#endif

