#include <gmp.h>
#include <inttypes.h>

typedef struct Table
{
    unsigned int seed;
    uint32_t length;
    char * start;
    char * distinguished;

}Table_t;

void mode_detection(uint8_t n, uint8_t memory, mpz_t prob);
void rho_mode_detection(uint8_t n, mpz_t prob);
void rho(mpz_t start_point, mpz_t collision);

