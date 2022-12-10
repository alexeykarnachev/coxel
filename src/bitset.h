#include <stddef.h>


typedef struct Bitset {
    unsigned int bits[16];
} Bitset;


void bitset_set_bit(Bitset* bitset, size_t idx);
int bitset_get_bit(Bitset* bitset, size_t idx);
void bitset_disable_bit(Bitset* bitset, size_t idx);

