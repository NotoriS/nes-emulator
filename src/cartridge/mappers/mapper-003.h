#pragma once

#include <cstdint>

#include "../../debug/logger.h"
#include "mapper.h"

class Mapper003 : public Mapper
{
	uint8_t m_dynamicBank;

public:
	Mapper003(std::vector<uint8_t>& prgRom, std::vector<uint8_t>& chrRom);

	uint8_t CpuRead(uint16_t address) override;
	void CpuWrite(uint16_t address, uint8_t data) override;

	uint8_t PpuRead(uint16_t address) override;
	void PpuWrite(uint16_t address, uint8_t data) override;
};