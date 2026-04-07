/* tests/test_thumb_dis.c — unit tests for thumb_dis_one()
 *
 * Golden test approach: inline byte constants + expected output strings.
 * No file I/O.  Follows the dwarf_test.c style.
 *
 * Covers:
 *   P1: MOV/LSL/LSR/ASR/ADD/SUB, LDR/STR (multiple encodings), PUSH/POP,
 *       BX, B (all conds), BKPT, NOP, ADR, SP-relative ops
 *   P2: Data processing (AND/EOR/TST/CMP/ORR/BIC/MVN), sign/zero-extend,
 *       STM/LDM, IT block instruction
 *   32-bit: BL, MOVW, MOVT, ADDW, SUBW, LDR.W, STR.W, ADD.W, B.W, BEQ.W
 *   IT state machine: multi-step ITTEE block (4 instructions, 2T+2E)
 *   Edge cases: buf_len < 2, buf_len == 2 for 32-bit prefix, unknown encoding,
 *               itstate=NULL degraded mode
 *
 * SPDX-License-Identifier: MIT
 */

#include "thumb_dis.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* ── test harness ─────────────────────────────────────────────────────────── */

static int s_run = 0, s_pass = 0;

#define CHECK(cond, label) do { \
    s_run++; \
    if (cond) { s_pass++; printf("  PASS  %s\n", (label)); } \
    else             printf("  FAIL  %s\n", (label)); \
} while (0)

/* Disassemble one instruction and check output + return value. */
static void chk(const char *label,
                uint32_t pc, const uint8_t *buf, size_t buf_len,
                uint8_t *its,
                const char *expect_out, int expect_ret)
{
    char out[THUMB_DIS_OUT_MAX];
    out[0] = '\0';
    int ret = thumb_dis_one(pc, buf, buf_len, out, sizeof(out), its);
    int ok = (ret == expect_ret) && (strcmp(out, expect_out) == 0);
    s_run++;
    if (ok) {
        s_pass++;
        printf("  PASS  %s\n", label);
    } else {
        printf("  FAIL  %s  (ret=%d want=%d  out=\"%s\"  want=\"%s\")\n",
               label, ret, expect_ret, out, expect_out);
    }
}

/* ── 16-bit instruction tests ─────────────────────────────────────────────── */

