/*
 * t_add.c -- Test the arpra_add function.
 *
 * Copyright 2017-2018 James Paul Turner.
 *
 * This file is part of the Arpra library.
 *
 * The Arpra library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The Arpra library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the Arpra library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "arpra-test.h"

int main (int argc, char *argv[])
{
    const arpra_prec prec = 24;
    const arpra_prec prec_internal = 256;
    const arpra_uint test_n = 100000;
    unsigned i, fail, fail_n;

    FILE *shared_log, *partshared_log, *unshared_log;
    shared_log = fopen("add_shared.log", "w");
    partshared_log = fopen("add_partshared.log", "w");
    unshared_log = fopen("add_unshared.log", "w");

    // Init test.
    test_fixture_init(prec, prec_internal);
    test_log_init("add");
    test_rand_init();
    fail_n = 0;

    // Run test.
    for (i = 0; i < test_n; i++) {
        fail = 0;
        test_rand_arpra(&x_A, TEST_RAND_MIXED, TEST_RAND_SMALL);
        test_rand_arpra(&y_A, TEST_RAND_MIXED, TEST_RAND_SMALL);

        // Pass criteria (unshared symbols):
        // 1) Arpra z contains MPFI z.
        // 2) Arpra z unbounded and MPFI z unbounded.
        test_bivariate(arpra_add, mpfi_add);
        if (mpfr_greaterequal_p(&(z_I.left), &(z_A.true_range.left))
                && mpfr_lessequal_p(&(z_I.right), &(z_A.true_range.right))) {
            test_log_printf("Result (unshared symbols): PASS\n\n");
        }
        else if (!arpra_bounded_p(&z_A) && !mpfi_bounded_p(&z_I)) {
            test_log_printf("Result (unshared symbols): PASS\n\n");
        }
        else {
            test_log_printf("Result (unshared symbols): FAIL\n\n");
            fail = 1;
        }

        mpfr_out_str(unshared_log, 10, 40, &z_A_diam_rel, MPFR_RNDN);
        fputs("\n", unshared_log);

        // Pass criteria (random shared symbols):
        // 1) bounded(Arpra z) = bounded(MPFI z).
        test_share_n_syms(&x_A, &y_A, 3);
        test_bivariate(arpra_add, mpfi_add);
        if (arpra_bounded_p(&z_A) == mpfi_bounded_p(&z_I)) {
            test_log_printf("Result (random shared symbols): PASS\n\n");
        }
        else {
            test_log_printf("Result (random shared symbols): FAIL\n\n");
            fail = 1;
        }

        mpfr_out_str(partshared_log, 10, 40, &z_A_diam_rel, MPFR_RNDN);
        fputs("\n", partshared_log);

        // Pass criteria (all shared symbols):
        // 1) bounded(Arpra z) = bounded(MPFI z).
        test_share_all_syms(&x_A, &y_A);
        test_bivariate(arpra_add, mpfi_add);
        if (arpra_bounded_p(&z_A) == mpfi_bounded_p(&z_I)) {
            test_log_printf("Result (all shared symbols): PASS\n\n");
        }
        else {
            test_log_printf("Result (all shared symbols): FAIL\n\n");
            fail = 1;
        }

        mpfr_out_str(shared_log, 10, 40, &z_A_diam_rel, MPFR_RNDN);
        fputs("\n", shared_log);

        if (fail) fail_n++;
    }

    fclose(shared_log);
    fclose(partshared_log);
    fclose(unshared_log);

    // Cleanup test.
    printf("%lu out of %lu failed.\n", fail_n, test_n);
    test_fixture_clear();
    test_log_clear();
    test_rand_clear();
    return fail_n > 0;
}
