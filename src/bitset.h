typedef struct Bitset {
    unsigned int bits[16];
} Bitset;


void bitset_set_bit(Bitset* bitset, size_t idx) {
    bitset->bits[idx / 8] |= 1 << (7 - (idx % 8));
}

int bitset_get_bit(Bitset* bitset, size_t idx) {
    int res = bitset->bits[idx / 8] & (1 << (7 - (idx % 8)));
    return res;
}

void bitset_disable_bit(Bitset* bitset, size_t idx) {
    bitset->bits[idx / 8] &= ~(1 << (7 - (idx % 8)));
}
