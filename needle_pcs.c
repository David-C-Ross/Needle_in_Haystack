#include <gmp.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "pcs.h"
#include "needle_pcs.h"
#include "random_functions.h"
#include "pcs_storage.h"

static uint8_t nb_bits;
static uint8_t memory;
static uint8_t trailing_bits;
static mpz_t flavor;
static mpz_t offset;
static gmp_randstate_t r_state;

uint8_t trailing_bits_init() {
    uint8_t bits = nb_bits - memory;
    return bits / 2;
}

void find_collision(mpz_t distCollision, mpz_t a1, mpz_t a2, int length1, int length2, mpz_t prob) {
    mpz_t b1, b2, d1, d2;
    mpz_inits(b1, b2, d1, d2, NULL);

    int counter1 = 0, counter2 = 0;

    mpz_set(b1, a1);
    mpz_set(b2, a2);

    Table_t *inner_table1 = struct_init(nb_bits, memory);
    Table_t *inner_table2 = struct_init(nb_bits, memory);

    if (length1 > length2) {
        init_seed(a1);
        while (length1 > length2) {
            counter1 += pcs_run(inner_table1, b1, 1, &d1);
            get_next(d1, offset, b1);
            length1--;
        }
    }
    else {
        init_seed(a2);
        while (length2 > length1) {
            counter2 += pcs_run(inner_table2, b2, 1, &d2);
            get_next(d2, offset, b2);
            length2--;
        }
    }
    while (mpz_cmp(b1, b2) != 0) {
        get_current_rstate(a1, counter1);
        counter1 += pcs_run(inner_table1, b1, 1, &d1);

        get_current_rstate(a2, counter2);
        counter2 += pcs_run(inner_table2, b2, 1, &d2);

        get_next(d1, offset, b1);
        get_next(d2, offset, b2);
    }
    mpz_set(distCollision, d1);

    clear_table(inner_table1);
    clear_table(inner_table2);
    mpz_clears(b1, b2, d1, d2, NULL);
}


/** Determines whether a point is our needle.
 *
 *  @param[in]	collisions	    An array of possible needles.
 *  @param[in]	inverse_prob	1 / probability of needle.
 *  @return 	1 if one of the points is the needle, 0 otherwise.
 **/
int check_needle_mem(mpz_t *collisions, mpz_t inverse_prob, int nb_collisions) {
    int retval = 0;
    int *counters = calloc(nb_collisions, sizeof(int));

    mpz_t x;
    mpz_init(x);

    for (int i = 0; i < mpz_get_ui(inverse_prob); ++i) {
        mpz_urandomb(x, r_state, nb_bits);
        f(x);
        for (int j = 0; j < nb_collisions; ++j) {
            if (mpz_cmp(collisions[j], x) == 0) {
                counters[j]++;
                if (counters[j] >= 2) {
                    printf("%lu is the needle!", mpz_get_ui(collisions[j]));
                    retval = 1;
                    goto end;
                }
            }
        }
    }
    end:
    free(counters);
    mpz_clear(x);

    return retval;
}

void pcs_mode_detection(uint8_t n, uint8_t memory_init, mpz_t prob) {
    nb_bits = n;
    memory = memory_init;
    int M = pow(2, memory);
    trailing_bits = trailing_bits_init();
    char xDist_str[50];

    int counter = 0;
    int nb_collisions;
    int trail_length1, trail_length2;
    int trail_length_max = pow(2, trailing_bits) * 2;

    Table_t *inner_table;
    Table_t *outer_table;

    mpz_t *collisions = malloc( sizeof(mpz_t) * M);
    for (int i = 0; i < M; ++i) {
        mpz_init(collisions[i]);
    }

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
        nb_collisions = 0;
        pcs_init(nb_bits, memory, trailing_bits, flavor, prob);
        for (int i = 0; i < M; ++i) {
            mpz_urandomb(start1, r_state, nb_bits);

            inner_table = struct_init(nb_bits, memory);
            init_seed(start1);

            trail_length1 = 0;
            do {
                pcs_run(inner_table, start1, 1, &collision);
                get_next(collision, offset, start1);
                trail_length1++;

                //printf("collision found!, %lu \n", mpz_get_ui(collision));

                if (trail_length1 > trail_length_max) {
                    mpz_urandomb(start1, r_state, nb_bits);
                    init_seed(start1);
                    trail_length1 = 0;

                    clear_table(inner_table);
                    inner_table = struct_init(nb_bits, memory);
                }

            } while (!is_distinguished(collision));

            //printf("%d: collision found!, %lu \n", i + 1, mpz_get_ui(collision));
            clear_table(inner_table);

            trail_length2 = struct_add(outer_table, start2, start1, collision, trail_length1, xDist_str);
            if (trail_length2) {
                counter++;
                find_collision(distCollision, start1, start2, trail_length1, trail_length2, prob);
                printf("%d: repeated collision!, %lu \n", counter, mpz_get_ui(distCollision));
                mpz_set(collisions[nb_collisions], distCollision);
                nb_collisions++;
            }
        }
        if (check_needle_mem(collisions, inverse_prob, nb_collisions)) {
            goto end;
        }
        clear_table(outer_table);
        printf("________________________________ \n");
    }
    while (1);

    end:
    for (int i = 0; i < M; ++i) {
        mpz_clear(collisions[i]);
    }
    free(collisions);

    mpz_clears(start1, start2, collision, distCollision, inverse_prob, flavor, offset, NULL);
    gmp_randclear(r_state);

    clear_table(outer_table);
    pcs_clear();
}
