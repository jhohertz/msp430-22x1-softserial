
/* Soft UART Interface. Full duplex, buffered, 2400bps
 * using the default calibrated 1Mhz clock.
 * not perfect, but ok...
 */

#include "config.h"
#include "uart.h"

unsigned int TXWord = 0;
unsigned char RXByte = 0;
unsigned int rx_bitcnt = 0;
unsigned int tx_bitcnt = 0;

/* a circular buffer to for characters received/to send */
volatile char rx_buffer[BSIZE];
volatile char tx_buffer[BSIZE];
volatile unsigned int rx_bhead=0, rx_btail=0, tx_bhead=0, tx_btail=0;

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

const char uartGetChar( void ) {
    //if( rx_btail != rx_bhead ) {
    if(rx_size() > 0) {
        const char rx_char = rx_buffer[rx_btail++];
        rx_btail &= BSIZE-1;
        return rx_char;
    }
    // nothing here, should not have called
    return 0;
}

/* Print a NULL terminated string over UART.
 * string - null terminated sting to transmit over UART. */
void uartPrint(const char *s)
{
    while (*s) 
    {
        uartPutChar(*s++);
    }
}

// put a character into the tx buffer if there is room
int uartPutChar( const char c ) {
    //if( tx_btail != ((tx_bhead + 1) & (BSIZE-1)) ) {
    if( tx_size() < (BSIZE-1) ) {
        tx_buffer[tx_bhead++] = c;
        tx_bhead &= BSIZE-1;
        // ensure interrupt enabled, sync w/ rx
        //CCR0 = CCR1 + TPH;
        //CCTL0 = CCIS0 + OUTMOD0 + CCIE;
        // ensure interrupt enabled for send
        if( ! (CCTL0 & CCIE) ) {
           // CCTL0 |= CCIE;
           CCR0 = TAR;         // Current state of TA counter
           CCR0 += TPB;        // One bit time till first bit
	   CCTL0 = OUTMOD0 + CCIE;
           //CCTL0 = CCIS0 + OUTMOD0 + CCIE;
        }
        return 0;
    }
    // full
    return 1;
}

// current load in buffer, based on head/tail seperation
unsigned int rx_size( void ) {
    if( rx_btail <= rx_bhead ) {
        return(rx_bhead - rx_btail);
    } else {
        return((rx_bhead + BSIZE) - rx_btail);
    }
}

// current load in buffer, based on head/tail seperation
unsigned int tx_size( void ) {
    if( tx_btail <= tx_bhead ) {
        return(tx_bhead - tx_btail);
    } else {
        return((tx_bhead + BSIZE) - tx_btail);
    }
}


/* This continuously sends bits of the TXWord starting from the
 * least significant bit (the 0 start bit).  One bit is sent every
 * time the handler is activated.  When the bits run out, a new
 * byte is loaded from the data pointer, until tx_btail catches up
 * to tx_bhead.
 */
void TimerA0 (void) __attribute__((interrupt(TIMERA0_VECTOR)));
void TimerA0(void) {
    if( tx_bitcnt ) {
        /* send least significant bit */
        if( TXWord & 0x01 ) {
            CCTL0 &= ~ OUTMOD2;
        } else {
            CCTL0 |= OUTMOD2;
        }
        TXWord >>= 1;
        tx_bitcnt --;
    }

    /* If there are no bits left, load the next byte */
    if( !tx_bitcnt ) {
        if( tx_size() > 0 ) {
            /* load next byte with stop bit 0x100 and shifted left
             * to make the start bit */
            TXWord = ( 0x100 | tx_buffer[tx_btail++]) << 1;
            tx_btail &= (BSIZE - 1);
            /* 1 start bit + 8 data bits + 1 stop bit */
            tx_bitcnt = 10;
        } else {
            /* turn off interrupts if not receiving */
            if( ! rx_bitcnt ) CCTL0 &= ~ CCIE;
        }
    }


    /* add ticks per bit to trigger again on next bit in stream */
    CCR0 += TPB;
    /* reset the interrupt flag */
    CCTL0 &= ~CCIFG;

}

void UART_Start( void ) {
    /* Make ready to receive character.  Syncronize, negative edge
     * capture, enable interrupts.
     */
    rx_bitcnt = 8;
    CCTL1 = SCS + OUTMOD0 + CM1 + CAP + CCIE;
    // sync tx on half intervals of rx
    //CCR0 = CCR1 + TPH;
    //CCTL0 = CCIS0 + OUTMOD0 + CCIE;
    //CCTL0 = CCIS0 + OUTMOD0;

}

// receive interrupt
void TimerA1 (void) __attribute__((interrupt(TIMERA1_VECTOR)));
void TimerA1(void) {
    /* If we just caught the 0 start bit, then turn off capture
     * mode (it'll be all compares from here forward) and add
     * ticks-per-half so we'll catch signals in the middle of
     * each bit.
     */
    if( CCTL1 & CAP ) {
        /* 8 bits pending */
        rx_bitcnt = 8;

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
    rx_bitcnt --;


    /* last bit received */
    if( ! rx_bitcnt ) {
        /* Record this byte and reset for next.
         * Put character in circular buffer (unless full).
         */
        // all indications, this block working as it should
        if( rx_btail != ((rx_bhead + 1) & (BSIZE-1)) ) {
        //if( rx_size() < (BSIZE-1) ) {
            rx_buffer[rx_bhead++] = RXByte;
            rx_bhead &= BSIZE-1;
        }

        /* we're done, reset to capture */
        CCTL1 = SCS + OUTMOD0 + CM1 + CAP + CCIE;

        // wakeup, as program may want to process received data
        __bic_SR_register_on_exit(LPM0_bits);

        return;
    }

    /* add ticks per bit to trigger again on next bit in stream */
    CCR1 += TPB;

    /* reset the interrupt flag */
    CCTL1 &= ~CCIFG;
}

