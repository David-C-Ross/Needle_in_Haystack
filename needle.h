#include <gmp.h>
#include <inttypes.h>

void pcs_mode_detection(uint8_t n, uint8_t memory, mpz_t prob);
void rho_mode_detection(uint8_t n, mpz_t prob);
void nested_rho(mpz_t start_point, mpz_t collision);
void rho(mpz_t start_point, mpz_t collision);

