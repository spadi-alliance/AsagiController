#include <iostream>
#include <fstream>
#include <iomanip>
#include <ios>
#include <cstdio>

#include "RegisterMap.hh"
#include "network.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"
#include "rbcp.hh"

enum kArgIndex{kBin, kIp, kPfile};

using namespace HUL;

uint32_t GetAddrOffset(int32_t channel_num)
{
  const int32_t kShiftOffset = 20;
  return static_cast<uint32_t>(channel_num << kShiftOffset);
}

int main(int argc, char* argv[])
{
  if(1 == argc || 2 == argc){
    std::cout << "Usage\n";
    std::cout << "set_asic_register <IP address> <Parameter File>" << std::endl;
    //std::cout << "Please write how to use here" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip = argv[kIp];

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::UDPRBCP::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  std::ifstream infile(argv[kPfile]);
  std::string reg_val;

  std::vector<int> registers;
  while(std::getline(infile, reg_val)){
    std::cout << "#In:" << reg_val <<std::endl;
    registers.push_back(std::stoi(reg_val, nullptr, 16));
  }

  int reg_size = registers.size();
  uint8_t asic_registers[reg_size];
  for(int i=0; i<reg_size; ++i){
    asic_registers[i] = registers[i];
    std::cout << std::dec << (i-2) <<":#Out:" << std::hex << unsigned(asic_registers[i]) <<std::endl;
  }
  uint32_t register_size = sizeof(asic_registers)/sizeof(uint8_t);
  std::cout << "#D: Register size: " << register_size << std::endl;

  fpga_module.WriteModule(SctDriver::SCTDU::kAddrStateCom, 0x1F, 4);

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

  fpga_module.WriteModule(SctDriver::SCTDU::kAddrStateCom, 0x0, 4);
  std::cout << "#D: Everything done" << std::endl;
  
  return 0;

}// main
