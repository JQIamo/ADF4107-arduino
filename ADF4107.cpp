/* 
   ADF4107.cpp - ADF4107 PLL Communication Library
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
   aunsigned long with this program.  If not, see <http://www.gnu.org/licenses/>.


 */
 
 //MSB FIRST!!
 
#include "Arduino.h"
#include "SPI.h"
#include <ADF4350.h>


ADF4107::ADF4107(byte ssPin) {
    _ssPin = ssPin;
    pinMode(_ssPin, OUTPUT);
    digitalWrite(_ssPin, HIGH);
}



/*

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

#define ADF4107_REF = 0
#define ADF4107_AB = 1
#define ADF4107_FUNC = 2
#define ADF4107_INIT = 3
*/





ADF4107::initialize(){
    unsigned long func =  ADF4107_PRESCALAR(0) | ADF4107_CPI1(1) | ADF4107_CPI2(1) | ADF4107_PDPOL_POS;
    unsigned long r = ADF4107_REF | ADF4107_ABP(2) | (1 << 2);
    unsigned long ab = ADF4107_AB | ADF4107_CPG(0) | (1 << 8) | (1 << 2);
    
    byte funcReg[3];
    byte initReg[3];
    byte rReg[3];
    byte abReg[3];
    
    ADF4107::makeByteArray((ADF4107_INIT | func), initReg);
    ADF4107::makeByteArray((ADF4107_FUNC | func), funcReg);
    ADF4107::makeByteArray(r,rReg);
    ADF4107::makeByteArray(ab, abReg);
    
    ADF4107::writeRegister(funcReg);
    ADF4107::writeRegister(funcReg);
    
}



// takes the latch data, and converts to byteArray.
byte ADF4107::makeByteArray(unsigned long latchData, byte* buf[]){
    buf = { lowByte(latchData >> 16), lowByte(latchData >> 8), lowByte(latchData) };
}


// Writes SPI to particular register.
// data[] is a three-byte array.
void ADF4107::writeRegister(byte data[]){


    digitalWrite(_ssPin, LOW);

    // Writes the data
    for(int i = 0; i < 3 ; i++){
        SPI.transfer(data[i]);
    }

    digitalWrite(_ssPin, HIGH);

}