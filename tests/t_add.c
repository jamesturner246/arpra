/*
 * t_add.c -- Test the MPFA add function.
 *
 * Copyright 2017 James Paul Turner.
 *
 * This file is part of the MPFA library.
 *
 * The MPFA library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The MPFA library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the MPFA library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "mpfa-test.h"

int main (int argc, char *argv[])
{
    const mpfa_prec_t prec = 53;
    const mpfa_int_t n_tests = 1000;
    mpfa_int_t i, n_failed = 0;

#ifdef WITH_MPFI
    mpfa_t a_a, b_a, c_a;
    mpfi_t a_i, b_i, c_i;

    mpfa_set_default_prec(53);
    mpfa_set_internal_prec(53);

    mpfa_test_rand_init();
    mpfa_init(a_a);
    mpfa_init(b_a);
    mpfa_init(c_a);
    //mpfi_init(a_i);
    //mpfi_init(b_i);
    //mpfi_init(c_i);
    mpfi_init2(a_i, 54);
    mpfi_init2(b_i, 54);
    mpfi_init2(c_i, 53);

    mpfr_t cen, rad;
    mpfr_init(cen);
    mpfr_init(rad);

    for (i = 0; i < n_tests; i++) {
        printf("i=%i\n", i);

        //mpfr_set_d(cen, 0.35443, MPFR_RNDN);
        //mpfr_set_d(rad, 0.12426452533, MPFR_RNDN);
        //mpfa_set_mpfr_rad(a_a, cen, rad);
        //mpfr_set_d(cen, 0.25754556, MPFR_RNDN);
        //mpfr_set_d(rad, 0.2353253235, MPFR_RNDN);
        //mpfa_set_mpfr_rad(b_a, cen, rad);

        /*
        mpfa_test_rand_mpfr(cen, MPFA_TEST_RAND_SMALL);
        mpfa_test_rand_mpfr(rad, MPFA_TEST_RAND_SMALL);
        mpfa_set_mpfr_rad(a_a, cen, rad);
        mpfa_test_rand_mpfr(cen, MPFA_TEST_RAND_SMALL);
        mpfa_test_rand_mpfr(rad, MPFA_TEST_RAND_SMALL);
        mpfa_set_mpfr_rad(b_a, cen, rad);
        //*/

        //*
        mpfa_test_rand_mpfa(a_a, MPFA_TEST_RAND_SMALL);
        mpfa_test_rand_mpfa(b_a, MPFA_TEST_RAND_SMALL);
        //*/

        mpfa_get_mpfi(a_i, a_a);
        mpfa_get_mpfi(b_i, b_a);

        mpfa_add(c_a, a_a, b_a);
        mpfi_add(c_i, a_i, b_i);

        n_failed += mpfa_test_cmp_mpfi(c_a, c_i);
    }

    printf("Failed %i out of %i.\n", n_failed, n_tests);

    mpfr_clear(cen);
    mpfr_clear(rad);

    mpfa_test_rand_clear();
    mpfa_clear(a_a);
    mpfa_clear(b_a);
    mpfa_clear(c_a);
    mpfi_clear(a_i);
    mpfi_clear(b_i);
    mpfi_clear(c_i);
#endif // WITH_MPFI

    mpfr_free_cache();
    return n_failed;
}
