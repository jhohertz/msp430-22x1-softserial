
## msp430 soft serial cli for launchpad with msp430g2211 or msp430g2231 

I had an older launchpad dev board for the msp430, version 1.2, that came with both of the following model chips:

- msp430g2211 [Datasheet](http://www.ti.com/lit/ds/symlink/msp430g2211.pdf) [Errata](http://www.ti.com/lit/er/slaz413d/slaz413d.pdf)
- msp430g2231 [Datasheet](http://www.ti.com/lit/ds/symlink/msp430g2231.pdf) [Errata](http://www.ti.com/lit/er/slaz417e/slaz417e.pdf)
- [Users Guide](http://www.ti.com/lit/ug/slau144j/slau144j.pdf) for both of the above

Both of these are tiny 14-pin, 16-bit microcontrollers, with just 2048 bytes of flash, and 128 bytes of ram, with 16 16-bit registers, including status. The msp430g2211 is very basic, offering 10 pins of general IO with interrupts and comparator functions, an 16-bit timer with 2 count comparators. The msp430g2231 has all of that but the IO comparator functions, but adds 8 channels of ADC that can be enabled on 8 of the pins, and a 'universal serial interface' which provides hardware assist for I2C or SPI interfacing. It also adds an internal temperature sensor connected as an extra ADC channel.

This code works on either of these chips, maybe others, and implements a sofware serial based on [code from Don Binder](http://dbindner.freeshell.org/msp430/#_send_and_receive_2). That code hardwires an echo from rx to tx. This code generalizes it for use, splitting the ring buffer into seperate send/receive buffers. Get/Put character functions were added for programs to interact with the serial interface, as well as a function to check size of buffers, and a non-blocking send that allows for retry on buffer-full conditions.

There is a reasonable makefile, which will work with apt-get installed msp430 tools on ubuntu 12.04 LTS, which you can then use mspdebug to erase and load the file in build/main.hex after running make. The makefile is based on the [one from uctools](https://github.com/uctools/msp430-template). It has additions of a disassembly of the final .elf file for debugging references, and a target to build and program one of the chips above in with one command:

    make program

To demonstrate the use of the serial, an example CLI implementation adapted from [code from Alan Bar](https://github.com/alanbarr/msp430-launchpad/tree/master/examples/simpleCli) is added, which lets you toggle LEDs, get "help", and has a hidden "reset" command.

After programming, it seems you need to disconnect/reconnect the launchpad to get at the terminal. I am using this to access the CLI after re-connecting:

    screen /dev/ttyACM0 2400

#### Limitations

- There is no hardware flow control. (Though this could be done if you wanted to devote the pins to it). Not in current plans
- It is locked to 2400 bps, 8-bit, no parity, 1 stop bit
- It assumes you are running of the internal 1Mhz clock source, which is the fasted calibrated source without adding a crystal. This is the scope for now.


