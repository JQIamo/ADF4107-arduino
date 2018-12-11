/*
   ADF4107.cpp - ADF4107 PLL Communication Library
   Created by Neal Pisenti, 2014.
   Some additions by Sandy Craddock (marked by SC)
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

#include "Arduino.h"
#include "SPI.h"
#include "ADF4107.h"

// General Considerations:
//
// SPI should be configured MSB (which is the default, perhaps?)
// Additionally, the maximum clock rate is 20MHz, so for Arduino Due you
// need the appropriate clock divider.
// SPI should be Mode 0.

// Not everything is implemented below -- ie, I've chosen some
// (hopefully) intelligent defaults for things like charge pump gain,
// CP current, etc.
// In the future, if there is time/need, more functionality can be exposed
// to the Arduino API.
// At the end of the day, you can always write the registers yourself!

// Some useful #define statements, for writing to the registers.
// When in doubt, consult the data sheet!

// Register 0 - Reference counter bits 2-15
// #define ADF4107_LDP_SLOW 0 << 20
// #define ADF4107_LDP_FAST 1 << 20
// Anti-backlash pulse width. 0 = 2.9ns; 1 = DNU; 2 = 6ns; 3 = 2.9ns. 6ns recommended.
// #define ADF4107_ABP(width) (width << 16)

// Register 1 - AB counter
// #define ADF4107_CPG(gain) (gain << 21) // charge pump gain setting.

// Register 2, 3 - Function Latch/Initialization latch
// #define ADF4107_PRESCALAR(value) (value << 22)
// #define ADF4107_POWERDOWN(pd1, pd2) (pd1 << 3) | (pd2 << 21) // (0,0) for normal op
// #define ADF4107_CPI2(val) (val << 18) // Charge pump current setting 2
// #define ADF4107_CPI1(val) (val << 15) // CP current setting 1
// #define ADF4107_TIMERCONTROL(val) (val << 11)
// #define ADF4107_FASTLOCK_ENABLE(val) (val << 9)
// #define ADF4107_FASTLOCK_MODE(val) (val << 10)
// #define ADF4107_CPTRISTATE (1 << 8)
// #define ADF4107_PDPOL_POS (1 << 7) // positive PD polarity
// #define ADF4107_MUXOUT(val) (val << 4)
// #define ADF4107_COUNTER_RESET (1 << 2)

// Register control bits
// #define ADF4107_REF = 0
// #define ADF4107_AB = 1
// #define ADF4107_FUNC = 2
// #define ADF4107_INIT = 3

// ADF4107: constructor function.
// ssPin is the arduino pin number to use for chip select.
ADF4107::ADF4107(byte ssPin) {
  _ssPin = ssPin;
  pinMode(_ssPin, OUTPUT);
  digitalWrite(_ssPin, HIGH);
}

void ADF4107::initialize(int P, int B, int A, int R){
	// Defaults to the mux providing N divider output and polarity of 1, used for locking the slave laser to the blue of the master
	ADF4107::initialize(P, B, A, R, 1, 2);
}

void ADF4107::initialize(int P, int B, int A, int R, int pol){
	// Defaults to the mux providing N divider output
	ADF4107::initialize(P, B, A, R, pol, 2);
}

// initialize the ADF4107.
// R: reference divider. Accepts integer between 1 and 16383 inclusive (14 bit)
// P: Prescalar. must be 8, 16, 32, or 64. Additionally, (RF/prescalar) must be less than 300 MHz.
// A: A counter; accepts integer 0-63 (6 bit)
// B: B counter; accepts integer 3-8191 (13 bit). B cannot take values 0, 1, or 2.
// The final multiplier is RF = [(P*B + A)/R]*REF
// polarity: lock polarity
// mux: what is sent to mux pin - SC
void ADF4107::initialize(int P, int B, int A, int R, int pol, int mux){
  int preg;   // prescalar register value
  // Have added breaks in for all cases as preg was defaulting to 0 every time - SC
  switch (P){
    case 8:
      preg = 0;
	  break;
    case 16:
      preg = 1;
	  break;
    case 32:
      preg = 2;
		break;
    case 64:
      preg = 3;
		break;
    default:
      preg = 0;
		break;
  }
  // construct the function latch
  unsigned long func = ((preg << 22) | ADF4107_CPI1(1) | ADF4107_CPI2(1) | (mux << 4));
	if (pol){
		func |= ADF4107_PDPOL_POS;
	}
  // construct the Reference counter latch
  unsigned long r = (ADF4107_REF | ADF4107_ABP(2) | (R << 2));
  // construct the AB counter latch
  unsigned long ab = (ADF4107_AB | ADF4107_CPG(0) | (B << 8) | (A << 2));

  ADF4107::writeRegisterData((ADF4107_INIT | func));
  ADF4107::writeRegisterData((ADF4107_FUNC | func));
  ADF4107::writeRegisterData(r);
  ADF4107::writeRegisterData(ab);

  _p = P;
  _b = B;
  _a = A;
  _r = R;
  _mux = mux;
}

// update
// R: reference divider. Accepts integer between 1 and 16383 inclusive (14 bit)
// P: Prescalar. must be 8, 16, 32, or 64. Additionally, (RF/prescalar) must be less than 300 MHz.
// A: A counter; accepts integer 0-63 (6 bit)
// B: B counter; accepts integer 3-8191 (13 bit). B cannot take values 0, 1, or 2.
// The final multiplier is RF = [(P*B + A)/R]*REF
void ADF4107::update(int P, int B, int A, int R, int pol){
  int preg;   // prescalar register value
	// Have added breaks in for all cases as preg was defaulting to 0 every time - SC
  switch (P){
    case 8:
      preg = 0;
		break;
    case 16:
      preg = 1;
		break;
    case 32:
      preg = 2;
		break;
    case 64:
      preg = 3;
		break;
    default:
      preg = 0;
	  break;
  }
  // construct the function latch
  unsigned long func = ((preg << 22) | ADF4107_CPI1(1) | ADF4107_CPI2(1) | (_mux << 4));
  if (pol){
    func |= ADF4107_PDPOL_POS;
  }
  // construct the Reference counter latch
  unsigned long r = (ADF4107_REF | ADF4107_ABP(2) | (R << 2));
  // construct the AB counter latch
  unsigned long ab = (ADF4107_AB | ADF4107_CPG(0) | (B << 8) | (A << 2));

  //ADF4107::writeRegisterData((ADF4107_INIT | func));
  ADF4107::writeRegisterData((ADF4107_FUNC | func));
  ADF4107::writeRegisterData(r);
  ADF4107::writeRegisterData(ab);

  _p = P;
  _b = B;
  _a = A;
  _r = R;
}

// Writes latch to particular register over SPI.
// data: three-byte number containing register data, MSB-formatted.
void ADF4107::writeRegisterData(unsigned long data){
  digitalWrite(_ssPin, LOW);
  // Writes the data
  for(int i = 2; i >= 0 ; i--){
    SPI.transfer(lowByte(data >> 8*i));
  }
  digitalWrite(_ssPin, HIGH);
}
