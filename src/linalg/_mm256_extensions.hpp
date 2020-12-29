#ifndef MM256_EXP_PS
#define MM256_EXP_PS

#include <immintrin.h> 

__m256 _mm256_mod_ps(__m256 lhs, __m256 rhs) {
    // a % b = a - b * floor(a / b)
    __m256 q = _mm256_div_ps(lhs, rhs);
    q = _mm256_floor_ps(q);
    q = _mm256_mul_ps(q, rhs);
    return _mm256_sub_ps(lhs, q);
}

__m256 _mm256_abs_ps(__m256 x) {
    return _mm256_andnot_ps(_mm256_set1_ps(-0.), x);
}

// Source: https://stackoverflow.com/a/49090523
__m256 _mm256_exp_ps(__m256 x) {
    __m256 t, f, p, r;
    __m256i i, j;

    const __m256 l2e = _mm256_set1_ps (1.442695041f); /* log2(e) */
    const __m256 l2h = _mm256_set1_ps (-6.93145752e-1f); /* -log(2)_hi */
    const __m256 l2l = _mm256_set1_ps (-1.42860677e-6f); /* -log(2)_lo */
    /* coefficients for core approximation to exp() in [-log(2)/2, log(2)/2] */
    const __m256 c0 =  _mm256_set1_ps (0.041944388f);
    const __m256 c1 =  _mm256_set1_ps (0.168006673f);
    const __m256 c2 =  _mm256_set1_ps (0.499999940f);
    const __m256 c3 =  _mm256_set1_ps (0.999956906f);
    const __m256 c4 =  _mm256_set1_ps (0.999999642f);

    /* exp(x) = 2^i * e^f; i = rint (log2(e) * x), f = x - log(2) * i */
    t = _mm256_mul_ps (x, l2e);      /* t = log2(e) * x */
    r = _mm256_round_ps (t, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC); /* r = rint (t) */

    p = _mm256_mul_ps (r, l2h);      /* log(2)_hi * r */
    f = _mm256_add_ps (x, p);        /* x - log(2)_hi * r */
    p = _mm256_mul_ps (r, l2l);      /* log(2)_lo * r */
    f = _mm256_add_ps (f, p);        /* f = x - log(2)_hi * r - log(2)_lo * r */
    i = _mm256_cvtps_epi32(t);       /* i = (int)rint(t) */

    /* p ~= exp (f), -log(2)/2 <= f <= log(2)/2 */
    p = c0;                          /* c0 */
    p = _mm256_mul_ps (p, f);        /* c0*f */
    p = _mm256_add_ps (p, c1);       /* c0*f+c1 */
    p = _mm256_mul_ps (p, f);        /* (c0*f+c1)*f */
    p = _mm256_add_ps (p, c2);       /* (c0*f+c1)*f+c2 */
    p = _mm256_mul_ps (p, f);        /* ((c0*f+c1)*f+c2)*f */
    p = _mm256_add_ps (p, c3);       /* ((c0*f+c1)*f+c2)*f+c3 */
    p = _mm256_mul_ps (p, f);        /* (((c0*f+c1)*f+c2)*f+c3)*f */
    p = _mm256_add_ps (p, c4);       /* (((c0*f+c1)*f+c2)*f+c3)*f+c4 ~= exp(f) */
    
    /* exp(x) = 2^i * p */
    j = _mm256_slli_epi32 (i, 23); /* i << 23 */
    r = _mm256_castsi256_ps (_mm256_add_epi32 (j, _mm256_castps_si256 (p))); /* r = p * 2^i */

    return r;
}


// source: https://jrfonseca.blogspot.com/2008/09/fast-sse2-pow-tables-or-polynomials.html
#define EXP_POLY_DEGREE 3

#define POLY0(x, c0) _mm256_set1_ps(c0)
#define POLY1(x, c0, c1) _mm256_add_ps(_mm256_mul_ps(POLY0(x, c1), x), _mm256_set1_ps(c0))
#define POLY2(x, c0, c1, c2) _mm256_add_ps(_mm256_mul_ps(POLY1(x, c1, c2), x), _mm256_set1_ps(c0))
#define POLY3(x, c0, c1, c2, c3) _mm256_add_ps(_mm256_mul_ps(POLY2(x, c1, c2, c3), x), _mm256_set1_ps(c0))
#define POLY4(x, c0, c1, c2, c3, c4) _mm256_add_ps(_mm256_mul_ps(POLY3(x, c1, c2, c3, c4), x), _mm256_set1_ps(c0))
#define POLY5(x, c0, c1, c2, c3, c4, c5) _mm256_add_ps(_mm256_mul_ps(POLY4(x, c1, c2, c3, c4, c5), x), _mm256_set1_ps(c0))

