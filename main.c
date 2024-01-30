#include <avr/io.h>

const int LED_1 = 1 << PB3;
const int LED_2 = 1 << PB4;

const int ANAL_IN = 1 << PB2;

int main() {
  // Thomas
  ADMUX |= (1 << MUX1) | (1 << MUX0);
}