#include "Common.hpp"

namespace GUI::Framework::Widget
{
    CButton::CButton() : m_text(L"Button") {}

    CButton::CButton(const std::wstring& text) : m_text(text) {}

    void CButton::Render(float parentX, float parentY)
    {
        if (!GUI::m_pRenderer) return;

        const float absX = parentX + m_x;
        const float absY = parentY + m_y;

        // Choose the appropriate color based on button state
        const float* color = m_normalColor;
        if (m_isPressed) {
            color = m_pressedColor;
        }
        else if (m_isHovered) {
            color = m_hoveredColor;
        }

        // Draw button background
        GUI::m_pRenderer->DrawRectFilled(absX, absY, m_width, m_height, color);

        // Draw button border
        float borderColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
        GUI::m_pRenderer->DrawRect(absX, absY, m_width, m_height, borderColor);

        // Draw button text
        if (!m_text.empty()) {
            GUI::m_pRenderer->DrawText(
                m_text,
                absX + 5.0f,
                absY + (m_height - 15.0f) / 2,
                m_width - 10.0f,
                15.0f,
                m_textColor
            );
        }
    }

    void CButton::SetPosition(float x, float y)
    {
        m_x = x;
        m_y = y;
    }

    void CButton::SetSize(float width, float height)
    {
        m_width = width;
        m_height = height;
    }

    bool CButton::IsHovered(float mouseX, float mouseY) const
    {
        return (mouseX >= m_x && mouseX <= m_x + m_width &&
            mouseY >= m_y && mouseY <= m_y + m_height);
    }

    bool CButton::HandleMouseClick(float mouseX, float mouseY)
    {
        if (IsHovered(mouseX, mouseY)) {
            m_isPressed = true;

            // Execute callback if set
            if (m_callback) {
                m_callback();
            }

            return true;
        }

        m_isPressed = false;
        return false;
    }

    void CButton::SetText(const std::wstring& text)
    {
        m_text = text;
    }

    void CButton::SetCallback(WidgetCallback callback)
    {
        m_callback = std::move(callback);
    }

    void CButton::SetColors(const float normal[4], const float hovered[4], const float pressed[4])
    {
        memcpy(m_normalColor, normal, sizeof(float) * 4);
        memcpy(m_hoveredColor, hovered, sizeof(float) * 4);
        memcpy(m_pressedColor, pressed, sizeof(float) * 4);
    }

    void CButton::SetTextColor(const float color[4])
    {
        memcpy(m_textColor, color, sizeof(float) * 4);
    }
}