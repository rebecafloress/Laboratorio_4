// Universidad del Valle de Guatemala
// Ie2023: Programación de Microcontroladores
// Pre Laboratorio 4.asm

// Author: Rebeca Flores
// Hardware: ATMega328P
// Creado 06/04/2026
// Descripción: Contador binario 8 bits, con botones de incremento y decremento.


// Libraries
#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


// Constants & Global Variables
#define DEBOUNCE_DELAY 50    // ms para antirebote

volatile uint8_t counter = 0;
volatile uint8_t btn_up_prev   = 1;   // Estado anterior PB+ (PC2)
volatile uint8_t btn_down_prev = 1;   // Estado anterior PB- (PC3)


// Function Prototypes
void setup(void);
void update_ports(void);
void check_buttons(void);

// Main Function
int main(void)
{
    setup();

    while (1)
    {
        check_buttons();
    }

    return 0;
}

// NON-Interrupt Subroutines
void setup(void)
{
    cli();

    DDRB  |= (1 << PB0) | (1 << PB1) | (1 << PB2) |
             (1 << PB3) | (1 << PB4) | (1 << PB5);
    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) |
               (1 << PB3) | (1 << PB4) | (1 << PB5));

    DDRC  |=  (1 << PC0) | (1 << PC1);
    DDRC  &= ~((1 << PC2) | (1 << PC3));   // entradas
    PORTC |=  (1 << PC2) | (1 << PC3);     // pull-up activado
    PORTC &= ~((1 << PC0) | (1 << PC1));   // salidas a 0

    sei();
}

void update_ports(void)
{
    // Bits 0-5 ? PB0-PB5
    PORTB = (PORTB & 0xC0) | (counter & 0x3F);

    // Bits 6-7 ? PC0-PC1
    PORTC = (PORTC & 0xFC) | ((counter >> 6) & 0x03);
}

// Antirrebote y lectura de botones
void check_buttons(void)
{
    uint8_t btn_up   = (PINC >> PC2) & 0x01;
    uint8_t btn_down = (PINC >> PC3) & 0x01;

    // Botón Incremento (PC2)
    if (btn_up == 0 && btn_up_prev == 1)
    {
        _delay_ms(DEBOUNCE_DELAY);
        btn_up = (PINC >> PC2) & 0x01;

        if (btn_up == 0)
        {
            counter++;
            update_ports();
        }
    }

    // Botón Decremento (PC3)
    if (btn_down == 0 && btn_down_prev == 1)
    {
        _delay_ms(DEBOUNCE_DELAY);
        btn_down = (PINC >> PC3) & 0x01;

        if (btn_down == 0)
        {
            counter--;
            update_ports();
        }
    }

    btn_up_prev   = btn_up;
    btn_down_prev = btn_down;
}

// Interrupt Routines
