#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>

typedef enum { false, true } bool;

// PB3 = GREEN
// PB1 = YELLOW
// PB0 = RED

// PB5 = Fus RED (unused)
// PB4 = Fus GREEN

// PB2 (INT0) = Interrupt Fußgänger buton

const int ontime_ms = 2000;
const int offtime_ms = 5000;

const int switch_state_dur_ms = 1000;

volatile bool fus_pressed = false;

ISR(INT0_vect) {
  if (PINB & (1 << PB2)) {
    fus_pressed = true;
    __asm__ volatile("cli" ::: "memory");
  }
}

const int offphases[] = {
    (1 << PB3) | (1 << PB5),  // Green
    (1 << PB1) | (1 << PB5),  // Yellow
    (1 << PB0) | (1 << PB5),  // Red
};

const int onphases[] = {
    (1 << PB0) | (1 << PB5),               // Red
    (1 << PB0) | (1 << PB5) | (1 << PB1),  // Red + Yellow
    (1 << PB3) | (1 << PB5)                // Fus Red + Green
};

void sleep() {
  // Set sleep mode to Power-down
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  // Enable external interrupt INT0
  EIMSK |= (1 << INT0);

  // Trigger INT0 on any logical change
  EICRA |= (1 << ISC00);

  // Enable global interrupts
  sei();

  // Enable sleep
  sleep_enable();

  // Put the device to sleep
  sleep_mode();

  // Upon waking up, sketch continues from this point.
  sleep_disable();
}

int main() {
  __asm__ volatile("sei" ::: "memory");
  GIMSK |= (1 << 6);
  PCMSK |= (1 << INT0);
  DDRB = (1 << PB0) | (1 << PB1) | (0 << PB2) | (1 << PB3) | (1 << PB4) |
         (1 << PB5);
  while (1) {
    for (int i = 0; i < sizeof(onphases) / sizeof(int); i++) {
      PORTB = onphases[i];
      _delay_ms(switch_state_dur_ms);
    }
    _delay_ms(ontime_ms);
    // Blink GREEN
    for (int i = 0; i < 4; i++) {
      _delay_ms(500);
      PORTB = (0 << PB3) | (1 << PB5);
      _delay_ms(500);
      PORTB = (1 << PB3) | (1 << PB5);
    }

    for (int i = 0; i < sizeof(offphases) / sizeof(int); i++) {
      PORTB = offphases[i];
      _delay_ms(switch_state_dur_ms);
    }
    // Check if unser Fußgänger wü drüwa
    if (fus_pressed) {
      PORTB = (1 << PB4) | (1 << PB0);
    }
    _delay_ms(offtime_ms);
    if (fus_pressed) {
      for (int i = 0; i < 4; i++) {
        _delay_ms(500);
        PORTB = (1 << PB0) | (1 << PB4);
        _delay_ms(500);
        PORTB = (1 << PB0) | (0 << PB4);
      }
    }
    fus_pressed = false;
  }
  return 0;
}