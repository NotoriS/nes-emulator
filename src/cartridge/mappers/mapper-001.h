#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "../../debug/logger.h"
#include "mapper.h"
#include "../mirror-mode.h"

class Mapper001 : public Mapper
{
	uint8_t m_loadReg = 0b10000;
	uint8_t m_controlReg = 0;
	uint8_t m_chrBankZeroReg = 0;
	uint8_t m_chrBankOneReg = 0;
	uint8_t m_prgBankReg = 0;

	std::vector<uint8_t> m_prgRam;

public:
	Mapper001(std::vector<uint8_t>& prgRom, std::vector<uint8_t>& chrRom);

	uint8_t CpuRead(uint16_t address) override;
	void CpuWrite(uint16_t address, uint8_t data) override;

	uint8_t PpuRead(uint16_t address) override;
	void PpuWrite(uint16_t address, uint8_t data) override;

	virtual std::optional<MirrorMode> GetMirrorMode() override;
};