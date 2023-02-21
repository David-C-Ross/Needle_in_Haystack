#include <inttypes.h>

uint32_t jenkins(uint32_t a);
uint32_t hash32shift(uint32_t a);
uint32_t hash32shiftmult(int a);
uint32_t hashint( uint32_t a);
void get_offset(mpz_t point, mpz_t offset, mpz_t next);
void f(mpz_t point, mpz_t flavor);
void init_f(uint8_t n_init, uint8_t prob_init);