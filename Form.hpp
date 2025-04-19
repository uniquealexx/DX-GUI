#pragma once

namespace GUI::Framework
{
    class CForm
    {
    public:
        CForm();
        ~CForm();

        void Render();
        void SetPosition(float x, float y);
        void SetSize(float width, float height);
        void UpdateMouseInput(float mouseX, float mouseY, bool isDragging);
        void SetTitle(const std::wstring& title);

        bool IsHovered(float mouseX, float mouseY) const;
        bool IsDragging() const { return m_bDragging; }

        float GetX() const { return m_x; }
        float GetY() const { return m_y; }
        float GetWidth() const { return m_width; }
        float GetHeight() const { return m_height; }
        const std::wstring& GetTitle() const { return m_title; }

        void AddChild(std::unique_ptr<CChild> child);
        const std::vector<std::unique_ptr<CChild>>& GetChildren() const { return m_children; }

        void UpdateLayout();

    private:
        std::vector<std::unique_ptr<CChild>> m_children;

        std::wstring m_title = L"New Form";
        float m_titleBarHeight = 25.0f;
        DWRITE_FONT_WEIGHT m_titleFontWeight = DWRITE_FONT_WEIGHT_BOLD;

        float m_x, m_y;
        float m_width, m_height;
        bool m_bDragging = false;
        float m_dragOffsetX = 0;
        float m_dragOffsetY = 0;
        float m_padding = 5.0f;    
        float m_gap = 5.0f;        
        float m_childHeight = 30.0f;
    };

    extern std::unique_ptr<CForm> m_pForm;
}