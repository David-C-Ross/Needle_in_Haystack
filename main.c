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
    uint8_t n = 30, memory = 6;

    mpz_t prob;
    mpz_init_set_ui(prob, 24); // the needle will appear 2^6=64 times, jenkins hash has been checked for 2^30 to not contain any other needles of similar magnitude.

    //pcs_mode_detection(n, memory, prob);
    rho_mode_detection(n, prob);

    /*
    mpz_t flavor;
    mpz_init(flavor);

    int nb_collisions = 200;
    mpz_t *collisions = malloc( sizeof(mpz_t) * nb_collisions);
    for (int i = 0; i <nb_collisions; ++i) {
        mpz_init(collisions[i]);
    }
    Table_t *inner_table = struct_init(n, memory);
    pcs_init(n, memory, 6, flavor, prob);
    mpz_set_ui(prob, time(NULL));
    init_seed(prob);
    pcs_run(inner_table,prob, nb_collisions, collisions);
    for (int i = 0; i <nb_collisions; ++i) {
        printf("%d: %lu \n", i, mpz_get_ui(collisions[i]));
    }
    clear_table(inner_table);
    pcs_clear();

    free(collisions);
    mpz_clear(flavor);
     */

    mpz_clear(prob);

    return 0;
}