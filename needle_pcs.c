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
#include "needle_rho.h"

static uint8_t nb_bits;
static uint8_t memory;
static uint8_t trailing_bits;

static mpz_t inner_flavor;
static mpz_t outer_flavor;

static gmp_randstate_t r_state;
static gmp_randstate_t r_state_pcs1;
static gmp_randstate_t r_state_pcs2;

uint8_t trailingBitsInit() {
    uint8_t bits = nb_bits - memory;
    return bits / 2;
}

int collisionPcs(mpz_t distCollision, mpz_t a1, mpz_t a2, int length1, int length2) {

    mpz_t b1, b2;
    mpz_inits(b1, b2, NULL);

    mpz_set(b1, a1);
    mpz_set(b2, a2);

    int retval = 0;

    Table_t *inner_table1 = struct_init(memory);
    Table_t *inner_table2 = struct_init(memory);

    gmp_randseed(r_state_pcs1, a1);
    gmp_randseed(r_state_pcs2, a2);

    while (length1 > length2) {
        pcsRun(inner_table1, outer_flavor, 1, r_state_pcs1, &b1);
        length1--;
    }
    while (length2 > length1) {
        pcsRun(inner_table2, outer_flavor, 1, r_state_pcs2, &b2);
        length2--;
    }
    if (mpz_cmp(b1, b2) != 0) {
        while (mpz_cmp(b1, b2) != 0) {
            pcsRun(inner_table1, outer_flavor, 1, r_state_pcs1, &b1);

            pcsRun(inner_table2, outer_flavor, 1, r_state_pcs2, &b2);
        }
        mpz_set(distCollision, b1);
        retval = 1;
    }
    structFree(inner_table1);
    structFree(inner_table2);

    return retval;
}

int collisionRho(mpz_t distCollision, mpz_t a1, mpz_t a2, int length1, int length2) {

    mpz_t b1, b2;
    mpz_inits(b1, b2, NULL);

    mpz_set(b1, a1);
    mpz_set(b2, a2);

    int retval = 0;

    while (length1 > length2) {
        mpz_xor(b1, b1, outer_flavor);
        mpz_set(inner_flavor, b1);
        rho(b1, inner_flavor, b1);
        length1--;
    }
    while (length2 > length1) {
        mpz_xor(b2, b2, outer_flavor);
        mpz_set(inner_flavor, b2);
        rho(b2, inner_flavor, b2);
        length2--;
    }
    // checks for robin hood
    if (mpz_cmp(b1, b2) != 0) {
        while (mpz_cmp(b1, b2) != 0) {

            mpz_xor(b1, b1, outer_flavor);
            mpz_set(inner_flavor, b1);
            rho(b1, inner_flavor, b1);

            mpz_xor(b2, b2, outer_flavor);
            mpz_set(inner_flavor, b2);
            rho(b2, inner_flavor, b2);
        }
        mpz_set(distCollision, b1);
        retval = 1;
    }
    mpz_clears(b1, b2, NULL);

    return retval;
}


/** Determines whether a point is our needle.
 *
 *  @param[in]	collisions	    An array of possible needles.
 *  @param[in]	inverse_prob	1 / probability of needle.
 *  @param[in]	nb_collisions	number of possible needles to check.
 *  @return 	1 if one of the points is the needle, 0 otherwise.
 **/
