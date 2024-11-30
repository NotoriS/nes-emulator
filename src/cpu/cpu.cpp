#include "cpu.h"

CPU::CPU()
{
}

CPU::~CPU()
{
}

void CPU::Clock()
{
    if (m_microInstructionQueue.empty())
    {
        QueueNextInstuction();
    }
    else
    {
        m_microInstructionQueue.front()();
        m_microInstructionQueue.pop();
    }
}

void CPU::QueueNextInstuction()
{
}
