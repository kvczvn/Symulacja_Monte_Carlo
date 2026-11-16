#pragma once
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace mcx {

// Szablon klasy: bufor cykliczny o stałej pojemności.
// Trzyma ostatnie N cen (historia notowań) bez rosnącej pamięci.
// Indeks 0 = najstarszy element, size()-1 = najnowszy.
template <typename T>
class RingBuffer {
public:
    explicit RingBuffer(std::size_t capacity) : data_(capacity), capacity_(capacity) {
        if (capacity == 0) throw std::invalid_argument("RingBuffer: capacity must be > 0");
    }

    void push(const T& value) {
        if (size_ < capacity_) {
            data_[(head_ + size_) % capacity_] = value;
            ++size_;
        } else {
            data_[head_] = value;              // nadpisz najstarszy
            head_ = (head_ + 1) % capacity_;
        }
    }

    const T& operator[](std::size_t i) const { return data_[(head_ + i) % capacity_]; }

    const T& at(std::size_t i) const {
        if (i >= size_) throw std::out_of_range("RingBuffer::at");
        return (*this)[i];
    }

    T& back() {
        if (size_ == 0) throw std::out_of_range("RingBuffer::back on empty buffer");
        return data_[(head_ + size_ - 1) % capacity_];
    }
    const T& back() const {
        if (size_ == 0) throw std::out_of_range("RingBuffer::back on empty buffer");
        return data_[(head_ + size_ - 1) % capacity_];
    }
    const T& front() const {
        if (size_ == 0) throw std::out_of_range("RingBuffer::front on empty buffer");
        return data_[head_];
    }

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }

    std::vector<T> toVector() const {
        std::vector<T> out;
        out.reserve(size_);
        for (std::size_t i = 0; i < size_; ++i) out.push_back((*this)[i]);
        return out;
    }

private:
    std::vector<T> data_;
    std::size_t capacity_;
    std::size_t head_ = 0;
    std::size_t size_ = 0;
};

}  // namespace mcx
