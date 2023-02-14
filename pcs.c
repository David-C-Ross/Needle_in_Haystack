#include <gmp.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "pcs.h"
#include "pcs_storage.h"
#include "random_functions.h"

static uint8_t nb_bits;
static uint8_t trailling_bits;
static gmp_randstate_t r_state;
static int r_state_counter;

/** Determines whether a point is a distinguished one.
 *
 *  @param[in]	point				A point.
 *  @param[in]	trailling_bits  Number of trailling zero bits.
 *  @return 	1 if the point is distinguished, 0 otherwise.
 */
int is_distinguished(mpz_t point) {
    int res;
    mpz_t r;
    mpz_init(r);
    mpz_fdiv_r_2exp(r, point, trailling_bits);
    res = (mpz_sgn(r) == 0);
    mpz_clears(r, NULL);
    return (res);
}

int get_distinguished(mpz_t point, mpz_t dist) {
    int trail_length = 0;
    int trail_length_max = pow(2, trailling_bits) * 20;
    mpz_t x;

    mpz_init_set(x, point);

    while (!is_distinguished(x)) {
        f(x);
        trail_length++;

        if (trail_length > trail_length_max) {
            mpz_urandomb(point, r_state, nb_bits);
            r_state_counter++;

            mpz_set(x, point);
            trail_length = 0;
        }
    }
    mpz_set(dist, x);
    mpz_clear(x);
    return trail_length;
}

/** Checks if there is a collision and returns the image.
 *
 */
int is_collision(mpz_t collision, mpz_t a1, mpz_t a2, int length1, int length2) {
    mpz_t xDist_;
    mpz_t b1, b2;
    mpz_t pre1, pre2;

    mpz_inits(b1, b2, xDist_, pre1, pre2, NULL);

    int retval = 0;

    //recompute first point
    mpz_set(b1, a1);

    //recompute second point
    mpz_set(b2, a2);

    while (length1 > length2) {
        f(b1);
        length1--;
    }
    while (length2 > length1) {
        f(b2);
        length2--;
    }
    // checks for robin hood
    if (mpz_cmp(b1, b2) != 0) {
        while (mpz_cmp(b1, b2) != 0) {
            mpz_set(pre1, b1);
            mpz_set(pre2, b2);

            f(b1);
            f(b2);

        }
        mpz_set(collision, b1);
        retval = 1;

        //printf("f(%lu) = f(%lu) = %lu \n", mpz_get_ui(pre1), mpz_get_ui(pre2), mpz_get_ui(collision));
    }
    mpz_clears(b1, b2, xDist_, pre1, pre2, NULL);
    return retval;
}

/** Initialize all variables needed to do a PCS algorithm.
 *
 */
void pcs_init(uint8_t n_init, uint8_t memory_init, uint8_t trailling_bits_init, mpz_t flavor_init, mpz_t prob) {
    nb_bits = n_init;
    trailling_bits = trailling_bits_init;

    // Threshold is used to create our needle - can be removed if different function (f) is used.
    mpz_t threshold;
    mpz_init(threshold);
    mpz_ui_sub(threshold, nb_bits, prob);
    mpz_ui_pow_ui(threshold, 2, mpz_get_ui(threshold));

    init_f(nb_bits, flavor_init, threshold);
    mpz_clear(threshold);

    gmp_randinit_default(r_state);
}

/** Run the PCS algorithm.
 *
 */
int pcs_run(Table_t *table, mpz_t start_point, int nb_collisions, mpz_t *collisions) {
    mpz_t a, a2;
    mpz_t dist, collision;
    char xDist_str[50];

    r_state_counter = 0;
    int trail_length1, trail_length2;
    int collision_count = 0;

    mpz_inits(a, a2, dist, collision, NULL);
    //Initialize a starting point
    mpz_set(a, start_point);

    while (collision_count < nb_collisions) {
        trail_length1 = get_distinguished(a, dist);

        //mpz_fdiv_q_2exp(dist, dist, trailling_bits);
        trail_length2 = struct_add(table, a2, a, dist, trail_length1, xDist_str);
        if (trail_length2) {
            if (is_collision(collision, a, a2, trail_length1, trail_length2)) {
                mpz_set(collisions[collision_count], collision);
                collision_count++;

                //printf("collision found!, %lu \n", mpz_get_ui(collision));
                //printf("start point 1, %lu \n", mpz_get_ui(a));
                //printf("start point 2, %lu \n", mpz_get_ui(a2));
            }
        }
        mpz_urandomb(a, r_state, nb_bits);
        r_state_counter++;
    }
    mpz_clears(a, a2, collision, dist, NULL);
    return r_state_counter;
}

void init_seed(mpz_t seed_init) {
    unsigned long int seed;
    srand((mpz_get_ui(seed_init)));
    seed = rand();
    gmp_randseed_ui(r_state, seed);
}

void get_current_rstate(mpz_t seed, int counter) {
    mpz_t temp;
    mpz_init(temp);
    init_seed(seed);

    for (int i = 0; i < counter; ++i) {
        mpz_urandomb(temp, r_state, nb_bits);
    }
    mpz_clear(temp);
}

/** Free all variables used in the previous PCS run.
 *
 */
void pcs_clear() {
    gmp_randclear(r_state);
}

void clear_table(Table_t *table) {
    struct_free(table);
}
