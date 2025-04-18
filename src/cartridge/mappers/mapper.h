#pragma once

#include <cstdint>
#include <vector>
#include <optional>

#include "../mirror-mode.h"

class Mapper
{
public:
	Mapper(std::vector<uint8_t>& prgRom, std::vector<uint8_t>& chrRom) : m_prgRom(prgRom), m_chrRom(chrRom) {}
	virtual ~Mapper() {}

	virtual uint8_t CpuRead(uint16_t address) = 0;
	virtual void CpuWrite(uint16_t address, uint8_t data) = 0;

	virtual uint8_t PpuRead(uint16_t address) = 0;
	virtual void PpuWrite(uint16_t address, uint8_t data) = 0;

	virtual std::optional<MirrorMode> GetMirrorMode() { return {}; }

	virtual bool PollIrqInterrupt() { return false; }

protected:
	std::vector<uint8_t>& m_prgRom;  // PRG ROM data
	std::vector<uint8_t>& m_chrRom;  // CHR ROM data
};