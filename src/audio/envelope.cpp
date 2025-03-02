#include "envelope.h"

void Envelope::Clock()
{
    if (m_start)
    {
        m_start = false;
        m_decayLevel = 15;
        m_divider = m_dividerReload;
    }
    else
    {
        ClockDivider();
    }
}

void Envelope::ClockDivider()
{
    if (m_divider != 0)
    {
        m_divider--;
        return;
    }

    m_divider = m_dividerReload;
    ClockDecayLevel();
}

void Envelope::ClockDecayLevel()
{
    if (m_decayLevel != 0)
    {
        m_decayLevel--;
        return;
    }

    if (m_loop) m_decayLevel = 15;
}
