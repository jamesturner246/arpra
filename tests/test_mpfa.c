/*
 * test_mpfa.c -- Test an affine form against another affine form.
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

int mpfa_test_cmp_mpfa (mpfa_srcptr x, mpfa_srcptr y)
{
    mpfa_uint_t term;

    // Return 1 if x and y differ.
    if (!mpfr_equal_p(&(x->centre), &(y->centre))) {
        return 1;
    }

    if (!mpfr_equal_p(&(x->radius), &(y->radius))) {
        return 1;
    }

    if (x->nTerms != y->nTerms) {
        return 1;
    }

    for (term = 0; term < x->nTerms; term++) {
        if (x->symbols[term] != y->symbols[term]) {
            return 1;
        }

        if (!mpfr_equal_p(&(x->deviations[term]), &(y->deviations[term]))) {
            return 1;
        }
    }

    // Else return 0.
    return 0;
}

void mpfa_test_rand_mpfa (mpfa_ptr z)
{
    mpfa_uint_t zTerm;

    // Set random centre and radius.
    mpfa_test_rand_mpfr(&(z->centre));
    mpfa_test_rand_mpfr(&(z->radius));

    // set random deviation terms.
    z->nTerms = mpfa_test_rand_ui(3);
    z->symbols = malloc(z->nTerms * sizeof(mpfa_uint_t));
    z->deviations = malloc(z->nTerms * sizeof(mpfr_t));
    for (zTerm = 0; zTerm < z->nTerms; zTerm++) {
        z->symbols[zTerm] = mpfa_next_sym();
        mpfa_test_rand_mpfr(&(z->deviations[zTerm]));
    }
}
