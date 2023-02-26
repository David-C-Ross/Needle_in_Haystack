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
static mpz_t flavor;

/** Determines whether a point is a distinguished one.
 *
 *  @param[in]	point				A point.
 *  @param[in]	trailling_bits  Number of trailing zero bits.
 *  @return 	1 if the point is distinguished, 0 otherwise.
 */
int isDistinguished(mpz_t point) {

    int res;
    mpz_t r;
    mpz_init(r);
    mpz_fdiv_r_2exp(r, point, trailling_bits);
    res = (mpz_sgn(r) == 0);
    mpz_clears(r, NULL);
    return (res);
}

int getDistinguished(mpz_t point, mpz_t dist, gmp_randstate_t r_state) {

    int trail_length = 0;
    int trail_length_max = (int) pow(2, trailling_bits) * 15;
    mpz_t x;

    mpz_init_set(x, point);

    while (!isDistinguished(x)) {
        f(x, flavor);
        trail_length++;

        if (trail_length > trail_length_max) {
            mpz_urandomb(point, r_state, nb_bits);

            mpz_set(x, point);
            trail_length = 0;
        }
    }
    //printf("distinguished point %lu \n", mpz_get_ui(x));
    mpz_set(dist, x);
    mpz_clear(x);
    return trail_length;
}

/** Checks if there is a findCollision and returns the image.
 *
 */
int findCollision(mpz_t collision, mpz_t a1, mpz_t a2, int length1, int length2) {

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
        f(b1, flavor);
        length1--;
    }
    while (length2 > length1) {
        f(b2, flavor);
        length2--;
    }
    // checks for robin hood
    if (mpz_cmp(b1, b2) != 0) {
        while (mpz_cmp(b1, b2) != 0) {
            mpz_set(pre1, b1);
            mpz_set(pre2, b2);

            f(b1, flavor);
            f(b2, flavor);

        }
        mpz_set(collision, b1);
        retval = 1;

        //printf("f(%lu) = f(%lu) = %lu \n", mpz_get_ui(pre1), mpz_get_ui(pre2), mpz_get_ui(findCollision));
    }
    mpz_clears(b1, b2, xDist_, pre1, pre2, NULL);
    return retval;
}

/** Initialize all variables needed to do a PCS algorithm.
 *
 */
void pcsInit(uint8_t n_init, uint8_t trailling_bits_init) {

    nb_bits = n_init;
    trailling_bits = trailling_bits_init;
}

/** Run the PCS algorithm.
 *
 */
void pcsRun(Table_t *table, mpz_t flavor_init, int nb_collisions, gmp_randstate_t r_state, mpz_t *collisions) {

    mpz_t a, a2;
    mpz_t dist, collision;
    char xDist_str[50];

    int trail_length1, trail_length2, i = 0;

    mpz_inits(a, a2, dist, collision, NULL);
    mpz_init_set(flavor, flavor_init);

    while(i < nb_collisions) {
        //Initialize a starting point
        mpz_urandomb(a, r_state, nb_bits);

        trail_length1 = getDistinguished(a, dist, r_state);
        //mpz_fdiv_q_2exp(dist, dist, trailing_bits);
        trail_length2 = structAdd(table, a2, a, dist, trail_length1, xDist_str);
        if (trail_length2) {
            if (findCollision(collision, a, a2, trail_length1, trail_length2)) {
                mpz_set(collisions[i], collision);
                i++;

                //printf("collision found!, %lu \n", mpz_get_ui(findCollision));
                //printf("start point 1, %lu \n", mpz_get_ui(a));
                //printf("start point 2, %lu \n", mpz_get_ui(a2));
            }
        }
    }
    mpz_clears(a, a2, collision, dist, flavor, NULL);
}