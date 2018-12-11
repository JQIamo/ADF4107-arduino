/*
   ADF4107.h - ADF4107 PLL Communication Library
   Created by Neal Pisenti, 2014.
   JQI - Strontium - UMD

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ADF4107_h
#define ADF4107_h

#include "Arduino.h"

// Register 0 - Reference counter bits 2-15
#define ADF4107_LDP_SLOW 0 << 20
#define ADF4107_LDP_FAST 1 << 20
// Anti-backlash pulse width. 0 = 2.9ns; 1 = DNU; 2 = 6ns; 3 = 2.9ns. 6ns recommended.
#define ADF4107_ABP(width) (width << 16)

// Register 1 - AB counter
#define ADF4107_CPG(gain) (gain << 21) // charge pump gain setting.

// Register 2, 3 - Function Latch/Initialization latch
#define ADF4107_PRESCALAR(value) (value << 22)
#define ADF4107_POWERDOWN(pd1, pd2) (pd1 << 3) | (pd2 << 21) // (0,0) for normal op
#define ADF4107_CPI2(val) (val << 18) // Charge pump current setting 2
#define ADF4107_CPI1(val) (val << 15) // CP current setting 1
#define ADF4107_TIMERCONTROL(val) (val << 11)
#define ADF4107_FASTLOCK_ENABLE(val) (val << 9)
#define ADF4107_FASTLOCK_MODE(val) (val << 10)
#define ADF4107_CPTRISTATE (1 << 8)
#define ADF4107_PDPOL_POS (1 << 7) // positive PD polarity
#define ADF4107_MUXOUT(val) (val << 4)
#define ADF4107_COUNTER_RESET (1 << 2)

#define ADF4107_REF  0
#define ADF4107_AB  1
#define ADF4107_FUNC  2
#define ADF4107_INIT 3

class ADF4107
{
  public:
    // Constructor function.
    // Creates PLL object, with given SS pin
    ADF4107(byte ssPin);
    // initialize function.
    // initialize(int P, int B, int A, int R).
    // final PLL output obeys RF = [(P*B + A)/R]*REF
    // P: 8, 16, 32, or 64
    // B: B counter; accepts integer 3-8191 (13 bit).
    //    B cannot take values 0, 1, or 2.
    // A: A counter; accepts integer 0-63 (6 bit)
    // R: reference divider. Accepts integer between
    //    1 and 16383 inclusive (14 bit)
    void initialize(int P, int B, int A, int R);
		// with pol
		void initialize(int P, int B, int A, int R, int pol);
		// with pol and mux
		void initialize(int P, int B, int A, int R, int pol, int mux);
    // update function. Same as initialize.
    // Upon initial power-up, should call initialize function. Afterwards,
    // can use update instead.
    void update(int P, int B, int A, int R, int pol = 1);

  private:
    // Instance variables that hold pinout mapping
    byte _ssPin;
    // register values
    int _p, _b, _a, _r, _mux;
    // function to write data to register.
    void writeRegisterData(unsigned long);
};

#endif
