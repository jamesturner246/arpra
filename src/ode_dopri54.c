/*
 * ode_dopri54.c -- Dormand-Prince 5(4) ODE stepper.
 *
 * Copyright 2018 James Paul Turner.
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

typedef struct dopri54_scratch_struct
{
    arpra_range *k_1;
    arpra_range *k_2;
    arpra_range *k_3;
    arpra_range *k_4;
    arpra_range *k_5;
    arpra_range *k_6;
    arpra_range *k_7;
    arpra_range *k_weights_5;
    arpra_range *k_weights_4;
    arpra_range *next_t;
    arpra_range *next_x_5;
    arpra_range *next_x_4;
    arpra_range *temp;
} dopri54_scratch;

static const unsigned char dopri54_stages = 7;

static void dopri54_init (arpra_ode_stepper *stepper, arpra_ode_system *system)
{
    arpra_uint i;
    arpra_precision prec;
    dopri54_scratch *scratch;

    scratch = malloc(sizeof(dopri54_scratch));
    scratch->k_1 = malloc(system->dims * sizeof(arpra_range));
    scratch->k_2 = malloc(system->dims * sizeof(arpra_range));
    scratch->k_3 = malloc(system->dims * sizeof(arpra_range));
    scratch->k_4 = malloc(system->dims * sizeof(arpra_range));
    scratch->k_5 = malloc(system->dims * sizeof(arpra_range));
    scratch->k_6 = malloc(system->dims * sizeof(arpra_range));
    scratch->k_7 = malloc(system->dims * sizeof(arpra_range));
    scratch->k_weights_5 = malloc(dopri54_stages * sizeof(arpra_range));
    scratch->k_weights_4 = malloc(dopri54_stages * sizeof(arpra_range));
    scratch->next_t = malloc(sizeof(arpra_range));
    scratch->next_x_5 = malloc(system->dims * sizeof(arpra_range));
    scratch->next_x_4 = malloc(system->dims * sizeof(arpra_range));
    scratch->temp = malloc(sizeof(arpra_range));
    for (i = 0; i < system->dims; i++) {
        prec = arpra_get_precision(&(system->x[i]));
        arpra_init2(&(scratch->k_1[i]), prec);
        arpra_init2(&(scratch->k_2[i]), prec);
        arpra_init2(&(scratch->k_3[i]), prec);
        arpra_init2(&(scratch->k_4[i]), prec);
        arpra_init2(&(scratch->k_5[i]), prec);
        arpra_init2(&(scratch->k_6[i]), prec);
        arpra_init2(&(scratch->k_7[i]), prec);
        arpra_init2(&(scratch->next_x_5[i]), prec);
        arpra_init2(&(scratch->next_x_4[i]), prec);
    }
    prec = arpra_get_default_precision();
    for (i = 0; i < dopri54_stages; i++) {
        arpra_init2(&(scratch->k_weights_5[i]), prec);
        arpra_init2(&(scratch->k_weights_4[i]), prec);
    }
    arpra_init2(scratch->next_t, prec);
    arpra_init2(scratch->temp, prec);
    stepper->method = arpra_ode_dopri54;
    stepper->system = system;
    stepper->error = NULL;
    stepper->scratch = scratch;
}

static void dopri54_clear (arpra_ode_stepper *stepper)
{
    arpra_uint i;
    arpra_ode_system *system;
    dopri54_scratch *scratch;

    system = stepper->system;
    scratch = (dopri54_scratch *) stepper->scratch;
    for (i = 0; i < system->dims; i++) {
        arpra_clear(&(scratch->k_1[i]));
        arpra_clear(&(scratch->k_2[i]));
        arpra_clear(&(scratch->k_3[i]));
        arpra_clear(&(scratch->k_4[i]));
        arpra_clear(&(scratch->k_5[i]));
        arpra_clear(&(scratch->k_6[i]));
        arpra_clear(&(scratch->k_7[i]));
        arpra_clear(&(scratch->next_x_5[i]));
        arpra_clear(&(scratch->next_x_4[i]));
    }
    for (i = 0; i < dopri54_stages; i++) {
        arpra_clear(&(scratch->k_weights_5[i]));
        arpra_clear(&(scratch->k_weights_4[i]));
    }
    arpra_clear(scratch->next_t);
    arpra_clear(scratch->temp);
    free(scratch->k_1);
    free(scratch->k_2);
    free(scratch->k_3);
    free(scratch->k_4);
    free(scratch->k_5);
    free(scratch->k_6);
    free(scratch->k_7);
    free(scratch->k_weights_5);
    free(scratch->k_weights_4);
    free(scratch->next_t);
    free(scratch->next_x_5);
    free(scratch->next_x_4);
    free(scratch->temp);
    free(scratch);
}

static void dopri54_step (arpra_ode_stepper *stepper, const arpra_range *h)
{
    arpra_uint i;
    arpra_precision prec;
    arpra_ode_system *system;
    dopri54_scratch *scratch;

    system = stepper->system;
    scratch = (dopri54_scratch *) stepper->scratch;

    // Synchronise scratch memory precision.
    for (i = 0; i < system->dims; i++) {
        prec = arpra_get_precision(&(system->x[i]));
        arpra_set_precision(&(scratch->k_1[i]), prec);
        arpra_set_precision(&(scratch->k_2[i]), prec);
        arpra_set_precision(&(scratch->k_3[i]), prec);
        arpra_set_precision(&(scratch->k_4[i]), prec);
        arpra_set_precision(&(scratch->k_5[i]), prec);
        arpra_set_precision(&(scratch->k_6[i]), prec);
        arpra_set_precision(&(scratch->k_7[i]), prec);
        arpra_set_precision(&(scratch->next_x_5[i]), prec);
        arpra_set_precision(&(scratch->next_x_4[i]), prec);
    }
    prec = arpra_get_precision(system->t);
    for (i = 0; i < dopri54_stages; i++) {
        arpra_set_precision(&(scratch->k_weights_5[i]), prec);
        arpra_set_precision(&(scratch->k_weights_4[i]), prec);
    }
    arpra_set_precision(scratch->next_t, prec);
    arpra_set_precision(scratch->temp, prec);

    // k_1 = f([t], [x(t)])
    system->f(scratch->k_1,
              system->t, system->x,
              system->dims, system->params);

    // k_2 = f([t
    arpra_set_d(scratch->temp, 2.0);
    arpra_div(&(scratch->k_weights_5[0]), h, scratch->temp);
    for (i = 0; i < system->dims; i++) {
        arpra_mul(&(scratch->next_x_5[i]), &(scratch->k_weights_5[0]), &(scratch->k_1[i]));
        arpra_add(&(scratch->next_x_5[i]), &(system->x[i]), &(scratch->next_x_5[i]));
    }
    arpra_add(scratch->next_t, system->t, &(scratch->k_weights_5[0]));
    system->f(scratch->k_2,
              scratch->next_t, scratch->next_x_5,
              system->dims, system->params);

    // k_3 = f([t
    arpra_set_d(scratch->temp, 4.0);
    arpra_div(&(scratch->k_weights_5[0]), h, scratch->temp);
    arpra_set_d(scratch->temp, 3.0);
    arpra_mul(&(scratch->k_weights_5[0]), scratch->temp, &(scratch->k_weights_5[0]));
    for (i = 0; i < system->dims; i++) {
        arpra_mul(&(scratch->next_x_5[i]), &(scratch->k_weights_5[0]), &(scratch->k_2[i]));
        arpra_add(&(scratch->next_x_5[i]), &(system->x[i]), &(scratch->next_x_5[i]));
    }
    arpra_add(scratch->next_t, system->t, &(scratch->k_weights_5[0]));
    system->f(scratch->k_3,
              scratch->next_t, scratch->next_x_5,
              system->dims, system->params);

    // k_4 = f([t
    arpra_set_d(scratch->temp, 9.0);
    arpra_div(&(scratch->k_weights_5[3]), h, scratch->temp);
    arpra_set_d(scratch->temp, 2.0);
    arpra_mul(&(scratch->k_weights_5[0]), scratch->temp, &(scratch->k_weights_5[3]));
    arpra_set_d(scratch->temp, 3.0);
    arpra_mul(&(scratch->k_weights_5[1]), scratch->temp, &(scratch->k_weights_5[3]));
    arpra_set_d(scratch->temp, 4.0);
    arpra_mul(&(scratch->k_weights_5[2]), scratch->temp, &(scratch->k_weights_5[3]));
    arpra_set_zero(&(scratch->k_weights_5[3]));
    for (i = 0; i < system->dims; i++) {
        prec = arpra_get_precision(&(system->x[i]));
        arpra_set_precision(scratch->temp, prec);
        arpra_mul(scratch->temp, &(scratch->k_weights_5[0]), &(scratch->k_1[i]));
        arpra_add(&(scratch->next_x_5[i]), &(system->x[i]), scratch->temp);
        arpra_mul(scratch->temp, &(scratch->k_weights_5[1]), &(scratch->k_2[i]));
        arpra_add(&(scratch->next_x_5[i]), &(scratch->next_x_5[i]), scratch->temp);
        arpra_mul(scratch->temp, &(scratch->k_weights_5[2]), &(scratch->k_3[i]));
        arpra_add(&(scratch->next_x_5[i]), &(scratch->next_x_5[i]), scratch->temp);
    }
    arpra_add(scratch->next_t, system->t, h);
    system->f(scratch->k_4,
              scratch->next_t, scratch->next_x_5,
              system->dims, system->params);

    // x_5(t + h) = x(t) +
    // This has already been computed above.

    // x_4(t + h) = x(t) +
    arpra_set_d(scratch->temp, 24.0);
    arpra_div(&(scratch->k_weights_4[3]), h, scratch->temp);
    arpra_set_d(scratch->temp, 7.0);
    arpra_mul(&(scratch->k_weights_4[0]), scratch->temp, &(scratch->k_weights_4[3]));
    arpra_set_d(scratch->temp, 6.0);
    arpra_mul(&(scratch->k_weights_4[1]), scratch->temp, &(scratch->k_weights_4[3]));
    arpra_set_d(scratch->temp, 8.0);
    arpra_mul(&(scratch->k_weights_4[2]), scratch->temp, &(scratch->k_weights_4[3]));
    arpra_set_d(scratch->temp, 3.0);
    arpra_mul(&(scratch->k_weights_4[3]), scratch->temp, &(scratch->k_weights_4[3]));
    for (i = 0; i < system->dims; i++) {
        prec = arpra_get_precision(&(system->x[i]));
        arpra_set_precision(scratch->temp, prec);
        arpra_mul(scratch->temp, &(scratch->k_weights_4[0]), &(scratch->k_1[i]));
        arpra_add(&(scratch->next_x_4[i]), &(system->x[i]), scratch->temp);
        arpra_mul(scratch->temp, &(scratch->k_weights_4[1]), &(scratch->k_2[i]));
        arpra_add(&(scratch->next_x_4[i]), &(scratch->next_x_4[i]), scratch->temp);
        arpra_mul(scratch->temp, &(scratch->k_weights_4[2]), &(scratch->k_3[i]));
        arpra_add(&(scratch->next_x_4[i]), &(scratch->next_x_4[i]), scratch->temp);
        arpra_mul(scratch->temp, &(scratch->k_weights_4[3]), &(scratch->k_4[i]));
        arpra_add(&(scratch->next_x_4[i]), &(scratch->next_x_4[i]), scratch->temp);
    }

    // Advance system.
    arpra_add(system->t, system->t, h);
    arpra_range *temp_x = system->x;
    system->x = scratch->next_x_5;
    scratch->next_x_5 = temp_x;
}

static const arpra_ode_method dopri54 =
{
    .init = &dopri54_init,
    .clear = &dopri54_clear,
    .step = &dopri54_step,
    .stages = dopri54_stages,
};

const arpra_ode_method *arpra_ode_dopri54 = &dopri54;
