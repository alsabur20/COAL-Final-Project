.include "m328pdef.inc"
.include "uartMacros.inc"
.include "delayMacro.inc"
.include "divMacro.inc"

.def A = r16
.def AH = r17

.org 0x00
	; I/O Pins Configuration
	SBI DDRB, 5
	SBI DDRB, 4
	SBI DDRB, 3

	SBI PORTB, 5 
	CBI PORTB, 4
	CBI PORTB, 3

	; ADC Configuration
	LDI A,0b11000111 ; [ADEN ADSC ADATE ADIF ADIE ADIE ADPS2 ADPS1 ADPS0]
	STS ADCSRA,A
	LDI A,0b01100000 ; [REFS1 REFS0 ADLAR – MUX3 MUX2 MUX1 MUX0]
	STS ADMUX,A ; Select ADC0 (PC0) pin
	SBI PORTC,PC0 ; Enable Pull-up Resistor

	; macro to initilize the serial communication
	Serial_begin

loop:
    ; Start an ADC conversion by setting ADSC bit (bit 6)
    LDS A,ADCSRA ; Start Analog to Digital Conversion
	ORI A,(1<<ADSC)
	STS ADCSRA,A
    ; Wait until the ADSC bit has been cleared
adc_wait:
    LDS A,ADCSRA ; wait for conversion to complete
	sbrc A,ADSC
    rjmp adc_wait
    
	; Read ADC result
    LDS r18, ADCL
	LDS r18, ADCH	
	Serial_writeReg_ASCII r18
	Serial_writeNewLine
	delay 1000
    ; Compare ADC result with TRIGPOINT
    cpi r18, 200
    brsh smokeDetected
    
    SBI PORTB, 5
	CBI PORTB, 4
	CBI PORTB, 3
    rjmp loop
    
smokeDetected:
    CBI PORTB, 5
	SBI PORTB, 4
	SBI PORTB, 3
    rjmp loop
