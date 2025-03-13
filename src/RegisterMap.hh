#ifndef REGISTER_HH
#define REGISTER_HH

#include<stdint.h>
#include"RegisterMapCommon.hh"

namespace HUL{
namespace SctDriver{
// SctDriver (SCTD)  -----------------------------------------------------------------
namespace SCTDU{
  enum LocalAddress
    {
      kAddrWriteFifo  = 0x0000'0000, // W, [7:0]
                                     // Adddress range is 0x0000'0000-0x01F0'0000. It corresponds to 0-31ch

      kAddrBusyFlag   = 0x0200'0000, // R, [31:0] If the bit is high, the register transfer sequence is running.
                                     // Bit position corresponds to the channel.
      
      kAddrStartCycle = 0x0300'0000, // W, [31:0] Start the register transfer cycle.
                                     // Bit position corresponds to the channel.
      kAddrStateCom   = 0x0400'0000  // W, [31:0] Change SctDriver state 0: idle, 1: communication
     
    };
};

namespace SCTDD{
  enum LocalAddress
    {
      kAddrWriteFifo  = 0x1000'0000, // W, [7:0]
                                     // Adddress range is 0x0000'0000-0x01F0'0000. It corresponds to 0-31ch
      kAddrBusyFlag   = 0x1200'0000, // R, [31:0] If the bit is high, the register transfer sequence is running.
                                     // Bit position corresponds to the channel.
      
      kAddrStartCycle = 0x1300'0000, // W, [31:0] Start the register transfer cycle.
                                     // Bit position corresponds to the channel.
      kAddrStateCom   = 0x1400'0000  // W, [31:0] Change SctDriver state 0: idle, 1: communication
     
    };
};  
};

};

#endif
