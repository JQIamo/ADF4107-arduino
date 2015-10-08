# ADF4107

Communication library, for use with Analog Devices' ADF4107 PLL chip. Designed for interfacing between ADF4107 and Arduino Due. Other Arduino versions might work, but milage may vary.

## Getting Started

In your Arduino Sketch, make sure to do something like:


    #include <SPI.h>
    #include <ADF4107.h>
    
    
    ADF4107 DPLL(COM_PIN);
    
    
    void setup(){
        SPI.begin();
        SPI.setClockDivider(6);
        SPI.setDataMode(SPI_MODE0); // this might be unnecessary - must check default!
        
        
        DPLL.initialize(P_VAL, B_VAL, A_VAL, R_VAL);
        // or use  ADF4107::initialize(int P, int B, int A, int R, int pol)
    }
    

## Important functions

`ADF4107::initialize(int P, int B, int A, int R)`

This function initializes the ADF4107 after initial power-up. 

* P: prescalar value; can be 8, 16, 32, or 64.
* B: B counter; accepts integer 3-8191 (13 bit). B cannot take values 0, 1, or 2. 
* A: A counter; accepts integer 0-63 (6 bit).
* R: Reference divider; accepts integer 1-16383 (14 bit).

The final PLL frequency relationship is `RF_FREQ = [(P*B+A)/R]*REF_FREQ`.

If you want to specify a polarity for the PLL, add it as the final flag, eg, `ADF4107::initialize(int P, int B, int A, int R, int pol)` where `pol` is either `0` or `1`.
You can also specify what you want to be sent to the mux pin by adding a further flag i.e. `ADF4107::initialize(int P, int B, int A, int R, int pol, int mux)` where `mux` may take values from `0` to `7` corresponding to:

* `0` - Three state output
* `1` - Digital lock detect (output high)
* `2` - N divider output
* `3` - DV_{DD}
* `4` - R divider output
* `5` - N channel open-drain lock detect
* `6` - Serial data output
* `7` - DGND

Enjoy!
