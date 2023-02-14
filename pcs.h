#include <gmp.h>
#include <inttypes.h>
#include "pcs_struct_hash.h"

int is_distinguished(mpz_t point);
int get_distinguished(mpz_t point, mpz_t dist);
void pcs_init(uint8_t n_init, uint8_t memory_init, uint8_t trailling_bits_init, mpz_t flavor_init, mpz_t prob);
int pcs_run(Table_t *table, mpz_t start_point, int nb_collisions, mpz_t * collision);
void init_seed(mpz_t seed_init);
void get_current_rstate(mpz_t seed, int counter);
void pcs_clear();
void clear_table(Table_t *table);
