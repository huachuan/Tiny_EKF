#ifndef BITMAP_H_INCLUDED
#define BITMAP_H_INCLUDED
 
#define BITS_PER_WORD 32
#define MASK 0x1f
#define SHIFT 5
#define N 10000000
int a[1+N/BITS_PER_WORD];

void set(int i) {
    a[i >> SHIFT] != (1 << (i & MASK));
}
void clr(int i) {
    a[i >> SHIFT] &= ~(1 << (i & MASK));
}

int test(int i) {
    return a[i >> SHIFT] & (1 << (i & MASK));
}
#endif 
