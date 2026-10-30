#pragma once

#include "grid/grid.hpp"

#include <raylib.h>

#include <cmath>
#include <concepts>
#include <vector>

class GridRenderer {
public:
    explicit GridRenderer(size_t width, size_t height)
        : m_width(width), m_height(height), m_pixel_buffer(width * height)
    {
        init_texture(width, height);
    }

    ~GridRenderer() {
        UnloadTexture(m_texture);
    }

    // Remove copy semantics to avoid double-freeing the texture
    GridRenderer(const GridRenderer&) = delete;
    GridRenderer& operator=(const GridRenderer&) = delete;

    // Requires function to calculate pixel color depending on grid state
    template <typename T, typename Func>
    requires requires (Func f, T v) {
        { f(v) } -> std::convertible_to<Color>;
    }
    void update(const Grid<T>& grid, Func pixel_color) {
        if (grid.empty()) return;

        // Auto-resize pixel buffer if grid dimensions changed
        if (grid.width() != m_width || grid.height() != m_height) {
            UnloadTexture(m_texture);
            init_texture(grid.width(), grid.height());
        }

        // Map grid state to pixel buffer
        for (size_t y = 0; y < grid.height(); ++y) {
            for (size_t x = 0; x < grid.width(); ++x) {
                m_pixel_buffer[grid.index(x, y)] = pixel_color(grid(x, y));
            }
        }

        UpdateTexture(m_texture, m_pixel_buffer.data());
    }

    void render() const {
        auto [scale, offset_x, offset_y] = compute_transform();

        const float dest_width = m_texture.width * scale;
        const float dest_height = m_texture.height * scale;

        const Rectangle source = {0.0f, 0.0f, static_cast<float>(m_texture.width), static_cast<float>(m_texture.height)};
        const Rectangle dest = {offset_x, offset_y, dest_width, dest_height};
        const Vector2 origin = {0.0f, 0.0f};

        DrawTexturePro(m_texture, source, dest, origin, 0.0f, WHITE);
    }

    // Map screen mouse coords to grid indices, returns whether mouse is within grid bounds
    bool screen_to_grid(int mouse_x, int mouse_y, size_t& grid_x, size_t& grid_y) const {
        auto [scale, offset_x, offset_y] = compute_transform();

        const float rel_x = static_cast<float>(mouse_x) - offset_x;
        const float rel_y = static_cast<float>(mouse_y) - offset_y;

        // Reject mouse coords in empty margins
        if (rel_x < 0 || rel_y < 0 || rel_x >= m_width * scale || rel_y >= m_height * scale)
            return false;

        grid_x = static_cast<size_t>(rel_x / scale);
        grid_y = static_cast<size_t>(rel_y / scale);

        return true;
    }

private:
    // Texture screen scaling values
    struct Transform {
        float scale;
        float offset_x;
        float offset_y;
    };

    Transform compute_transform() const {
        // Calculate scale to fit window maintaining aspect ratio
        const float scale_x = static_cast<float>(GetScreenWidth()) / m_texture.width;
        const float scale_y = static_cast<float>(GetScreenHeight()) / m_texture.height;
        const float scale = std::max(1.0f, std::floor(std::min(scale_x, scale_y)));

        // Center texture
        const float dest_width = m_texture.width * scale;
        const float dest_height = m_texture.height * scale;
        const float offset_x = std::floor((GetScreenWidth() - dest_width) * 0.5f);
        const float offset_y = std::floor((GetScreenHeight() - dest_height) * 0.5f);

        return {scale, offset_x, offset_y};
    }

    void init_texture(size_t width, size_t height) {
        m_width = width;
        m_height = height;
        m_pixel_buffer.resize(width * height);

        Image image = GenImageColor(width, height, BLANK);
        m_texture = LoadTextureFromImage(image);
        UnloadImage(image);
        SetTextureFilter(m_texture, TEXTURE_FILTER_POINT);
    }

    size_t m_width = 0;
    size_t m_height = 0;

    std::vector<Color> m_pixel_buffer;
    Texture m_texture;
};
