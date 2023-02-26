#include<gmp.h>
#include "pcs_storage.h"

/** Initialize the distinguished-point-storing structure.
 * 
 */
Table_t *struct_init(uint8_t memory)
{
    return structInitHash(memory);
}

/** Search and insert.
 *  
 *  @brief Look for a point in the structure. If the point is not found 
 *  it is added with the corresponding start point.
 *  
 *  @param[out]	a_out	The found point.
 *  @param[in]	a_in	The newly added point.
 *  @param[in]	xDist	The x coordinate, without the trailing zeros.
 *  @param[in]	length	The length of the trail.
 *  @return 	length of trail if the point was found, 0 otherwise.
 */
int structAdd(Table_t *table, mpz_t a_out, mpz_t a_in, mpz_t xDist, int length, char xDist_str[]) {

    int trail_length;
    mpz_get_str(xDist_str, 62, xDist);
    trail_length = structAddHash(table, a_out, a_in, length, xDist_str);
    return trail_length;
}

/** Free the distinguished-point-storing structure.
 * 
 */
void structFree(Table_t *table) {

    structFreeHash(table);
}
