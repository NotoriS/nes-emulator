#pragma once

#include <iostream>

#include "mapper.h"

class Mapper000 : public Mapper
{
public:
	Mapper000(std::vector<uint8_t>& prgRom, std::vector<uint8_t>& chrRom) : Mapper(prgRom, chrRom) {}

	uint8_t CpuRead(uint16_t address) override;
	void CpuWrite(uint16_t address, uint8_t data) override;

	uint8_t PpuRead(uint16_t address) override;
	void PpuWrite(uint16_t address, uint8_t data) override;
};