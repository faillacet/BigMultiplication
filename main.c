#include <stdio.h>
#include <inttypes.h>
#include <limits.h>

// Print Function for Debug
void bigprint(uint64_t val[], int sz) {
  for (int i =0; i< sz; i++) {
    printf("%016lx ", val[sz-1-i]);
  }
}

// Helper Functions
uint64_t getLSW(uint64_t x) {
  return x & 0xffffffff;
}

uint64_t getGSW(uint64_t x) { // Shifts down by 32 to leave on bit (the 33)
  return x >> 32;
}

// Works, however we must prove this by induction
uint32_t addto32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b) {
  // Assume that sz_b <= sz_a
  // Compute as += bs
  int i;  
  uint32_t c = 0;
  uint64_t sum;
  for (i=0; i< sz_b; i++) {
    sum = (uint64_t) as[i] + (uint64_t) bs[i] + (uint64_t)c;
    // s is a 33 bit value
    c = sum >> 32;
    as[i] = (uint32_t) sum;
  }
  
  for ( ; i< sz_a; i++) {                   //propogate carries across, if all fs
    sum = (uint64_t) as[i] + (uint64_t) c;
    // s is a 33 bit value
    c = sum >> 32;
    as[i] = (uint32_t)sum;
  }

  return c; // we return the carry bit
}

// Part 2 --- Calculates the partial product
uint32_t partialprod32(uint32_t a[], int szA, uint32_t x, uint32_t dest[], int szDest, uint32_t inheritedCarry) {
  // A is the big array
  // X is a single 32bit val (one index of our 2nd big array)
  // Dest is where we are putting the answer --- must be at least 1 index bigger than a 
  uint64_t tempM = 0; // used to calc the carries
  uint64_t tempS = 0;
  uint32_t carry = 0;
  int i;
  //dest += bs * d
  for (i = 0; i < szA; i++) {
    tempM = (uint64_t) x * (uint64_t) a[i];  // cast as 64 bits (up to 64 bit answer)
    tempS = (uint64_t)dest[i] + getLSW(tempM);
    carry += tempS >> 32;  
    dest[i] = getLSW(tempS);

    tempS = (uint64_t)dest[i+1] + getGSW(tempM) + (uint64_t)carry;
    carry = tempS >> 32;
    dest[i+1] = getLSW(tempS);
    if (i == szA - 1) {
      dest[i+1] += inheritedCarry;
    }
  }
  // how do we handle the carry
  return carry;
} // This takes the 

// What we need to work at the very end
void bigmul(uint64_t a[], int szA, uint64_t b[], int szB, uint64_t c[], int szC) {
  // dest size must be at least szA + szB
  // Convert A & B to arrays of uint32_t
  uint32_t *aP = (uint32_t *)a;
  uint32_t *bP = (uint32_t *)b;
  uint32_t *cP = (uint32_t *)c;   // c is destination
  uint32_t carry = 0;

  // loop that adds all partial products of (b[i] * a) to get final answer
  // Figure out how we line up the addition and handle the carry
  //printf("%016lx\n", b[1]);
  for (int i = 0; i < szC; i++) {
    carry = partialprod32(aP, szA*2, bP[i], &cP[i], szC*2, carry);
  }
}

int main(void) {
  static int size = 2;
  uint64_t a[2] = {0x666, 0x5555};
  uint64_t b[2] = {0x6, 0x5};
  uint64_t result[4] = {0x0, 0x0, 0x0, 0x0};

  uint32_t *aP = (uint32_t *)a;
  uint32_t *bP = (uint32_t *)b;
  uint32_t *rP = (uint32_t *)result;

  uint64_t *aP64 = a;
  uint64_t *bP64 = b;
  uint64_t *rP64 = result;
  uint32_t mult = 0x5;

  // Debug --- Testing
  // size of result must be at least szA + szB
  //partialprod32(aP, 4, mult, rP, 8);
  bigmul(aP64, 2, bP64, 2, rP64, 4);
  for (int i =0; i < 4; i++) {
    printf("%016lx\n", result[i]);
  }
  // End Debug
  return 0;
}
