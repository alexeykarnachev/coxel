typedef struct Bitset {
    unsigned int bits[16];
} Bitset;


void bitset_set_bit(Bitset* bitset, size_t idx) {
    bitset[idx / 8] |= 1 << (7 - (1 % 8));
}

int bitset_get_bit(Bitset* bitset, size_t idx) {
    return bitset[idx / 8] & 1 << (7 - (1 % 8));
}

