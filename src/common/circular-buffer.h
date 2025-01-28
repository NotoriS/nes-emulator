#pragma once

#include <array>
#include <stdexcept>

template<typename T, size_t capacity>
class CircularBuffer
{
    std::array<T, capacity> m_buffer;
    size_t m_size;
    size_t m_head;
    size_t m_tail;

public:
    CircularBuffer() : m_size(0), m_head(0), m_tail(0) {}

    void push_front(T item)
    {
        if (full()) return;
        m_buffer[m_head] = item;
        m_head = (m_head + 1) % capacity;
        m_size++;
    }

    void pop_front()
    {
        if (empty()) return;
        m_head = (m_head - 1) % capacity;
        m_size--;
    }

    T front() const
    {
        if (empty()) throw std::runtime_error("Cannot return the front of an empty buffer");
        return m_buffer[(m_head - 1) % capacity];
    }

    void push_back(T item)
    {
        if (full()) return;
        m_tail = (m_tail - 1) % capacity;
        m_buffer[m_tail] = item;
        m_size++;
    }

    void pop_back()
    {
        if (empty()) return;
        m_tail = (m_tail + 1) % capacity;
        m_size--;
    }

    T back() const
    {
        if (empty()) throw std::runtime_error("Cannot return the back of an empty buffer");
        return m_buffer[m_tail];
    }

    inline bool empty() const
    {
        return m_size == 0;
    }

    inline bool full() const
    {
        return m_size == capacity;
    }

    inline size_t size() const
    {
        return m_size;
    }
};