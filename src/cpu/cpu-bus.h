#include <cstdint>
#include <array>

#include "../interfaces/i-bus.h"

class CpuBus : public IBus
{
public:
	CpuBus();
	~CpuBus() override;

private:
	std::array<uint8_t, 2048> m_memory;

	uint8_t Read(uint16_t address) override;
	void Write(uint16_t address, uint8_t data) override;
};