// bitwise_test.c
int main() {
    unsigned int flags = 0;
    
    // Set bit 0
    flags |= 0x1;
    
    // Toggle bits 1 and 2
    flags ^= 0x6;
    
    // Shift left by 2
    int shifted = flags << 2;
    
    // Mask lower 8 bits
    int masked = flags & 0xFF;
    
    return 0;
}