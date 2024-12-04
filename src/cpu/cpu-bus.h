#pragma once

#include <cstdint>
#include <array>

#include "../interfaces/i-bus.h"
#include "../cartridge/cartridge.h"

class CpuBus : public IBus
{
public:
	CpuBus();
	~CpuBus() override;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	void ConnectCartridge(Cartridge* cartridge) { m_cartridge = cartridge; }

private:
	std::array<uint8_t, 2048> m_memory;

	Cartridge* m_cartridge;
};