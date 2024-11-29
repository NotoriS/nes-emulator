#pragma once

#include <queue>
#include <functional>

class CPU
{
public:

	CPU();
	~CPU();

	void Clock();

private:

	std::queue<std::function<void()>> m_microInstructionQueue;

	void QueueNextInstuction();
};