/*
 * sub.c -- Subtract one arpra_t from another.
 *
 * Copyright 2016-2017 James Paul Turner.
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

void arpra_sub (arpra_ptr z, arpra_srcptr x, arpra_srcptr y)
{
    mpfr_t alpha, beta, gamma, delta;
    arpra_prec_t prec;

    // Initialise vars.
    prec = arpra_get_prec(z);
    mpfr_init2(alpha, prec);
    mpfr_set_si(alpha, 1, MPFR_RNDN);
    mpfr_init2(beta, prec);
    mpfr_set_si(beta, -1, MPFR_RNDN);
    mpfr_init2(gamma, prec);
    mpfr_set_si(gamma, 0, MPFR_RNDN);
    mpfr_init2(delta, prec);
    mpfr_set_si(delta, 0, MPFR_RNDN);

    // z = x - y
    arpra_affine_2(z, x, y, alpha, beta, gamma, delta);

    // Clear vars.
    mpfr_clear(alpha);
    mpfr_clear(beta);
    mpfr_clear(gamma);
    mpfr_clear(delta);
}
