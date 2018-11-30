#include <avr/io.h>
#include <util/delay.h>

int main (void)
{
    // Set Data Direction to output on port B, pins 2 and 3:
    DDRH |= 0b00101000;
    while (1) {
        // set PB3 high
        PORTH = 0b00100000;
        _delay_ms(1000);
        // set PB3 low
        PORTH = 0b00000000;
        _delay_ms(1000);
    }

    return 1;
}