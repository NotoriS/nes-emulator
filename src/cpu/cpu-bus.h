#pragma once

#include <cstdint>
#include <array>
#include <memory>

#include "../debug/logger.h"
#include "../interfaces/i-bus.h"
#include "../cartridge/cartridge.h"
#include "../ppu/ppu.h"

class CpuBus : public IBus
{
public:
	CpuBus();
	~CpuBus() override;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;

	void ConnectControllers(std::shared_ptr<uint8_t> controllerOneState, std::shared_ptr<uint8_t> controllerTwoState);
	void ConnectCartridge(std::shared_ptr<Cartridge> cartridge) { m_cartridge = cartridge; }
	void ConnectPPU(std::shared_ptr<PPU> ppu) { m_ppu = ppu; }

	bool TryDirectMemoryAccess(bool cycleIsOdd);

private:
	std::array<uint8_t, 2048> m_memory;

	std::shared_ptr<uint8_t> m_controllerOneState;
    std::shared_ptr<uint8_t> m_controllerTwoState;
	std::shared_ptr<Cartridge> m_cartridge;
	std::shared_ptr<PPU> m_ppu;

	uint8_t m_controllerOneShifter = 0x00;
	uint8_t m_controllerTwoShifter = 0x00;

	uint8_t m_dmaPage = 0x00;
	uint8_t m_dmaAddress = 0x00;
	uint8_t m_dmaDataBuffer = 0x00;
	bool m_dmaReady = false;
	bool m_dmaInProgress = false;

	bool m_controllerPollingEnabled = false;

	void PollControllerState();
};