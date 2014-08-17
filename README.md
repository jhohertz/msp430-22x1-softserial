
## msp430 soft serial cli for launchpad with msp430g2211 or msp430g2231 

I had an older launchpad dev board for the 430, version 1.2, that came with both of the following model chips:

- msp430g2211 [Datasheet](http://www.ti.com/lit/ds/symlink/msp430g2211.pdf) [Errata](http://www.ti.com/lit/er/slaz413d/slaz413d.pdf)
- msp430g2231 [Datasheet](http://www.ti.com/lit/ds/symlink/msp430g2231.pdf) [Errata](http://www.ti.com/lit/er/slaz417e/slaz417e.pdf)

This code works on either of these chips, maybe others, and implements a sofware serial based on [code from Don Binder](http://dbindner.freeshell.org/msp430/#_send_and_receive_2). That code hardwires an echo from rx to tx. This code generalizes it for use, splitting the ring buffer into seperate send/receive buffers. Get/Put character functions were added for programs to interact with the serial interface, as well as a function to check size of buffers, and a non-blocking send that allows for retry on buffer-full conditions.

There is a reasonable makefile, which will work with apt-get installed msp430 tools on ubuntu 12.04 LTS, which you can then use mspdebug to erase and load the file in build/main.hex after running make.

#### Limitations

- There is no hardware flow control. (Though this could be done if you wanted to devote the pins to it). Not in current plans
- It is locked to 2400 bps
- It assumes you are running of the internal 1Mhz clock source, which is the fasted calibrated source without adding a crystal. This is the scope for now.


