/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright (c) 2021 Loongson Technology Corporation Limited
 */

#ifndef CONFIG_USER_ONLY
#define CHECK_FPE do { \
    if ((ctx->base.tb->flags & HW_FLAGS_EUEN_FPE) == 0) { \
        generate_exception(ctx, EXCCODE_FPD); \
        return true; \
    } \
} while (0)
#else
#define CHECK_FPE
#endif

static bool gen_fff(DisasContext *ctx, arg_fff *a,
                    void (*func)(TCGv, TCGv_env, TCGv, TCGv))
{
    TCGv dest = get_fpr(ctx, a->fd);
    TCGv src1 = get_fpr(ctx, a->fj);
    TCGv src2 = get_fpr(ctx, a->fk);

    CHECK_FPE;

    func(dest, cpu_env, src1, src2);
    set_fpr(a->fd, dest);

    return true;
}

static bool gen_ff(DisasContext *ctx, arg_ff *a,
                   void (*func)(TCGv, TCGv_env, TCGv))
{
    TCGv dest = get_fpr(ctx, a->fd);
    TCGv src = get_fpr(ctx, a->fj);

    CHECK_FPE;

    func(dest, cpu_env, src);
    set_fpr(a->fd, dest);

    return true;
}

static bool gen_muladd(DisasContext *ctx, arg_ffff *a,
                       void (*func)(TCGv, TCGv_env, TCGv, TCGv, TCGv, TCGv_i32),
                       int flag)
{
    TCGv_i32 tflag = tcg_constant_i32(flag);
    TCGv dest = get_fpr(ctx, a->fd);
    TCGv src1 = get_fpr(ctx, a->fj);
    TCGv src2 = get_fpr(ctx, a->fk);
    TCGv src3 = get_fpr(ctx, a->fa);

    CHECK_FPE;

    func(dest, cpu_env, src1, src2, src3, tflag);
    set_fpr(a->fd, dest);

    return true;
}

static bool trans_fcopysign_s(DisasContext *ctx, arg_fcopysign_s *a)
{
    TCGv dest = get_fpr(ctx, a->fd);
    TCGv src1 = get_fpr(ctx, a->fk);
    TCGv src2 = get_fpr(ctx, a->fj);

    CHECK_FPE;

    tcg_gen_deposit_i64(dest, src1, src2, 0, 31);
    set_fpr(a->fd, dest);

    return true;
}

static bool trans_fcopysign_d(DisasContext *ctx, arg_fcopysign_d *a)
{
    TCGv dest = get_fpr(ctx, a->fd);
    TCGv src1 = get_fpr(ctx, a->fk);
    TCGv src2 = get_fpr(ctx, a->fj);

    CHECK_FPE;

    tcg_gen_deposit_i64(dest, src1, src2, 0, 63);
    set_fpr(a->fd, dest);

    return true;
}

static bool trans_fabs_s(DisasContext *ctx, arg_fabs_s *a)
{
    TCGv dest = get_fpr(ctx, a->fd);
    TCGv src = get_fpr(ctx, a->fj);

    CHECK_FPE;

    tcg_gen_andi_i64(dest, src, MAKE_64BIT_MASK(0, 31));
    gen_nanbox_s(dest, dest);
    set_fpr(a->fd, dest);

    return true;
}

static bool trans_fabs_d(DisasContext *ctx, arg_fabs_d *a)
{
    TCGv dest = get_fpr(ctx, a->fd);
    TCGv src = get_fpr(ctx, a->fj);

    CHECK_FPE;

    tcg_gen_andi_i64(dest, src, MAKE_64BIT_MASK(0, 63));
    set_fpr(a->fd, dest);

    return true;
}

static bool trans_fneg_s(DisasContext *ctx, arg_fneg_s *a)
{
    TCGv dest = get_fpr(ctx, a->fd);
    TCGv src = get_fpr(ctx, a->fj);

    CHECK_FPE;

    tcg_gen_xori_i64(dest, src, 0x80000000);
    gen_nanbox_s(dest, dest);
    set_fpr(a->fd, dest);

    return true;
}

static bool trans_fneg_d(DisasContext *ctx, arg_fneg_d *a)
{
    TCGv dest = get_fpr(ctx, a->fd);
    TCGv src = get_fpr(ctx, a->fj);

    CHECK_FPE;

    tcg_gen_xori_i64(dest, src, 0x8000000000000000LL);
    set_fpr(a->fd, dest);

    return true;
}

TRANS(fadd_s, gen_fff, gen_helper_fadd_s)
TRANS(fadd_d, gen_fff, gen_helper_fadd_d)
TRANS(fsub_s, gen_fff, gen_helper_fsub_s)
TRANS(fsub_d, gen_fff, gen_helper_fsub_d)
TRANS(fmul_s, gen_fff, gen_helper_fmul_s)
TRANS(fmul_d, gen_fff, gen_helper_fmul_d)
TRANS(fdiv_s, gen_fff, gen_helper_fdiv_s)
TRANS(fdiv_d, gen_fff, gen_helper_fdiv_d)
TRANS(fmax_s, gen_fff, gen_helper_fmax_s)
TRANS(fmax_d, gen_fff, gen_helper_fmax_d)
TRANS(fmin_s, gen_fff, gen_helper_fmin_s)
TRANS(fmin_d, gen_fff, gen_helper_fmin_d)
TRANS(fmaxa_s, gen_fff, gen_helper_fmaxa_s)
TRANS(fmaxa_d, gen_fff, gen_helper_fmaxa_d)
TRANS(fmina_s, gen_fff, gen_helper_fmina_s)
TRANS(fmina_d, gen_fff, gen_helper_fmina_d)
TRANS(fscaleb_s, gen_fff, gen_helper_fscaleb_s)
TRANS(fscaleb_d, gen_fff, gen_helper_fscaleb_d)
TRANS(fsqrt_s, gen_ff, gen_helper_fsqrt_s)
TRANS(fsqrt_d, gen_ff, gen_helper_fsqrt_d)
TRANS(frecip_s, gen_ff, gen_helper_frecip_s)
TRANS(frecip_d, gen_ff, gen_helper_frecip_d)
TRANS(frsqrt_s, gen_ff, gen_helper_frsqrt_s)
TRANS(frsqrt_d, gen_ff, gen_helper_frsqrt_d)
TRANS(flogb_s, gen_ff, gen_helper_flogb_s)
TRANS(flogb_d, gen_ff, gen_helper_flogb_d)
TRANS(fclass_s, gen_ff, gen_helper_fclass_s)
TRANS(fclass_d, gen_ff, gen_helper_fclass_d)
TRANS(fmadd_s, gen_muladd, gen_helper_fmuladd_s, 0)
TRANS(fmadd_d, gen_muladd, gen_helper_fmuladd_d, 0)
TRANS(fmsub_s, gen_muladd, gen_helper_fmuladd_s, float_muladd_negate_c)
TRANS(fmsub_d, gen_muladd, gen_helper_fmuladd_d, float_muladd_negate_c)
TRANS(fnmadd_s, gen_muladd, gen_helper_fmuladd_s, float_muladd_negate_result)
TRANS(fnmadd_d, gen_muladd, gen_helper_fmuladd_d, float_muladd_negate_result)
TRANS(fnmsub_s, gen_muladd, gen_helper_fmuladd_s,
      float_muladd_negate_c | float_muladd_negate_result)
TRANS(fnmsub_d, gen_muladd, gen_helper_fmuladd_d,
      float_muladd_negate_c | float_muladd_negate_result)
