#pragma once

namespace GUI::Framework::Widget
{
    class CButton : public IWidget
    {
    public:
        CButton();
        explicit CButton(const std::wstring& text);

        // IWidget implementation
        void Render(float parentX, float parentY) override;
        void SetPosition(float x, float y) override;
        void SetSize(float width, float height) override;

        float GetX() const override { return m_x; }
        float GetY() const override { return m_y; }
        float GetWidth() const override { return m_width; }
        float GetHeight() const override { return m_height; }

        bool IsHovered(float mouseX, float mouseY) const override;
        bool HandleMouseClick(float mouseX, float mouseY) override;

        // Button specific methods
        void SetText(const std::wstring& text);
        void SetCallback(WidgetCallback callback);
        void SetColors(const float normal[4], const float hovered[4], const float pressed[4]);
        void SetTextColor(const float color[4]);

    private:
        float m_x = 0.0f;
        float m_y = 0.0f;
        float m_width = 100.0f;
        float m_height = 30.0f;

        std::wstring m_text;
        WidgetCallback m_callback;

        float m_normalColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
        float m_hoveredColor[4] = { 0.4f, 0.4f, 0.4f, 1.0f };
        float m_pressedColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
        float m_textColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        bool m_isPressed = false;
        bool m_isHovered = false;
    };
}