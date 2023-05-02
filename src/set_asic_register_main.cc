#include <iostream>
#include <iomanip>
#include <ios>
#include <cstdio>

#include "RegisterMap.hh"
#include "network.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"
#include "rbcp.hh"

enum kArgIndex{kBin, kIp};

using namespace HUL;

uint32_t GetAddrOffset(int32_t channel_num)
{
  const int32_t kShiftOffset = 20;
  return static_cast<uint32_t>(channel_num << kShiftOffset);
}

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "Please write how to use here" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip = argv[kIp];

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::UDPRBCP::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);


  uint8_t asic_registers[] =
    {
      0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9 // Registers for ASIC
    };
  uint32_t register_size = sizeof(asic_registers)/sizeof(uint8_t);

  std::cout << "#D: Register size: " << register_size << std::endl;


  // Example to send register from all DTL channel on Slot-U
  const int32_t kNumAsic = 2;
  uint32_t txchip_command[] =
    {
      0x00, 0x20
      // 1st byte. This data contais the command to the TX chip on ASAGI card.
      // 0x00: Send register to ASIC0
      // 0x20: Send register to ASIC1
    };

  for(int32_t index_asic = 0; index_asic<kNumAsic; ++index_asic){
    std::cout << "#D: Cycle for ASIC" << index_asic << std::endl;

    const int32_t kNumDtlChannel = 32;
    for(int32_t i = 0; i<kNumDtlChannel; ++i){
      // Set TX chip command
      fpga_module.WriteModule(SctDriver::SCTDU::kAddrWriteFifo + GetAddrOffset(i),
			      txchip_command[index_asic]);

      // Write registers to FIFOs
      fpga_module.WriteModule_nByte(SctDriver::SCTDU::kAddrWriteFifo + GetAddrOffset(i),
				    asic_registers,
				    register_size
				    );
    }
    std::cout << "#D: Registers are written into the FIFOs" << std::endl;

    // Send StartCycle signal to the register transfer sequencer (all the channel).
    fpga_module.WriteModule(SctDriver::SCTDU::kAddrStartCycle, 0x1F, 4);
    std::cout << "#D: StartCycle signals are sent" << std::endl;

    // Check busy flags. If the read value is not zero, the sequencer is still running.
    while(0 != fpga_module.ReadModule(SctDriver::SCTDU::kAddrBusyFlag,  4)){
      std::cout << "#D: Sequencer is still running" << std::endl;
      sleep(1);
    };
    
  }

  std::cout << "#D: Everything done" << std::endl;
  
  return 0;

}// main
