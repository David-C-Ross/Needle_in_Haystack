#include <gmp.h>
#include <inttypes.h>
#include "pcs_struct_hash.h"

int isDistinguished(mpz_t point);
int getDistinguished(mpz_t point, mpz_t dist, gmp_randstate_t r_state);
void pcsInit(uint8_t n_init, uint8_t trailling_bits_init);
void pcsRun(Table_t *table, mpz_t flavor_init, int nb_collisions, gmp_randstate_t r_state, mpz_t * collisions);
void pcsClear();
