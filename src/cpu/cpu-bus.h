#pragma once

#include "../debug/logger.h"
#include "../interfaces/i-bus.h"
#include "../cartridge/cartridge.h"
#include "../ppu/ppu.h"

#include <cstdint>
#include <array>
#include <memory>
#include <iostream>
#include <iomanip>

class CpuBus : public IBus
{
public:
	CpuBus();
	~CpuBus() override;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	void ConnectCartridge(std::shared_ptr<Cartridge> cartridge) { m_cartridge = cartridge; }
	void ConnectPPU(std::shared_ptr<PPU> ppu) { m_ppu = ppu; }

private:
	std::array<uint8_t, 2048> m_memory;

	std::shared_ptr<Cartridge> m_cartridge;
	std::shared_ptr<PPU> m_ppu;
};