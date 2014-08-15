
#include <msp430g2211.h>

/* Demo UART application.  Receives bytes from the computer
 * at 2400 bps, and sends the same byte back to the computer.
 */

#define   RED_LED   BIT0
#define   GRN_LED   BIT6

#define   TXD       BIT1
#define   RXD       BIT2

/* Ticks per bit, and ticks per half.  Use the following values based on speed:
 * 2400 bps ->  52
 * 1200 bps -> 104
 * I did not have success with slower speeds, like 300 bps.
 */
#define   TPB      52
#define   TPH      TPB/2

unsigned int TXWord = 0;
unsigned char RXByte = 0;
unsigned int rxbitcnt = 0;
unsigned int txbitcnt = 0;

/* a circular buffer to for characters received/to send */
#define BSIZE 16                // must be power of 2
unsigned char buffer[BSIZE];
unsigned int bhead=0, btail=0, bytestosend=0;

/* function prototypes */
void initUart( void );
inline void RX_Start( void );

void main(void) {
    /* stop the watchdog timer */
    WDTCTL = WDTPW + WDTHOLD;

    /* LEDs off, but we can use them for debugging if we want */
    P1DIR |= RED_LED+GRN_LED;
    P1OUT &= ~ (RED_LED + GRN_LED );

    initUart();

    /* Start listening for data */
    RX_Start();

    for( ; ; ) {
        /* go to sleep and wait for data */
        __bis_SR_register( LPM0_bits + GIE );
    }
}

void initUart( void ) {
    /* set up the clocks for 1 mhz */
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
    BCSCTL2 &= ~(DIVS_3);

    /* Set timer A to use continuous mode 1 mhz / 8 = 125 khz. */
    TACTL = TASSEL_2 + MC_2 + ID_3;

    /* When TXD isn't being used, it should be high */
    CCTL0 = OUT;

    /* TXD and RXD set for timer function, RXD input, TXD output */
    P1SEL |= TXD + RXD;
    P1DIR &= ~ RXD;
    P1DIR |= TXD;
}

/* This continuously sends bits of the TXWord starting from the
 * least significant bit (the 0 start bit).  One bit is sent every
 * time the handler is activated.  When the bits run out, a new
 * byte is loaded from the data pointer, until bytestosend equals 0.
 */
void TimerA0 (void) __attribute__((interrupt(TIMERA0_VECTOR)));
void TimerA0(void) {
    if( txbitcnt ) {
        /* send least significant bit */
        if( TXWord & 0x01 ) {
            CCTL0 &= ~ OUTMOD2;
        } else {
            CCTL0 |= OUTMOD2;
        }
        TXWord >>= 1;
        txbitcnt --;
    }

    /* If there are no bits left, load the next byte */
    if( !txbitcnt ) {
        if( bytestosend ) {
            /* load next byte with stop bit 0x100 and shifted left
             * to make the start bit */
            TXWord = ( 0x100 | buffer[btail++]) << 1;
            btail &= BSIZE-1;
            bytestosend --;

            /* 1 start bit + 8 data bits + 1 stop bit */
            txbitcnt = 10;
        } else {
            /* turn off interrupts if not receiving */
            if( ! rxbitcnt ) CCTL0 &= ~ CCIE;
        }
    }

    /* add ticks per bit to trigger again on next bit in stream */
    CCR0 += TPB;
    /* reset the interrupt flag */
    CCTL0 &= ~CCIFG;
}

void RX_Start( void ) {
    /* Make ready to receive character.  Syncronize, negative edge
     * capture, enable interrupts.
     */
    rxbitcnt = 8;
    CCTL1 = SCS + OUTMOD0 + CM1 + CAP + CCIE;
}

void TimerA1 (void) __attribute__((interrupt(TIMERA1_VECTOR)));
void TimerA1(void) {
    /* If we just caught the 0 start bit, then turn off capture
     * mode (it'll be all compares from here forward) and add
     * ticks-per-half so we'll catch signals in the middle of
     * each bit.
     */
    if( CCTL1 & CAP ) {
        /* 8 bits pending */
        rxbitcnt = 8;

        /* next interrupt in 1.5 bits (i.e. in middle of next bit) */
        CCR1 += TPH + TPB;

        /* reset capture mode and interrupt flag */
        CCTL1 &= ~ ( CAP + CCIFG );

        /* turn on transmitting also if needed */
        if( ! (CCTL0 & CCIE)) {
            /* interleave the interrupts, transmit half-bit after receive */
            CCR0 = CCR1 + TPH;
            CCTL0 = CCIS0 + OUTMOD0 + CCIE;
        }

        return;
    }

    /* Otherwise we need to catch another bit.  We'll shift right
     * the currently received data, and add new bits on the left.
     */
    RXByte >>= 1;
    if( CCTL1 & SCCI ) {
        RXByte |= 0x80;
    }
    rxbitcnt --;

    /* last bit received */
    if( ! rxbitcnt ) {
        /* Record this byte and reset for next.
         * Put character in circular buffer (unless full).
         */
        if( bytestosend < BSIZE ) {
            buffer[bhead++] = RXByte;
            bhead &= BSIZE-1;
            bytestosend ++;
        }

        /* we're done, reset to capture */
        CCTL1 = SCS + OUTMOD0 + CM1 + CAP + CCIE;

        return;
    }

    /* add ticks per bit to trigger again on next bit in stream */
    CCR1 += TPB;

    /* reset the interrupt flag */
    CCTL1 &= ~CCIFG;
}