int isNeedleMem(mpz_t *collisions, int inverse_prob, int nb_collisions) {
    int retval = 0;
    int *counters = calloc(nb_collisions, sizeof(int));

    mpz_t x, zero;
    mpz_inits(x, zero, NULL);

    for (int i = 0; i < 3 * inverse_prob; ++i) {
        mpz_urandomb(x, r_state, nb_bits);
        f(x, zero);
        for (int j = 0; j < nb_collisions; ++j) {
            if (mpz_cmp(collisions[j], x) == 0) {
                counters[j]++;
                if (counters[j] >= 2) {
                    if (mpz_cmp_ui(collisions[j], 1) == 0) {
                        retval = 1;
                    }
                    else {
                        retval = -1;
                    }
                    printf("%lu is the needle!", mpz_get_ui(collisions[j]));
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

int pcsPcsModeDetection(uint8_t n, uint8_t memory_init, uint8_t prob_init) {

    nb_bits = n;
    memory = memory_init;
    trailing_bits = trailingBitsInit();

    int nb_collisions = (int) pow(2, memory);
    int inverse_prob = (int) pow(2, prob_init);
    char xDist_str[50];

    int trail_length1, trail_length2;
    int trail_length_max = (int) pow(2, trailing_bits) * 15;
    int counter, flag;

    Table_t *inner_table;
    Table_t *outer_table;

    mpz_t *collisions = malloc(sizeof(mpz_t) * nb_collisions);
    for (int i = 0; i < nb_collisions; ++i) {
        mpz_init(collisions[i]);
    }

    mpz_t start1, start2, collision, distCollision;
    mpz_inits(start1, start2, collision, distCollision, outer_flavor, NULL);

    gmp_randinit_default(r_state);
    gmp_randseed_ui(r_state, time(NULL));

    gmp_randinit_default(r_state_pcs1);
    gmp_randinit_default(r_state_pcs2);

    initF(nb_bits, prob_init);
    pcsInit(nb_bits, trailing_bits);

    do {
        counter = 0;

        outer_table = structInitHash(memory);
        // create random outer_flavor
        mpz_urandomb(outer_flavor, r_state, nb_bits);
        // find O(M) collisions which are also distinguished points
        for (int i = 0; i < nb_collisions; ++i) {

            mpz_urandomb(start1, r_state, nb_bits);
            gmp_randseed(r_state_pcs1, start1);

            trail_length1 = 0;
            inner_table = struct_init(memory);

            do {
                pcsRun(inner_table, outer_flavor, 1, r_state_pcs1, &collision);
                //printf("collision found!, %lu \n", mpz_get_ui(collision));
                trail_length1++;

                if (trail_length1 > trail_length_max) {
                    mpz_urandomb(start1, r_state, nb_bits);
                    gmp_randseed(r_state_pcs1, start1);

                    trail_length1 = 0;
                    structFree(inner_table);
                    inner_table = struct_init(memory);
                }
            } while (!isDistinguished(collision));

            //printf("distinguished collision found!, %lu \n", mpz_get_ui(collision));
            structFree(inner_table);

            trail_length2 = structAdd(outer_table, start2, start1, collision, trail_length1, xDist_str);
            if (trail_length2) {
                if (collisionPcs(distCollision, start1, start2, trail_length1, trail_length2)) {
                    printf("repeated findCollision!, %lu \n", mpz_get_ui(distCollision));
                    mpz_set(collisions[counter], distCollision);
                    counter++;
                }
            }
        }
        flag = isNeedleMem(collisions, inverse_prob, counter);
        if (flag) goto found_needle;

        structFree(outer_table);
        printf("________________________________ \n");
    } while (1);

    found_needle:
    for (int i = 0; i < nb_collisions; ++i) {
        mpz_clear(collisions[i]);
    }
    free(collisions);

    mpz_clears(start1, start2, collision, distCollision, outer_flavor, NULL);
    gmp_randclear(r_state);
    gmp_randclear(r_state_pcs1);
    gmp_randclear(r_state_pcs2);

    structFree(outer_table);

    return flag;
}

int pcsRhoModeDetection(uint8_t n, uint8_t memory_init, uint8_t prob_init) {

    nb_bits = n;
    memory = memory_init;
    trailing_bits = trailingBitsInit();

    int nb_collisions = (int) pow(2, memory);
    int inverse_prob = (int) pow(2, prob_init);
    char xDist_str[50];

    int trail_length1, trail_length2;
    int trail_length_max = (int) pow(2, trailing_bits) * 13;
    int counter, flag;

    Table_t *outer_table;

    mpz_t *collisions = malloc(sizeof(mpz_t) * nb_collisions);
    for (int i = 0; i < nb_collisions; ++i) {
        mpz_init(collisions[i]);
    }

    mpz_t start1, start2, collision, distCollision;
    mpz_inits(start1, start2, collision, distCollision, inner_flavor, outer_flavor, NULL);

    gmp_randinit_default(r_state);
    gmp_randseed_ui(r_state, time(NULL));

    initF(nb_bits, prob_init);
    pcsInit(nb_bits, trailing_bits);

    do {
        counter = 0;

        outer_table = structInitHash(memory);
        // create random outer_flavor
        mpz_urandomb(outer_flavor, r_state, nb_bits);

        for (int i = 0; i < nb_collisions; ++i) {

            mpz_urandomb(start1, r_state, nb_bits);
            mpz_set(collision, start1);

            trail_length1 = 0;
            do {
                mpz_xor(collision, collision, outer_flavor);
                mpz_set(inner_flavor, collision);
                rho(collision, inner_flavor, collision);
                trail_length1++;

                //printf("collision found!, %lu \n", mpz_get_ui(collision));

                if (trail_length1 > trail_length_max) {
                    mpz_urandomb(start1, r_state, nb_bits);
                    mpz_set(collision, start1);
                    trail_length1 = 0;
                }

            } while (!isDistinguished(collision));

            //printf("distinguished collision found!, %lu \n", mpz_get_ui(collision));

            trail_length2 = structAdd(outer_table, start2, start1, collision, trail_length1, xDist_str);
            if (trail_length2) {
                if (collisionRho(distCollision, start1, start2, trail_length1, trail_length2)) {
                    printf("repeated findCollision!, %lu \n", mpz_get_ui(distCollision));
                    mpz_set(collisions[counter], distCollision);
                    counter++;
                }
            }
        }
        flag = isNeedleMem(collisions, inverse_prob, counter);
        if (flag) goto found_needle;

        structFree(outer_table);
        printf("________________________________ \n");
    } while (1);

    found_needle:
    for (int i = 0; i < nb_collisions; ++i) {
        mpz_clear(collisions[i]);
    }
    free(collisions);


    mpz_clears(start1, start2, collision, distCollision, inner_flavor, outer_flavor, NULL);
    gmp_randclear(r_state);

    structFree(outer_table);

    return flag;
}
