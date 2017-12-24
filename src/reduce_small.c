/*
 * reduce_small.c -- Reduce terms less than some fraction of the radius.
 *
 * Copyright 2017 James Paul Turner.
 *
 * This file is part of the ArPRA library.
 *
 * The ArPRA library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The ArPRA library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the ArPRA library. If not, see <http://www.gnu.org/licenses/>.
 */

#include "arpra-impl.h"

void arpra_reduce_small (arpra_ptr z, double fraction)
{
    arpra_uint_t zTerm, zNext;
    mpfr_t temp, error, threshold;
    arpra_prec_t prec_internal;

    // Handle trivial cases.
    if ((z->nTerms < 2) || (fraction >= 1)) return;

    // Handle domain violations.
    if (arpra_nan_p(z)) return;
    if (arpra_inf_p(z)) return;

    // Initialise vars.
    prec_internal = arpra_get_internal_prec();
    mpfr_init2(temp, prec_internal);
    mpfr_init2(threshold, prec_internal);
    mpfr_mul_d(threshold, &(z->radius), fraction, MPFR_RNDN);
    mpfr_init2(error, prec_internal);
    mpfr_set_ui(error, 0, MPFR_RNDU);
    mpfr_set_prec(&(z->radius), prec_internal);
    mpfr_set_ui(&(z->radius), 0, MPFR_RNDU);
    zTerm = 0;

    for (zNext = 0; zNext < z->nTerms; zNext++) {
        mpfr_abs(temp, &(z->deviations[zNext]), MPFR_RNDU);

        if (mpfr_lessequal_p(temp, threshold)) {
            // If deviation term is small, merge it.
            mpfr_add(error, error, temp, MPFR_RNDU);
        }
        else {
            // Else shift deviation term up.
            if (zTerm < zNext) {
                z->symbols[zTerm] = z->symbols[zNext];
                mpfr_set(&(z->deviations[zTerm]), &(z->deviations[zNext]), MPFR_RNDN);
            }
            mpfr_add(&(z->radius), &(z->radius), &(z->deviations[zTerm]), MPFR_RNDU);
            zTerm++;
        }
    }

    // Store nonzero merged deviation term.
    if (!mpfr_zero_p(error)) {
        z->symbols[zTerm] = arpra_next_sym();
        mpfr_set(&(z->deviations[zTerm]), error, MPFR_RNDU);
        mpfr_add(&(z->radius), &(z->radius), &(z->deviations[zTerm]), MPFR_RNDU);
        zTerm++;
    }

    // Clear unused deviation terms.
    for (zNext = zTerm; zNext < z->nTerms; zNext++) {
        mpfr_clear(&(z->deviations[zNext]));
    }

    // Handle domain violations, and resize memory.
    z->nTerms = zTerm;
    if (mpfr_nan_p(&(z->centre)) || mpfr_nan_p(&(z->radius))) {
        arpra_set_nan(z);
    }
    else if (mpfr_inf_p(&(z->centre)) || mpfr_inf_p(&(z->radius))) {
        arpra_set_inf(z);
    }
    else {
        if (z->nTerms == 0) {
            free(z->symbols);
            free(z->deviations);
        }
        else {
            z->symbols = realloc(z->symbols, z->nTerms * sizeof(arpra_uint_t));
            z->deviations = realloc(z->deviations, z->nTerms * sizeof(mpfr_t));
        }
    }

    // Clear vars.
    mpfr_clear(temp);
    mpfr_clear(error);
    mpfr_clear(threshold);
}
