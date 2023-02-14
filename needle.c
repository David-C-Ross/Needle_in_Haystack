#include <gmp.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pcs.h"
#include "needle.h"
#include "random_functions.h"
#include "pcs_storage.h"

static uint8_t nb_bits;
static uint8_t memory;
static uint8_t trailling_bits;
static mpz_t flavor;
static mpz_t offset;
static mpz_t threshold;
static gmp_randstate_t r_state;

uint8_t trailling_bits_init() {
    uint8_t bits = nb_bits + memory;
    return nb_bits - (uint8_t) floor(bits / 2);
}

void get_next(mpz_t point, mpz_t next) {
    /*
    mpz_t temp;
    mpz_init(temp);

    mpz_xor(temp, point, offset);
    srand(mpz_get_ui(temp));
    mpz_set_ui(temp, rand());
    mpz_mod_2exp(next, temp, nb_bits);

    mpz_clear(temp);
     */
    mpz_xor(next, point, offset);
}

void find_collision(mpz_t distCollision, mpz_t a1, mpz_t a2, int length1, int length2, mpz_t prob) {
    mpz_t b1, b2, d1, d2;
    mpz_inits(b1, b2, d1, d2, NULL);

    int counter1 = 0, counter2 = 0;

    mpz_set(b1, a1);
    mpz_set(b2, a2);

    hashUNIX_t **inner_table1 = pcs_init(nb_bits, memory, trailling_bits, flavor, prob);
    hashUNIX_t **inner_table2 = pcs_init(nb_bits, memory, trailling_bits, flavor, prob);

    if (length1 > length2) {
        init_seed(a1);
        while (length1 > length2) {
            counter1 += pcs_run(inner_table1, b1, 1, &d1);
            get_next(d1, b1);
            length1--;
        }
    }
    else {
        init_seed(a2);
        while (length2 > length1) {
            counter2 += pcs_run(inner_table2, b2, 1, &d2);
            get_next(d2, b2);
            length2--;
        }
    }
    while (mpz_cmp(b1, b2) != 0) {
        get_current_rstate(a1, counter1);
        counter1 += pcs_run(inner_table1, b1, 1, &d1);

        get_current_rstate(a2, counter2);
        counter2 += pcs_run(inner_table2, b2, 1, &d2);

        get_next(d1, b1);
        get_next(d2, b2);
    }
    mpz_set(distCollision, d1);

    clear_table(inner_table1);
    clear_table(inner_table2);
    mpz_clears(b1, b2, d1, d2, NULL);
}


/** Determines whether a point is our needle.
 *
 *  @param[in]	collision	A possible needle.
 *  @param[in]	inverse_prob		1 / probability of needle.
 *  @return 	1 if one of the points is the needle, 0 otherwise.
 **/
int check_needle(mpz_t collision, mpz_t inverse_prob) {
    int retval = 0, counter = 0, threshold = 3;

    mpz_t x;
    mpz_init(x);

    for (int i = 0; i < mpz_get_ui(inverse_prob); ++i) {
        mpz_urandomb(x, r_state, nb_bits);
        f(x);
        if (mpz_cmp(collision, x) == 0) {
            counter++;
        }
    }
    if (counter >= threshold) {
        printf("%lu is the needle!", mpz_get_ui(collision));
        retval =  1;
    }
    mpz_clear(x);
    return retval;
}

void pcs_mode_detection(uint8_t n, uint8_t memory_init, mpz_t prob) {
    nb_bits = n;
    memory = memory_init;
    int M = pow(2, memory);
    trailling_bits = trailling_bits_init();
    char xDist_str[50];

    int trail_length1, trail_length2;
    int trail_length_max = pow(2, trailling_bits) * 20;

    hashUNIX_t ** inner_table;
    hashUNIX_t ** outer_table;

    mpz_t start1, start2, collision, distCollision, inverse_prob;
    mpz_inits(start1, start2, collision, distCollision, inverse_prob, flavor, offset, NULL);
    mpz_ui_pow_ui(inverse_prob, 2, mpz_get_ui(prob));

    gmp_randinit_default(r_state);
    gmp_randseed_ui(r_state, time(NULL));

    do {
        outer_table = struct_init_hash(nb_bits, memory);
        // create random flavor
        mpz_urandomb(flavor, r_state, nb_bits);
        // create random offset
        mpz_urandomb(offset, r_state, nb_bits);
        // find M collisions which are also distinguished points
        for (int i = 0; i < M; ++i) {
            mpz_urandomb(start1, r_state, nb_bits);

            inner_table = pcs_init(nb_bits, memory, trailling_bits, flavor, prob);
            init_seed(start1);

            trail_length1 = 0;
            do {
                pcs_run(inner_table, start1, 1, &collision);
                get_next(collision, start1);
                trail_length1++;

                if (trail_length1 > trail_length_max) {
                    mpz_urandomb(start1, r_state, nb_bits);
                    init_seed(start1);
                    trail_length1 = 0;

                    clear_table(inner_table);
                    inner_table = pcs_init(nb_bits, memory, trailling_bits, flavor, prob);
                }

            } while (!is_distinguished(collision));

            //printf("%d collision found!, %lu \n", i + 1, mpz_get_ui(collision));

            trail_length2 = struct_add(outer_table, start2, start1, collision, trail_length1, xDist_str);
            if (trail_length2) {
                find_collision(distCollision, start1, start2, trail_length1, trail_length2, prob);
                printf("repeated collision!, %lu \n", mpz_get_ui(distCollision));
                if (check_needle(distCollision, inverse_prob)) {
                    goto end;
                }
            }
            clear_table(inner_table);
        }
        clear_table(outer_table);
        printf("________________________________ \n");
    }
    while (1);

    end:
    mpz_clears(start1, start2, collision, distCollision, inverse_prob, flavor, offset, NULL);
    gmp_randclear(r_state);

    clear_table(inner_table);
    clear_table(outer_table);
    pcs_clear();
}

void rho_mode_detection(uint8_t n, mpz_t prob) {
    nb_bits = n;

    mpz_t start, tortoise, hare, inverse_prob;
    mpz_inits(start, tortoise, hare, inverse_prob, flavor, offset, threshold, NULL);
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
        get_next(start, flavor);

        rho(start, tortoise);
        get_next(tortoise, flavor);

        rho(tortoise, hare);

        while (mpz_cmp(tortoise, hare) != 0) {
            get_next(tortoise, flavor);
            rho(tortoise, tortoise);

            get_next(hare, flavor);
            rho(hare, hare);
            get_next(hare, flavor);
            rho(hare, hare);
        }

        mpz_set(tortoise, start);
        while (mpz_cmp(tortoise, hare) != 0) {
            get_next(tortoise, flavor);
            rho(tortoise, tortoise);

            get_next(hare, flavor);
            rho(hare, hare);
        }

        printf("collision found!, %lu \n", mpz_get_ui(tortoise));
        printf("________________________________ \n");
    }
    while (!check_needle(tortoise, inverse_prob));

    mpz_clears(start, tortoise, hare, inverse_prob, flavor, offset, threshold, NULL);
    gmp_randclear(r_state);
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
