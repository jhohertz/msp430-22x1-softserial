
#include "config.h"
#include "uart.h"

/* Demo UART application.  Receives bytes from the computer
 * at 2400 bps, and sends the same byte back to the computer.
 */

#define   RED_LED   BIT0
#define   GRN_LED   BIT6

void main(void) {
    /* stop the watchdog timer */
    WDTCTL = WDTPW + WDTHOLD;

    /* LEDs off, but we can use them for debugging if we want */
    P1DIR |= RED_LED+GRN_LED;
    P1OUT &= ~ (RED_LED + GRN_LED );

    initUart();

    /* Start listening for data */
    UART_Start();

    uartPrint("Hello World!\n\r");
    for( ; ; ) {
        //if( rx_btail != rx_bhead ) {
        while(rx_size() > 0) {
        P1OUT |= RED_LED;
            P1OUT &= ~ (RED_LED);
        P1OUT &= ~ (GRN_LED);
        P1OUT |= GRN_LED;

            uartPutChar(uartGetChar());
        }
        /* go to sleep and wait for data */
        __bis_SR_register( LPM0_bits + GIE );
    }
}
