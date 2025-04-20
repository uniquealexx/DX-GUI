#pragma once

namespace GUI::Framework
{
    class CChild : public IWidgets
    {
    public:
        CChild();

        void Render(float parentX, float parentY);
        void SetPosition(float x, float y);
        void SetSize(float width, float height);
        void SetColor(const float color[4]);
        void SetTitle(const std::wstring& title);
        void SetTitleBarHeight(float height);
        void SetTitleBarColor(const float color[4]);
        void SetTitleTextColor(const float color[4]);

        float GetX() const { return m_x; }
        float GetY() const { return m_y; }
        float GetWidth() const { return m_width; }
        float GetHeight() const { return m_height; }

        void AddChild(std::unique_ptr<CChild> child);
        const std::vector<std::unique_ptr<CChild>>& GetChildren() const;

        void AddWidget(std::unique_ptr<IWidget> widget) override;
        const std::vector<std::unique_ptr<IWidget>>& GetWidgets() const { return m_widgets; }

        bool HandleWidgetEvents(float mouseX, float mouseY, bool isMouseDown);

        void UpdateChildLayout();

    private:
        float m_x = 0.0f;
        float m_y = 0.0f;
        float m_width = 100.0f;
        float m_height = 30.0f;
        float m_color[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

        std::wstring m_title;
        float m_titleBarHeight = 20.0f;
        float m_titleBarColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
        float m_titleTextColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float m_contentHeight = 0.0f;

        std::vector<std::unique_ptr<CChild>> m_children; 
        float m_childPadding = 5.0f;
        float m_childGap = 5.0f;    

        std::vector<std::unique_ptr<IWidget>> m_widgets;

    };
}