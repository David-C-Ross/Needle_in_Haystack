#include<gmp.h>
#include <inttypes.h>
#include "pcs_struct_hash.h"

Table_t *struct_init(uint8_t memory);
int structAdd(Table_t *table, mpz_t a_out, mpz_t a_in, mpz_t xDist, int length, char xDist_str[]);
void structFree(Table_t *table);