#include "stdlib.h"
#include "stdio.h"
#include <inttypes.h>
#include <time.h>
#include <math.h>
#include "needle_pcs.h"
#include "needle_rho.h"
#include "pcs_storage.h"
#include "pcs.h"
#include "random_functions.h"

int main(int argc,char * argv[]) {
    // the needle will appear 2^5=32 times, hash has been checked to not contain any other needles of similar magnitude.
    uint8_t n = 24, memory = 6, prob = 18;

    //pcs_mode_detection(n, memory, prob);
    //rho_mode_detection(n, prob);
/*
    mpz_t start, seed, inner_flavor;
    mpz_inits(start, seed, inner_flavor, NULL);

    int counter = 0;
    int nb_collisions = 10000;

    gmp_randstate_t r_state;
    gmp_randinit_default(r_state);
    gmp_randseed_ui(r_state, time(NULL));

    mpz_urandomb(start, r_state, n);
    mpz_urandomb(inner_flavor, r_state, n);

    mpz_t *collisions = malloc( sizeof(mpz_t) * nb_collisions);
    for (int i = 0; i < nb_collisions; ++i) {
        mpz_init(collisions[i]);
    }
    Table_t *inner_table = struct_init(n, memory);
    pcs_init(n, prob, 9, inner_flavor);

    mpz_set_ui(seed, time(NULL));
    init_seed(seed);

    pcs_run(inner_table, start, nb_collisions, collisions);
    for (int i = 0; i <nb_collisions; ++i) {
        printf("%d: %lu \n", i, mpz_get_ui(collisions[i]));
        if (mpz_cmp_ui(collisions[i], 1) == 0) counter++;
    }
    printf("%d", counter);
    clear_table(inner_table);
    pcs_clear();

    free(collisions);
    mpz_clears(start, seed, inner_flavor, NULL);
*/
    return 0;
}