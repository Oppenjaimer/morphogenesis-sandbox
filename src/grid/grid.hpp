#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

template <typename T>
class Grid {
public:
    Grid() = default;

    explicit Grid(size_t width, size_t height, const T& default_value = {})
        : m_width(width), m_height(height), m_data(width * height, default_value) {}

    void fill(const T& value) {
        std::fill(m_data.begin(), m_data.end(), value);
    }

    void resize(size_t width, size_t height, const T& default_value = T{}) {
        m_width = width;
        m_height = height;
        m_data.assign(width * height, default_value);
    }

    size_t index(size_t x, size_t y) const {
        assert(in_bounds(x, y) && "Grid coordinates out of bounds");
        return y * m_width + x;
    }

    bool in_bounds(size_t x, size_t y) const {
        return x < m_width && y < m_height;
    }

    // Toroidal wrap-around for a single coordinate
    size_t wrap(int val, size_t max_val) const {
        assert(max_val > 0 && "Cannot wrap around empty grid dimension");

        int m = static_cast<int>(max_val);
        val %= m;

        return static_cast<size_t>(val < 0 ? val + m : val);
    }

    // Safe 2D access (bounds wrap-around)
    T& at(int x, int y) { return m_data[index(wrap(x, m_width), wrap(y, m_height))]; }
    const T& at(int x, int y) const { return m_data[index(wrap(x, m_width), wrap(y, m_height))]; }

    // Fast 2D access (no bounds checking)
    T& operator()(size_t x, size_t y) { return m_data[index(x, y)]; }
    const T& operator()(size_t x, size_t y) const { return m_data[index(x, y)]; }

    // Direct data access
    T* data() { return m_data.data(); }
    const T* data() const { return m_data.data(); }

    size_t width() const { return m_width; }
    size_t height() const { return m_height; }
    size_t size() const { return m_data.size(); }
    bool empty() const { return m_data.empty(); }

private:
    size_t m_width = 0;
    size_t m_height = 0;
    std::vector<T> m_data;
};
