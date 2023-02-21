#include <gmp.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "needle_rho.h"
#include "random_functions.h"

static uint8_t nb_bits;
static uint8_t prob;
static mpz_t inner_flavor;
static mpz_t outer_flavor;
static gmp_randstate_t r_state;

/** Determines whether a point is our needle.
 *
 *  @param[in]	collision	A possible needle.
 *  @param[in]	inverse_prob		1 / probability of needle.
 *  @return 	1 if one of the points is the needle, 0 otherwise.
 **/
int check_needle(mpz_t collision, int inverse_prob) {
    int retval = 0, counter = 0;

    mpz_t x, zero;
    mpz_inits(x, zero, NULL);

    for (int i = 0; i < inverse_prob; ++i) {
        mpz_urandomb(x, r_state, nb_bits);
        f(x, zero);
        if (mpz_cmp(collision, x) == 0) {
            counter++;
        }
    }
    if (counter >= 2) {
        printf("%lu is the needle!", mpz_get_ui(collision));
        retval =  1;
    }
    mpz_clears(x, zero, NULL);
    return retval;
}

void rho_mode_detection(uint8_t n, uint8_t prob_init) {
    nb_bits = n;
    prob = prob_init;
    int inverse_prob = pow(2, prob);
    int counter = 0;

    mpz_t start, collision;
    mpz_inits(start, collision, inner_flavor, outer_flavor, NULL);

    gmp_randinit_default(r_state);
    gmp_randseed_ui(r_state, time(NULL));

    init_f(nb_bits, prob);
    do {
        counter++;
        mpz_urandomb(start, r_state, nb_bits);
        mpz_urandomb(outer_flavor, r_state, nb_bits);

        nested_rho(start, collision);

        printf("%d: collision found!, %lu \n", counter, mpz_get_ui(collision));
        printf("________________________________ \n");
    }
    while (!check_needle(collision, inverse_prob));

    mpz_clears(start, collision, inner_flavor, outer_flavor, NULL);
    gmp_randclear(r_state);
}

void nested_rho(mpz_t start_point, mpz_t collision) {
    mpz_t tortoise, hare;
    mpz_inits(tortoise, hare, NULL);

    get_offset(start_point, outer_flavor, tortoise);
    mpz_set(inner_flavor, tortoise);
    rho(tortoise, tortoise);

    get_offset(tortoise, outer_flavor, hare);
    mpz_set(inner_flavor, hare);
    rho(hare, hare);

    while (mpz_cmp(tortoise, hare) != 0) {
        get_offset(tortoise, outer_flavor, tortoise);
        mpz_set(inner_flavor, tortoise);
        rho(tortoise, tortoise);
        //printf("tortoise!, %lu \n", mpz_get_ui(tortoise));

        get_offset(hare, outer_flavor, hare);
        mpz_set(inner_flavor, hare);
        rho(hare, hare);
        //printf("hare!, %lu \n", mpz_get_ui(hare));
        get_offset(hare, outer_flavor, hare);
        mpz_set(inner_flavor, hare);
        rho(hare, hare);
        //printf("hare!, %lu \n", mpz_get_ui(hare));
    }

    mpz_set(tortoise, start_point);
    while (mpz_cmp(tortoise, hare) != 0) {
        get_offset(tortoise, outer_flavor, tortoise);
        mpz_set(inner_flavor, tortoise);
        rho(tortoise, tortoise);

        get_offset(hare, outer_flavor, hare);
        mpz_set(inner_flavor, hare);
        rho(hare, hare);
    }
    mpz_set(collision, tortoise);
    mpz_clears(tortoise, hare, NULL);
}

void rho(mpz_t start_point, mpz_t collision) {
    mpz_t t, h;
    mpz_inits(t, h, NULL);

    mpz_set(t, start_point);
    f(t, inner_flavor);

    mpz_set(h, start_point);
    f(h, inner_flavor);
    f(h, inner_flavor);

    while (mpz_cmp(t, h) != 0) {
        f(t, inner_flavor);

        f(h, inner_flavor);
        f(h, inner_flavor);
    }

    mpz_set(t, start_point);
    while (mpz_cmp(t, h) != 0) {
        f(t, inner_flavor);
        f(h, inner_flavor);
    }
    mpz_set(collision, t);
    //printf("collision!, %lu \n", mpz_get_ui(collision));

    mpz_clears(t, h, NULL);
}
