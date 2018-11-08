/*
 * add.c -- Add one Arpra range to another.
 *
 * Copyright 2016-2018 James Paul Turner.
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

void arpra_add (arpra_range *z, const arpra_range *x, const arpra_range *y)
{
    arpra_mpfr alpha, beta, gamma, delta;
    arpra_prec prec;

    // Initialise vars.
    prec = arpra_get_precision(z);
    mpfr_init2(&alpha, prec);
    mpfr_set_si(&alpha, 1, MPFR_RNDN);
    mpfr_init2(&beta, prec);
    mpfr_set_si(&beta, 1, MPFR_RNDN);
    mpfr_init2(&gamma, prec);
    mpfr_set_si(&gamma, 0, MPFR_RNDN);
    mpfr_init2(&delta, prec);
    mpfr_set_si(&delta, 0, MPFR_RNDN);

    // z = x + y
    arpra_affine_2(z, x, y, &alpha, &beta, &gamma, &delta);

    // Clear vars.
    mpfr_clear(&alpha);
    mpfr_clear(&beta);
    mpfr_clear(&gamma);
    mpfr_clear(&delta);
}
