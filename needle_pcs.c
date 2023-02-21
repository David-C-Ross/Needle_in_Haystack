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

void find_collision(mpz_t distCollision, mpz_t a1, mpz_t a2, int length1, int length2) {
    mpz_t b1, b2, d1, d2;
    mpz_inits(b1, b2, d1, d2, NULL);

    mpz_set(b1, a1);
    mpz_set(b2, a2);

    Table_t *inner_table1 = struct_init(nb_bits, memory);
    Table_t *inner_table2 = struct_init(nb_bits, memory);

    while (length1 > length2) {
        init_seed(b1);
        pcs_run(inner_table1, b1, 1, &d1);
        get_offset(d1, offset, b1);
        length1--;
    }
    while (length2 > length1) {
        init_seed(b2);
        pcs_run(inner_table2, b2, 1, &d2);
        get_offset(d2, offset, b2);
        length2--;
    }
    while (mpz_cmp(b1, b2) != 0) {
        init_seed(b1);
        pcs_run(inner_table1, b1, 1, &d1);

        init_seed(b2);
        pcs_run(inner_table2, b2, 1, &d2);

        get_offset(d1, offset, b1);
        get_offset(d2, offset, b2);
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
int check_needle_mem(mpz_t *collisions, int inverse_prob, int nb_collisions) {
    int retval = 0;
    int *counters = calloc(nb_collisions, sizeof(int));

    mpz_t x, zero;
    mpz_inits(x, zero, NULL);

    for (int i = 0; i < inverse_prob; ++i) {
        mpz_urandomb(x, r_state, nb_bits);
        f(x, zero);
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
    mpz_clears(x, zero, NULL);

    return retval;
}

void pcs_mode_detection(uint8_t n, uint8_t memory_init, uint8_t prob_init) {
    nb_bits = n;
    memory = memory_init;
    trailing_bits = trailing_bits_init();

    int nb_collisions = pow(2, memory);
    int inverse_prob = pow(2, prob_init);
    char xDist_str[50];

    int counter = 0, collision_count = 0;
    int trail_length1, trail_length2;
    int trail_length_max = pow(2, trailing_bits) * 3;

    Table_t *inner_table;
    Table_t *outer_table;

    mpz_t *collisions = malloc(sizeof(mpz_t) * nb_collisions);
    for (int i = 0; i < nb_collisions; ++i) {
        mpz_init(collisions[i]);
    }

    mpz_t start1, start2, collision, distCollision;
    mpz_inits(start1, start2, collision, distCollision, flavor, offset, NULL);

    gmp_randinit_default(r_state);
    gmp_randseed_ui(r_state, time(NULL));

    do {
        outer_table = struct_init_hash(nb_bits, memory);
        // create random inner_flavor
        mpz_urandomb(flavor, r_state, nb_bits);
        // create random outer_flavor
        mpz_urandomb(offset, r_state, nb_bits);

        // find nb_collisions collisions which are also distinguished points
        pcs_init(nb_bits, prob_init, trailing_bits, flavor);
        while (collision_count < nb_collisions) {
            mpz_urandomb(start1, r_state, nb_bits);

            inner_table = struct_init(nb_bits, memory);

            trail_length1 = 0;
            do {
                init_seed(start1);
                pcs_run(inner_table, start1, 1, &collision);
                get_offset(collision, offset, start1);
                trail_length1++;

                printf("collision found!, %lu \n", mpz_get_ui(collision));

                if (trail_length1 > trail_length_max) {
                    mpz_urandomb(start1, r_state, nb_bits);
                    trail_length1 = 0;

                    clear_table(inner_table);
                    inner_table = struct_init(nb_bits, memory);
                }

            } while (!is_distinguished(collision));

            //printf("collision found!, %lu \n", mpz_get_ui(collision));
            clear_table(inner_table);

            trail_length2 = struct_add(outer_table, start2, start1, collision, trail_length1, xDist_str);
            if (trail_length2) {
                counter++;
                find_collision(distCollision, start1, start2, trail_length1, trail_length2);
                printf("%d: repeated collision!, %lu \n", counter, mpz_get_ui(distCollision));
                mpz_set(collisions[collision_count], distCollision);
                collision_count++;
            }
        }
        if (check_needle_mem(collisions, inverse_prob, nb_collisions)) {
            goto end;
        }
        collision_count = 0;
        clear_table(outer_table);
        printf("________________________________ \n");
    }
    while (1);

    end:
    for (int i = 0; i < nb_collisions; ++i) {
        mpz_clear(collisions[i]);
    }
    free(collisions);

    mpz_clears(start1, start2, collision, distCollision, flavor, offset, NULL);
    gmp_randclear(r_state);

    clear_table(outer_table);
    pcs_clear();
}
