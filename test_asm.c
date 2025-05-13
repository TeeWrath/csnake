int main() {
    int x = 42;
    asm("mov %0, %%eax" : "=r"(x) : "r"(x) : );
    printf("Value after asm: %d\n", x);
    return 0;
}