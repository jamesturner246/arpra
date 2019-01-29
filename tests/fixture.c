/*
 * fixture.c -- Initialise and clear the test fixture.
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

int test_fixture_ready = 0;
arpra_range x_A, y_A, z_A;
arpra_mpfi z_I;
arpra_mpfr z_I_diam, z_A_diam, z_A_diam_rel;

void test_fixture_init (arpra_prec prec, arpra_prec prec_internal)
{
    // Ensure that we do not double-initialise.
    if (!test_fixture_ready) {
        test_fixture_ready = 1;

        // Set internal precision.
        arpra_set_internal_precision(prec_internal);

        // Initialise Arpra variables.
        arpra_init2(&x_A, prec);
        arpra_init2(&y_A, prec);
        arpra_init2(&z_A, prec);

        // Initialise MPFI variables.
        mpfi_init2(&z_I, prec);

        // Initialise relative diameter variables.
        mpfr_init2(&z_I_diam, prec_internal);
        mpfr_init2(&z_A_diam, prec_internal);
        mpfr_init2(&z_A_diam_rel, prec_internal);
    }
    else {
        fprintf(stderr, "Error: test fixture is alreay initialised.\n");
        exit(EXIT_FAILURE);
    }
}

void test_fixture_clear ()
{
    // Ensure that we do not double-clear.
    if (test_fixture_ready) {
        test_fixture_ready = 0;

        // Clear Arpra variables.
        arpra_clear(&x_A);
        arpra_clear(&y_A);
        arpra_clear(&z_A);

        // Clear MPFI variables.
        mpfi_clear(&z_I);

        // Clear relative diameter variables.
        mpfr_clear(&z_I_diam);
        mpfr_clear(&z_A_diam);
        mpfr_clear(&z_A_diam_rel);

        // All else.
        mpfr_free_cache();
    }
    else {
        fprintf(stderr, "Error: test fixture is not initialised.\n");
        exit(EXIT_FAILURE);
    }
}
