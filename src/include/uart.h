
#ifndef UART_H_
#define UART_H_

// size of rx/tx ring buffers (each of that size)
#define BSIZE 16                // must be power of 2

#define   TXD       BIT1
#define   RXD       BIT2

/* Ticks per bit, and ticks per half.  Use the following values based on speed:
 * 2400 bps ->  52
 * 1200 bps -> 104
 * I did not have success with slower speeds, like 300 bps.
 */
#define   TPB      52
#define   TPH      TPB/2

/* function prototypes */
void initUart( void );
inline void UART_Start( void );
unsigned int rx_size( void );
unsigned int tx_size( void );
const char uartGetChar( void );
int uartPutChar( const char );
void uartPrint(const char * );

#endif /*UART_H_*/

