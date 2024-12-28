#pragma once

#include "../interfaces/i-bus.h"
#include "../cartridge/cartridge.h"

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

private:
	std::array<uint8_t, 2048> m_memory;

	std::shared_ptr<Cartridge> m_cartridge;
};