// Universidad del Valle de Guatemala
// Ie2023: Programación de Microcontroladores
// Laboratorio 4
// Author: Rebeca Flores
// Hardware: ATMega328P
// Creado 06/04/2026
// Descripción: Contador binario 8 bits, con botones de incremento y decremento. También lee el valor de un potenciometro y despliega el valor del ADC


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

// Tabla HEX
const uint8_t seg_table_anodo[16] = {
	0xC0, // 0
	0xF9, // 1
	0xA4, // 2
	0xB0, // 3
	0x99, // 4
	0x92, // 5
	0x82, // 6
	0xF8, // 7
	0x80, // 8
	0x90, // 9
	0x88, // A
	0x83, // b
	0xC6, // C
	0xA1, // d
	0x86, // E
	0x8E  // F
};

// Function Prototypes
void setup(void);
void update_ports(void);
void check_buttons(void);
void adc_init(void);
uint16_t adc_read(void);
void display_hex(uint8_t value);

// Main Function
int main(void)
{
    setup();
	adc_init();
    while (1)
    {
        check_buttons();
        uint16_t adc_val = adc_read();
		uint8_t hex_val = adc_val >> 2;
		
		display_hex(hex_val);
    }
}

// NON-Interrupt Subroutines
void setup(void)
{
    cli();
	
    DDRB  |= 0x3F;
    PORTB &= ~0x3F;

    DDRC  |= (1 << PC1);
    DDRC  &= ~((1 << PC2) | (1 << PC3)); 
    PORTC |= (1 << PC2) | (1 << PC3);

    DDRD = 0x7F;

    DDRC |= (1 << PC4) | (1 << PC5);

    sei();
}

void adc_init(void)
{
	ADMUX = (1 << REFS0) | (1 << MUX2) | (1 << MUX1);

	ADCSRA = (1 << ADEN)
	| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_read(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

void display_hex(uint8_t value)
{
    uint8_t high = (value >> 4) & 0x0F;
    uint8_t low  = value & 0x0F;

    PORTC |= (1 << PC4) | (1 << PC5); // apagar ambos

    PORTD = seg_table_anodo[high];    // cargar segmentos

    PORTC &= ~(1 << PC4); // encender izquierdo
    _delay_ms(1);

    PORTC |= (1 << PC4) | (1 << PC5); // apagar ambos

    PORTD = seg_table_anodo[low];

    PORTC &= ~(1 << PC5); // encender derecho
    _delay_ms(1);
}

void update_ports(void)
{
    PORTB = (PORTB & 0xC0) | (counter & 0x3F);

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
