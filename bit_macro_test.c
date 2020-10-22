#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

int main(void) {
  // input bitfield
  uint16_t inputs = 0;
  // set bits
  PU_SET_BIT(inputs, 0);
  printf("%u\n", inputs);
  PU_SET_BIT(inputs, 2);
  printf("%u\n", inputs);
  PU_SET_BIT(inputs, 4);
  printf("%u\n", inputs);
  PU_SET_BIT(inputs, 6);
  printf("%u\n", inputs);

  PU_SET_BIT(inputs, 8);
  printf("%u\n", inputs);
  PU_SET_BIT(inputs, 10);
  printf("%u\n", inputs);
  PU_SET_BIT(inputs, 12);
  printf("%u\n", inputs);
  PU_SET_BIT(inputs, 14);
  printf("%u\n", inputs);
  //clear bit
  PU_CLR_BIT(inputs, 0);
  printf("%u\n", inputs);
  PU_CLR_BIT(inputs, 0);
  printf("%u\n", inputs);
  PU_CLR_BIT(inputs, 8);
  printf("%u\n", inputs);
  PU_CLR_BIT(inputs, 12);
  printf("%u\n", inputs);
  // is bit set and print bitfield
  for (size_t i = 0; i < 16; i++) {
    if (PU_IS_BIT_SET(inputs, i)){
      printf("+");
    }else{
      printf(".");
    }
  }
  printf("\n");

  return 0;
}
