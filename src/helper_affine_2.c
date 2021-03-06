/*
 * helper_affine_2.c -- Compute a bivariate affine function.
 *
 * Copyright 2016-2020 James Paul Turner.
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

#include "arpra-impl.h"

void arpra_helper_affine_2 (arpra_range *y, const arpra_range *x1, const arpra_range *x2,
                            mpfi_srcptr alpha, mpfi_srcptr beta, mpfi_srcptr gamma, mpfr_srcptr delta)
{
    mpfr_t temp, error;
    arpra_range yy;
    arpra_prec prec_internal;
    arpra_uint i_y, i_x1, i_x2;

    // Initialise vars.
    prec_internal = arpra_get_internal_precision();
    mpfr_init2(temp, prec_internal);
    mpfr_init2(error, prec_internal);
    arpra_init2(&yy, y->precision);
    mpfr_set_zero(error, 1);

    // y[0] = (alpha * x1[0]) + (beta * x2[0]) + (gamma)
    arpra_helper_term_fmmaa(error, &(yy.centre), &(x1->centre), &(x2->centre), alpha, beta, gamma);

    // Allocate memory for deviation terms.
    yy.symbols = malloc((x1->nTerms + x2->nTerms + 1) * sizeof(arpra_uint));
    yy.deviations = malloc((x1->nTerms + x2->nTerms + 1) * sizeof(mpfr_t));

    for (i_y = 0, i_x1 = 0, i_x2 = 0; (i_x1 < x1->nTerms) || (i_x2 < x2->nTerms); i_y++) {
        mpfr_init2(&(yy.deviations[i_y]), prec_internal);

        if ((i_x2 == x2->nTerms) || ((i_x1 < x1->nTerms) && (x1->symbols[i_x1] < x2->symbols[i_x2]))) {
            // y[i] = (alpha * x1[i])
            yy.symbols[i_y] = x1->symbols[i_x1];
            arpra_helper_term_mul(error, &(yy.deviations[i_y]), &(x1->deviations[i_x1]), alpha);
            i_x1++;
        }
        else if ((i_x1 == x1->nTerms) || ((i_x2 < x2->nTerms) && (x2->symbols[i_x2] < x1->symbols[i_x1]))) {
            // y[i] = (beta * x2[i])
            yy.symbols[i_y] = x2->symbols[i_x2];
            arpra_helper_term_mul(error, &(yy.deviations[i_y]), &(x2->deviations[i_x2]), beta);
            i_x2++;
        }
        else {
            // y[i] = (alpha * x1[i]) + (beta * x2[i])
            yy.symbols[i_y] = x1->symbols[i_x1];
            arpra_helper_term_fmma(error, &(yy.deviations[i_y]), &(x1->deviations[i_x1]), &(x2->deviations[i_x2]), alpha, beta);
            i_x1++;
            i_x2++;
        }
    }

    // Add delta to error.
    mpfr_add(error, error, delta, MPFR_RNDU);

    // Store new deviation term.
    yy.symbols[i_y] = arpra_helper_next_symbol();
    yy.deviations[i_y] = *error;
    yy.nTerms = i_y + 1;

    // Clear vars, and set y.
    mpfr_clear(temp);
    arpra_clear(y);
    *y = yy;
}
