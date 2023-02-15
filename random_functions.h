#include <inttypes.h>

uint32_t jenkins( uint32_t a);
void get_next(mpz_t point, mpz_t offset, mpz_t next);
void f(mpz_t point);
void init_f(uint8_t n_init, mpz_t flavor_init, mpz_t threshold_init);