.include "m328pdef.inc"
.include "uartMacros.inc"
.include "delayMacro.inc"

.def A = r16
.def AH = r17
.def mode = r18
.def alarmStatus = r19
.def serialInput = r20


.org 0x0000
	
	; I/O Pins Configuration
	SBI DDRB, 5		;Green LED
	SBI DDRB, 4		;Red LED
	SBI DDRB, 3		;Alarm

	SBI PORTB, 5	;Turning on the Green LED representing the normal state
	CBI PORTB, 4	;Turning off the Red LED in normal state
	CBI PORTB, 3	;Turning off the alarm in normal state
	
	; ADC Configuration
	LDI A,0b11000111 ; [ADEN ADSC ADATE ADIF ADIE ADIE ADPS2 ADPS1 ADPS0]
	STS ADCSRA,A
	LDI A,0b01100000 ; [REFS1 REFS0 ADLAR – MUX3 MUX2 MUX1 MUX0]
	STS ADMUX,A ; Select ADC0 (PC0) pin
	SBI PORTC,PC0 ; Enable Pull-up Resistor

	; macro to initilize the serial communication
	Serial_begin	;Initialization of Serial Communication

	ldi mode,'0'	;Clearing to remove garbage
	ldi alarmStatus,'0'		;Clearing to remove garbage

loop:
	ldi A, '0'
	Serial_read
	mov serialInput, r16	;reading serial for input if mode is changed

	;Comparing the read value if received
	cpi serialInput, '0'
	breq setAutoMode

	cpi serialInput, '1'
	breq setManualMode

	cpi serialInput, '2'
	breq alarmOn

	cpi serialInput, '3'
	breq alarmOff

	;Comparing and jumping to the selected working mode
	cpi mode,'0'
	breq automatic

	cpi mode, '1'
	breq manual

	cpi mode, '2'
	breq alarmOn

	cpi mode, '3'
	breq alarmOff

	rjmp automatic		;By default working in Automatic Mode

setAutoMode:
	ldi mode, '0'  ; '0' represents automatic mode
	rjmp loop

setManualMode:
	ldi mode, '1'  ; '1' represents manual mode
	rjmp loop

manual:	
	delay 1000
	rjmp loop

alarmOn:
	ldi mode, '2'
	CBI PORTB, 5	
	SBI PORTB, 4	
	SBI PORTB, 3
	rjmp loop

alarmOff:
	ldi mode, '3'
	SBI PORTB, 5
	CBI PORTB, 4
	CBI PORTB, 3
    rjmp loop

automatic:
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
    LDS A, ADCL
	LDS A, ADCH	

	;Send the reading serialy to ESP
	Serial_writeReg A
	delay 1000

    ; Compare ADC result with TRIGPOINT
    cpi A, 190
    brsh smokeDetected

    SBI PORTB, 5
	CBI PORTB, 4
	CBI PORTB, 3
    rjmp loop

smokeDetected:		;If smoke detected turn of the green light and turn on the alarm and red led
	CBI PORTB, 5	
	SBI PORTB, 4	
	SBI PORTB, 3
	rjmp loop	