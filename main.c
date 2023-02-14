#include "stdlib.h"
#include "stdio.h"
#include <inttypes.h>
#include <time.h>
#include "needle.h"
#include "pcs_storage.h"
#include "pcs.h"


int main(int argc,char * argv[]) {
    uint8_t n = 20, memory = 5;

    mpz_t prob;
    mpz_init_set_ui(prob, 17);

    pcs_mode_detection(n, memory, prob);
    rho_mode_detection(n, prob);

    /*
    mpz_t flavor;
    mpz_init(flavor);

    int nb_collisions = 20;
    mpz_t *collisions = malloc( sizeof(mpz_t) * nb_collisions);
    for (int i = 0; i <nb_collisions; ++i) {
        mpz_init(collisions[i]);
    }
    hashUNIX_t ** inner_table = pcs_init(n, memory, 6, flavor, prob);
    mpz_set_ui(prob, time(NULL));
    init_seed(prob);
    pcs_run(inner_table,prob, nb_collisions, collisions);
    for (int i = 0; i <nb_collisions; ++i) {
        printf("%lu \n", mpz_get_ui(collisions[i]));
    }
    clear_table(inner_table);
    pcs_clear();

    free(collisions);
    mpz_clear(flavor);
     */

    mpz_clear(prob);

    return 0;
}