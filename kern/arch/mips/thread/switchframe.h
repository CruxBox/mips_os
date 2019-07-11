#ifndef _MIPS_SWITCHFRAME_H_
#define _MIPS_SWITCHFRAME_H_

/*
 * Structure describing what is saved on the stack during a context switch.
 *
 * This must agree with the code in switch.S.
 */

struct switchframe {
        uint32_t sf_s0;
        uint32_t sf_s1;
        uint32_t sf_s2;
        uint32_t sf_s3;
        uint32_t sf_s4;
        uint32_t sf_s5;
        uint32_t sf_s6;
        uint32_t sf_s8;
        uint32_t sf_gp;
        uint32_t sf_ra;
};

#endif /* _MIPS_SWITCHFRAME_H_ */
