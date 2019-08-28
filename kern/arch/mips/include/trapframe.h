#ifndef _MIPS_TRAPFRAME_H_
#define _MIPS_TRAPFRAME_H_

/*
 * Structure describing what is saved on the stack during entry to
 * the exception handler.
 *
 * This must agree with the code in exception-*.S.
 */

struct trapframe {
	uint32_t tf_vaddr;	/* coprocessor 0 vaddr register */
	uint32_t tf_status;	/* coprocessor 0 status register */
	uint32_t tf_cause;	/* coprocessor 0 cause register */
	uint32_t tf_lo;
	uint32_t tf_hi;
	uint32_t tf_ra;		/* Saved register 31 */
	uint32_t tf_at;		/* Saved register 1 (AT) */
	uint32_t tf_v0;		/* Saved register 2 (v0) */
	uint32_t tf_v1;		/* etc. */
	uint32_t tf_a0;
	uint32_t tf_a1;
	uint32_t tf_a2;
	uint32_t tf_a3;
	uint32_t tf_t0;
	uint32_t tf_t1;
	uint32_t tf_t2;
	uint32_t tf_t3;
	uint32_t tf_t4;
	uint32_t tf_t5;
	uint32_t tf_t6;
	uint32_t tf_t7;
	uint32_t tf_s0;
	uint32_t tf_s1;
	uint32_t tf_s2;
	uint32_t tf_s3;
	uint32_t tf_s4;
	uint32_t tf_s5;
	uint32_t tf_s6;
	uint32_t tf_s7;
	uint32_t tf_t8;
	uint32_t tf_t9;
	uint32_t tf_gp;
	uint32_t tf_sp;
	uint32_t tf_s8;
	uint32_t tf_epc;	/* coprocessor 0 epc register */
};

/*
 * MIPS exception codes.
 */
#define EX_IRQ    0    /* Interrupt */
#define EX_MOD    1    /* TLB Modify (write to read-only page) */
#define EX_TLBL   2    /* TLB miss on load */
#define EX_TLBS   3    /* TLB miss on store */
#define EX_ADEL   4    /* Address error on load */
#define EX_ADES   5    /* Address error on store */
#define EX_IBE    6    /* Bus error on instruction fetch */
#define EX_DBE    7    /* Bus error on data load *or* store */
#define EX_SYS    8    /* Syscall */
#define EX_BP     9    /* Breakpoint */
#define EX_RI     10   /* Reserved (illegal) instruction */
#define EX_CPU    11   /* Coprocessor unusable */
#define EX_OVF    12   /* Arithmetic overflow */

/*
 * Function to enter user mode. Does not return. The trapframe must
 * be on the thread's own stack or bad things will happen.
 */
__DEAD void mips_usermode(struct trapframe *tf);

/*
 * Arrays used to load the kernel stack and curthread on trap entry.
 */
extern vaddr_t cpustacks[];
extern vaddr_t cputhreads[];


#endif /* _MIPS_TRAPFRAME_H_ */
