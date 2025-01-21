#pragma once

#include "../interfaces/i-bus.h"
#include "../cartridge/cartridge.h"

#include <array>
#include <cstdint>

class PpuBus : public IBus
{
public:
	PpuBus();
	~PpuBus() override;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	void ConnectCartridge(std::shared_ptr<Cartridge> cartridge) { m_cartridge = cartridge; }
private:
	std::shared_ptr<Cartridge> m_cartridge;
	uint8_t m_nameTables[2][0x400];
	std::array<uint8_t, 0x20> m_paletteMemory;
};