#include <gmp.h>
#include <stdio.h>
#include <time.h>
#include "needle_pcs.h"
#include "needle_rho.h"
#include "random_functions.h"

static uint8_t nb_bits;
static mpz_t flavor;
static mpz_t offset;
static mpz_t threshold;
static gmp_randstate_t r_state;

/** Determines whether a point is our needle.
 *
 *  @param[in]	collision	A possible needle.
 *  @param[in]	inverse_prob		1 / probability of needle.
 *  @return 	1 if one of the points is the needle, 0 otherwise.
 **/
int check_needle(mpz_t collision, mpz_t inverse_prob) {
    int retval = 0, counter = 0;

    mpz_t x;
    mpz_init(x);

    for (int i = 0; i < mpz_get_ui(inverse_prob); ++i) {
        mpz_urandomb(x, r_state, nb_bits);
        f(x);
        if (mpz_cmp(collision, x) == 0) {
            counter++;
        }
    }
    if (counter >= 2) {
        printf("%lu is the needle!", mpz_get_ui(collision));
        retval =  1;
    }
    mpz_clear(x);
    return retval;
}

void rho_mode_detection(uint8_t n, mpz_t prob) {
    nb_bits = n;

    mpz_t start, collision, inverse_prob;
    mpz_inits(start, collision, inverse_prob, flavor, offset, threshold, NULL);
    mpz_ui_pow_ui(inverse_prob, 2, mpz_get_ui(prob));

    // Threshold is used to create our needle - can be removed if different function (f) is used.
    mpz_ui_sub(threshold, nb_bits, prob);
    mpz_ui_pow_ui(threshold, 2, mpz_get_ui(threshold));

    gmp_randinit_default(r_state);
    gmp_randseed_ui(r_state, time(NULL));

    do {
        mpz_urandomb(start, r_state, nb_bits);
        // create random offset
        mpz_urandomb(offset, r_state, nb_bits);

        nested_rho(start, collision);

        printf("collision found!, %lu \n", mpz_get_ui(collision));
        printf("________________________________ \n");
    }
    while (!check_needle(collision, inverse_prob));

    mpz_clears(start, collision, inverse_prob, flavor, offset, threshold, NULL);
    gmp_randclear(r_state);
}

void nested_rho(mpz_t start_point, mpz_t collision) {
    mpz_t tortoise, hare;
    mpz_inits(tortoise, hare, NULL);

    get_next(start_point, offset, flavor);
    rho(start_point, tortoise);

    get_next(tortoise, offset, flavor);
    rho(tortoise, hare);

    while (mpz_cmp(tortoise, hare) != 0) {
        get_next(tortoise, offset, flavor);
        rho(tortoise, tortoise);

        get_next(hare, offset, flavor);
        rho(hare, hare);
        get_next(hare, offset, flavor);
        rho(hare, hare);
    }

    mpz_set(tortoise, start_point);
    while (mpz_cmp(tortoise, hare) != 0) {
        get_next(tortoise, offset, flavor);
        rho(tortoise, tortoise);

        get_next(hare, offset, flavor);
        rho(hare, hare);
    }
    mpz_set(collision, tortoise);
    mpz_clears(tortoise, hare, NULL);
}

void rho(mpz_t start_point, mpz_t collision) {
    init_f(nb_bits, flavor, threshold);

    mpz_t tortoise, hare;
    mpz_inits(tortoise, hare, NULL);

    mpz_set(tortoise, start_point);
    f(tortoise);

    mpz_set(hare, start_point);
    f(hare);
    f(hare);

    while (mpz_cmp(tortoise, hare) != 0) {
        f(tortoise);

        f(hare);
        f(hare);
    }

    mpz_set(tortoise, start_point);
    while (mpz_cmp(tortoise, hare) != 0) {
        f(tortoise);
        f(hare);
    }
    mpz_set(collision, tortoise);
    //printf("collision!, %lu \n", mpz_get_ui(collision));

    mpz_clears(tortoise, hare, NULL);
}