static void test_16bit(void)
{
    printf("\n--- 16-bit instructions ---\n");

    /* MOV immediate T1: movs r0, #42  → hw=0x202A */
    { const uint8_t b[] = {0x2A, 0x20};
      chk("movs r0, #42", 0x1000, b, 2, NULL, "movs r0, #42", 2); }

    /* LSL T1 (imm5=0 → movs alias): movs r0, r1  → hw=0x0008 */
    { const uint8_t b[] = {0x08, 0x00};
      chk("movs r0, r1 (lsl#0)", 0x1000, b, 2, NULL, "movs r0, r1", 2); }

    /* LSL T1 (imm5=3): lsls r0, r1, #3  → hw=0x00C8 */
    { const uint8_t b[] = {0xC8, 0x00};
      chk("lsls r0, r1, #3", 0x1000, b, 2, NULL, "lsls r0, r1, #3", 2); }

    /* LSR T1 (imm5=1): lsrs r0, r1, #1  → hw=0x0848 */
    { const uint8_t b[] = {0x48, 0x08};
      chk("lsrs r0, r1, #1", 0x1000, b, 2, NULL, "lsrs r0, r1, #1", 2); }

    /* ASR T1 (imm5=1): asrs r0, r1, #1  → hw=0x1048 */
    { const uint8_t b[] = {0x48, 0x10};
      chk("asrs r0, r1, #1", 0x1000, b, 2, NULL, "asrs r0, r1, #1", 2); }

    /* ADD register T1: adds r0, r1, r2 → hw=0x1888 (sub=0,imm=0,val=2,rn=1,rd=0) */
    { const uint8_t b[] = {0x88, 0x18};
      chk("adds r0, r1, r2 (reg)", 0x1000, b, 2, NULL, "adds r0, r1, r2", 2); }

    /* ADD immediate T1 (imm3): adds r0, r1, #2 → hw=0x1C88 */
    { const uint8_t b[] = {0x88, 0x1C};
      chk("adds r0, r1, #2 (imm3)", 0x1000, b, 2, NULL, "adds r0, r1, #2", 2); }

    /* SUB immediate T1 (imm3): subs r2, r3, #1 → hw=0x1E5A */
    { const uint8_t b[] = {0x5A, 0x1E};
      chk("subs r2, r3, #1 (imm3)", 0x1000, b, 2, NULL, "subs r2, r3, #1", 2); }

    /* CMP immediate: cmp r0, #5 → hw=0x2805 */
    { const uint8_t b[] = {0x05, 0x28};
      chk("cmp r0, #5", 0x1000, b, 2, NULL, "cmp r0, #5", 2); }

    /* ADDS immediate T2 (imm8): adds r1, #10 → hw=0x310A (bits[12:11]=10 → op5=2) */
    { const uint8_t b[] = {0x0A, 0x31};
      chk("adds r1, #10", 0x1000, b, 2, NULL, "adds r1, #10", 2); }

    /* SUBS immediate T2 (imm8): subs r0, #1 → hw=0x3801 */
    { const uint8_t b[] = {0x01, 0x38};
      chk("subs r0, #1", 0x1000, b, 2, NULL, "subs r0, #1", 2); }

    /* Data processing: ANDS r0, r1 → hw=0x4008 (op4=0) */
    { const uint8_t b[] = {0x08, 0x40};
      chk("ands r0, r1", 0x1000, b, 2, NULL, "ands r0, r1", 2); }

    /* Data processing: EORS r0, r1 → hw=0x4048 (op4=1) */
    { const uint8_t b[] = {0x48, 0x40};
      chk("eors r0, r1", 0x1000, b, 2, NULL, "eors r0, r1", 2); }

    /* Data processing: TST r0, r1 → hw=0x4208 (op4=8) */
    { const uint8_t b[] = {0x08, 0x42};
      chk("tst r0, r1", 0x1000, b, 2, NULL, "tst r0, r1", 2); }

    /* Data processing: RSBS r0, r1, #0 → hw=0x4248 (op4=9) */
    { const uint8_t b[] = {0x48, 0x42};
      chk("rsbs r0, r1, #0", 0x1000, b, 2, NULL, "rsbs r0, r1, #0", 2); }

    /* Data processing: CMP r0, r1 → hw=0x4288 (op4=10) */
    { const uint8_t b[] = {0x88, 0x42};
      chk("cmp r0, r1 (dp)", 0x1000, b, 2, NULL, "cmp r0, r1", 2); }

    /* Data processing: ORRS r0, r1 → hw=0x4308 (op4=12) */
    { const uint8_t b[] = {0x08, 0x43};
      chk("orrs r0, r1", 0x1000, b, 2, NULL, "orrs r0, r1", 2); }

    /* Data processing: MULS r0, r1, r0 → hw=0x4348 (op4=13) */
    { const uint8_t b[] = {0x48, 0x43};
      chk("muls r0, r1, r0", 0x1000, b, 2, NULL, "muls r0, r1, r0", 2); }

    /* Data processing: BICS r0, r1 → hw=0x4388 (op4=14) */
    { const uint8_t b[] = {0x88, 0x43};
      chk("bics r0, r1", 0x1000, b, 2, NULL, "bics r0, r1", 2); }

    /* Data processing: MVNS r0, r1 → hw=0x43C8 (op4=15) */
    { const uint8_t b[] = {0xC8, 0x43};
      chk("mvns r0, r1", 0x1000, b, 2, NULL, "mvns r0, r1", 2); }

    /* Special data: ADD hi-reg, ADD r0, r8 → hw=0x4440 (op2=0,rdn=0,rm=8) */
    { const uint8_t b[] = {0x40, 0x44};
      chk("add r0, r8 (hi-reg)", 0x1000, b, 2, NULL, "add r0, r8", 2); }

    /* Special data: MOV T2 (hi-reg), MOV r0, r1 → hw=0x4608 */
    { const uint8_t b[] = {0x08, 0x46};
      chk("mov r0, r1 (hi-reg)", 0x1000, b, 2, NULL, "mov r0, r1", 2); }

    /* BX LR → hw=0x4770 */
    { const uint8_t b[] = {0x70, 0x47};
      chk("bx lr", 0x1000, b, 2, NULL, "bx lr", 2); }

    /* LDR literal T1: ldr r0, [pc, #0] → hw=0x4800, pc=0x2000
     * addr = (0x2000&~3)+4+0 = 0x2004 */
    { const uint8_t b[] = {0x00, 0x48};
      chk("ldr r0, [pc, #0]", 0x2000, b, 2, NULL,
          "ldr r0, [pc, #0]  ; 0x00002004", 2); }

    /* LDR literal T1: ldr r1, [pc, #4] → hw=0x4901, pc=0x1000
     * addr = (0x1000&~3)+4+4 = 0x1008 */
    { const uint8_t b[] = {0x01, 0x49};
      chk("ldr r1, [pc, #4]", 0x1000, b, 2, NULL,
          "ldr r1, [pc, #4]  ; 0x00001008", 2); }

    /* STR register offset: str r0, [r1, r2] → hw=0x5088 */
    { const uint8_t b[] = {0x88, 0x50};
      chk("str r0, [r1, r2]", 0x1000, b, 2, NULL, "str r0, [r1, r2]", 2); }

    /* LDR register offset: ldr r0, [r1, r2] → hw=0x5888 */
    { const uint8_t b[] = {0x88, 0x58};
      chk("ldr r0, [r1, r2]", 0x1000, b, 2, NULL, "ldr r0, [r1, r2]", 2); }

    /* STR word immediate: str r0, [r1, #4] → hw=0x6048 (load=0,byte=0,imm5=1,off=4) */
    { const uint8_t b[] = {0x48, 0x60};
      chk("str r0, [r1, #4]", 0x1000, b, 2, NULL, "str r0, [r1, #4]", 2); }

    /* LDR word immediate: ldr r0, [r1, #4] → hw=0x6848 (load=1,byte=0,imm5=1) */
    { const uint8_t b[] = {0x48, 0x68};
      chk("ldr r0, [r1, #4]", 0x1000, b, 2, NULL, "ldr r0, [r1, #4]", 2); }

    /* LDR word zero offset: ldr r0, [r1] → hw=0x6808 */
    { const uint8_t b[] = {0x08, 0x68};
      chk("ldr r0, [r1]", 0x1000, b, 2, NULL, "ldr r0, [r1]", 2); }

    /* LDRB immediate: ldrb r0, [r1, #1] → hw=0x7848 (byte=1,load=1,imm5=1) */
    { const uint8_t b[] = {0x48, 0x78};
      chk("ldrb r0, [r1, #1]", 0x1000, b, 2, NULL, "ldrb r0, [r1, #1]", 2); }

    /* STRH immediate zero offset: strh r0, [r1] → hw=0x8008 */
    { const uint8_t b[] = {0x08, 0x80};
      chk("strh r0, [r1]", 0x1000, b, 2, NULL, "strh r0, [r1]", 2); }

    /* LDRH immediate: ldrh r0, [r1, #2] → hw=0x8848 (load=1,imm5=1,off=2) */
    { const uint8_t b[] = {0x48, 0x88};
      chk("ldrh r0, [r1, #2]", 0x1000, b, 2, NULL, "ldrh r0, [r1, #2]", 2); }

    /* STR SP-relative: str r0, [sp] → hw=0x9000 */
    { const uint8_t b[] = {0x00, 0x90};
      chk("str r0, [sp]", 0x1000, b, 2, NULL, "str r0, [sp]", 2); }

    /* LDR SP-relative: ldr r0, [sp, #8] → hw=0x9802 (imm8=2,off=8) */
    { const uint8_t b[] = {0x02, 0x98};
      chk("ldr r0, [sp, #8]", 0x1000, b, 2, NULL, "ldr r0, [sp, #8]", 2); }

    /* ADR T1: adr r0, ... pc=0x2000, imm8=0 → addr=0x2004 */
    { const uint8_t b[] = {0x00, 0xA0};
      chk("adr r0 (imm8=0)", 0x2000, b, 2, NULL, "adr r0, 0x00002004", 2); }

    /* ADD SP T1: add r1, sp, #4 → hw=0xA901 (rd=1,imm8=1) */
    { const uint8_t b[] = {0x01, 0xA9};
      chk("add r1, sp, #4", 0x1000, b, 2, NULL, "add r1, sp, #4", 2); }

    /* ADD SP T2: add sp, sp, #8 → hw=0xB002 (imm7=2) */
    { const uint8_t b[] = {0x02, 0xB0};
      chk("add sp, sp, #8", 0x1000, b, 2, NULL, "add sp, sp, #8", 2); }

    /* SUB SP T1: sub sp, sp, #8 → hw=0xB082 (imm7=2) */
    { const uint8_t b[] = {0x82, 0xB0};
      chk("sub sp, sp, #8", 0x1000, b, 2, NULL, "sub sp, sp, #8", 2); }

    /* PUSH T1: push {r4, lr} → hw=0xB510 */
    { const uint8_t b[] = {0x10, 0xB5};
      chk("push {r4, lr}", 0x1000, b, 2, NULL, "push {r4, lr}", 2); }

    /* PUSH T1: push {r4, r5, r6, r7} → hw=0xB4F0 (list8=0xF0) */
    { const uint8_t b[] = {0xF0, 0xB4};
      chk("push {r4,r5,r6,r7}", 0x1000, b, 2, NULL,
          "push {r4, r5, r6, r7}", 2); }

    /* POP T1: pop {r4, pc} → hw=0xBD10 */
    { const uint8_t b[] = {0x10, 0xBD};
      chk("pop {r4, pc}", 0x1000, b, 2, NULL, "pop {r4, pc}", 2); }

    /* BKPT: bkpt #7 → hw=0xBE07 */
    { const uint8_t b[] = {0x07, 0xBE};
      chk("bkpt #7", 0x1000, b, 2, NULL, "bkpt #7", 2); }

    /* NOP hint: nop → hw=0xBF00 */
    { const uint8_t b[] = {0x00, 0xBF};
      chk("nop", 0x1000, b, 2, NULL, "nop", 2); }

    /* Sign extend: sxth r0, r1 → hw=0xB208 */
    { const uint8_t b[] = {0x08, 0xB2};
      chk("sxth r0, r1", 0x1000, b, 2, NULL, "sxth r0, r1", 2); }

    /* Zero extend: uxtb r0, r1 → hw=0xB2C8 (op2=3) */
    { const uint8_t b[] = {0xC8, 0xB2};
      chk("uxtb r0, r1", 0x1000, b, 2, NULL, "uxtb r0, r1", 2); }

    /* STMIA T1: stmia r0!, {r1, r2} → hw=0xC006 (load=0,rn=0,list8=0x06) */
    { const uint8_t b[] = {0x06, 0xC0};
      chk("stmia r0!, {r1, r2}", 0x1000, b, 2, NULL,
          "stmia r0!, {r1, r2}", 2); }

    /* LDMIA T1: ldmia r1!, {r0} → hw=0xC901 (load=1,rn=1,list8=0x01) */
    { const uint8_t b[] = {0x01, 0xC9};
      chk("ldmia r1!, {r0}", 0x1000, b, 2, NULL, "ldmia r1!, {r0}", 2); }

    /* SVC: svc #0 → hw=0xDF00 (cond=0xF in B block) */
    { const uint8_t b[] = {0x00, 0xDF};
      chk("svc #0", 0x1000, b, 2, NULL, "svc #0", 2); }

    /* B conditional T1: beq 0x00001004 (off8=0, pc=0x1000) → hw=0xD000 */
    { const uint8_t b[] = {0x00, 0xD0};
      chk("beq 0x00001004", 0x1000, b, 2, NULL, "beq 0x00001004", 2); }

    /* B conditional T1: bne 0x00001004 (off8=0, pc=0x1000) → hw=0xD100 */
    { const uint8_t b[] = {0x00, 0xD1};
      chk("bne 0x00001004", 0x1000, b, 2, NULL, "bne 0x00001004", 2); }

    /* B conditional T1: blt backward (off8=-2, pc=0x1000)
     * off8 = -2 (0xFE signed), dest = 0x1000+4+(-2)*2 = 0x1000 */
    { const uint8_t b[] = {0xFE, 0xDB};
      chk("blt 0x00001000 (back)", 0x1000, b, 2, NULL, "blt 0x00001000", 2); }

    /* B unconditional T2: b 0x00001004 (off11=0, pc=0x1000) → hw=0xE000 */
    { const uint8_t b[] = {0x00, 0xE0};
      chk("b 0x00001004", 0x1000, b, 2, NULL, "b 0x00001004", 2); }
}

/* ── 32-bit instruction tests ─────────────────────────────────────────────── */