__m256 _mm256_exp2_ps(__m256 x) {
   __m256i ipart;
   __m256 fpart, expipart, expfpart;

   x = _mm256_min_ps(x, _mm256_set1_ps( 129.00000f));
   x = _mm256_max_ps(x, _mm256_set1_ps(-126.99999f));

   /* ipart = int(x - 0.5) */
   ipart = _mm256_cvtps_epi32(_mm256_sub_ps(x, _mm256_set1_ps(0.5f)));

   /* fpart = x - ipart */
   fpart = _mm256_sub_ps(x, _mm256_cvtepi32_ps(ipart));

   /* expipart = (float) (1 << ipart) */
   expipart = _mm256_castsi256_ps(_mm256_slli_epi32(_mm256_add_epi32(ipart, _mm256_set1_epi32(127)), 23));

   /* minimax polynomial fit of 2**x, in range [-0.5, 0.5[ */
#if EXP_POLY_DEGREE == 5
   expfpart = POLY5(fpart, 9.9999994e-1f, 6.9315308e-1f, 2.4015361e-1f, 5.5826318e-2f, 8.9893397e-3f, 1.8775767e-3f);
#elif EXP_POLY_DEGREE == 4
   expfpart = POLY4(fpart, 1.0000026f, 6.9300383e-1f, 2.4144275e-1f, 5.2011464e-2f, 1.3534167e-2f);
#elif EXP_POLY_DEGREE == 3
   expfpart = POLY3(fpart, 9.9992520e-1f, 6.9583356e-1f, 2.2606716e-1f, 7.8024521e-2f);
#elif EXP_POLY_DEGREE == 2
   expfpart = POLY2(fpart, 1.0017247f, 6.5763628e-1f, 3.3718944e-1f);
#else
#error
#endif

   return _mm256_mul_ps(expipart, expfpart);
}

#define LOG_POLY_DEGREE 5

__m256 _mm256_log2_ps(__m256 x)
{
   __m256i exp = _mm256_set1_epi32(0x7F800000);
   __m256i mant = _mm256_set1_epi32(0x007FFFFF);

   __m256 one = _mm256_set1_ps( 1.0f);

   __m256i i = _mm256_castps_si256(x);

    

   __m256 e = _mm256_cvtepi32_ps(_mm256_sub_epi32(_mm256_srli_epi32(_mm256_and_si256(i, exp), 23), _mm256_set1_epi32(127)));

   __m256 m = _mm256_or_ps(_mm256_castsi256_ps(_mm256_and_si256(i, mant)), one);

   __m256 p;

   /* Minimax polynomial fit of log2(x)/(x - 1), for x in range [1, 2[ */
#if LOG_POLY_DEGREE == 6
   p = POLY5( m, 3.1157899f, -3.3241990f, 2.5988452f, -1.2315303f,  3.1821337e-1f, -3.4436006e-2f);
#elif LOG_POLY_DEGREE == 5
   p = POLY4(m, 2.8882704548164776201f, -2.52074962577807006663f, 1.48116647521213171641f, -0.465725644288844778798f, 0.0596515482674574969533f);
#elif LOG_POLY_DEGREE == 4
   p = POLY3(m, 2.61761038894603480148f, -1.75647175389045657003f, 0.688243882994381274313f, -0.107254423828329604454f);
#elif LOG_POLY_DEGREE == 3
   p = POLY2(m, 2.28330284476918490682f, -1.04913055217340124191f, 0.204446009836232697516f);
#else
#error
#endif

   /* This effectively increases the polynomial degree by one, but ensures that log2(1) == 0*/
   p = _mm256_mul_ps(p, _mm256_sub_ps(m, one));

   return _mm256_add_ps(p, e);
}

static inline __m256 _mm256_pow_ps(__m256 x, __m256 y) {
   return _mm256_exp2_ps(_mm256_mul_ps(_mm256_log2_ps(x), y));
}

#endif