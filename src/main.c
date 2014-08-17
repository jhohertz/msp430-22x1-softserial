
#include "config.h"
#include "uart.h"
#include "cli.h"

/* Demo UART application.  Receives bytes from the computer
 * at 2400 bps, and sends the same byte back to the computer.
 */

int main(void) {
    /* stop the watchdog timer */
    WDTCTL = WDTPW + WDTHOLD;

    /* set up the clocks for 1 mhz */
    BCSCTL1  = CALBC1_1MHZ;       // Set range
    DCOCTL   = CALDCO_1MHZ;
    BCSCTL2 &= ~(DIVS_3);         // SMCLK = DCO / 8 = 1Mhz

    /* LEDs off, but we can use them for debugging if we want */
    P1DIR |= RED_LED+GRN_LED;
    P1OUT &= ~ (RED_LED + GRN_LED );

    initUart();

    /* Start listening for data */
    UART_Start();

    /* enable interrupts */
    __bis_SR_register( GIE );

    uartPrint("Cli Started.\n\r");
    cliHelp();
    uartPrint(PROMPT);

    char in_char;
    while(1) {
        while(rx_size() > 0) {
            in_char = uartGetChar();
            uartPutChar(in_char);
            cli_input(in_char);
        }
        /* go to sleep and wait for data */
        __bis_SR_register( LPM0_bits );
    }
}
