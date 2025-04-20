#include "Common.hpp"

namespace GUI::Framework
{
    std::unique_ptr<CForm> m_pForm;

    CForm::CForm() : m_x(0), m_y(0), m_width(200), m_height(150) {}

    CForm::~CForm() {}

    void CForm::Render()
    {
        if (!GUI::m_pRenderer) return;

        float bgColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GUI::m_pRenderer->DrawRectFilled(m_x, m_y, m_width, m_height, bgColor);

        float titleBarColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GUI::m_pRenderer->DrawRectFilled(m_x, m_y, m_width, m_titleBarHeight, titleBarColor);

        float textColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GUI::m_pRenderer->DrawText(
            m_title,
            m_x + 5.0f,
            m_y + (m_titleBarHeight - 15.0f) / 2,
            m_width - 10.0f,
            15.0f,
            textColor
        );

        float borderColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
        GUI::m_pRenderer->DrawRect(m_x, m_y, m_width, m_height, borderColor);

        UpdateLayout();

        // Render child components first
        for (auto& child : m_children)
        {
            child->Render(m_x, m_y);
        }

        // Render widgets
        for (auto& widget : m_widgets)
        {
            widget->Render(m_x, m_y);
        }
    }

    // Implement IWidgets interface
    void CForm::AddWidget(std::unique_ptr<IWidget> widget)
    {
        if (widget) {
            m_widgets.emplace_back(std::move(widget));
        }
    }

    bool CForm::HandleWidgetEvents(float mouseX, float mouseY, bool isMouseDown)
    {
        float localMouseX = mouseX - m_x;
        float localMouseY = mouseY - m_y;

        if (localMouseX < 0 || localMouseX > m_width ||
            localMouseY < 0 || localMouseY > m_height)
        {
            return false;
        }

        for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it)
        {
            auto& widget = *it;
            if (widget->IsHovered(localMouseX, localMouseY))
            {
                if (isMouseDown)
                {
                    if (widget->HandleMouseClick(localMouseX, localMouseY))
                    {
                        return true;
                    }
                }
                return true;
            }
        }

        for (auto& child : m_children)
        {
            if (child->HandleWidgetEvents(localMouseX, localMouseY, isMouseDown))
            {
                return true;
            }
        }

        return false;
    }

    void CForm::SetTitle(const std::wstring& title)
    {
        m_title = title;
    }

    void CForm::SetPosition(float x, float y)
    {
        m_x = x;
        m_y = y;
    }

    void CForm::SetSize(float width, float height)
    {
        m_width = width;
        m_height = height;

        UpdateLayout();
    }

    bool CForm::IsHovered(float mouseX, float mouseY) const
    {
        return mouseX >= m_x && mouseX <= (m_x + m_width) &&
            mouseY >= m_y && mouseY <= (m_y + m_height);
    }

    void CForm::UpdateMouseInput(float mouseX, float mouseY, bool isDragging)
    {
        if (isDragging && m_bDragging)
        {
            m_x = mouseX - m_dragOffsetX;
            m_y = mouseY - m_dragOffsetY;
        }
        else if (IsHovered(mouseX, mouseY) && !m_bDragging)
        {
            m_bDragging = true;
            m_dragOffsetX = mouseX - m_x;
            m_dragOffsetY = mouseY - m_y;
        }
        else
        {
            m_bDragging = false;
        }
    }

    void CForm::UpdateLayout()
    {
        const size_t childCount = m_children.size();
        if (childCount == 0) return;

        const float availableWidth = m_width - 2 * m_padding;
        const float availableHeight = m_height - m_titleBarHeight - 2 * m_padding;
        const float startY = m_titleBarHeight + m_padding;

        if (childCount == 1)
        {
            auto& child = m_children[0];
            child->SetPosition(m_padding, startY);
            child->SetSize(availableWidth, availableHeight);
        }
        else
        {
            const float totalGaps = (childCount - 1) * m_gap;
            const float childWidth = (availableWidth - totalGaps) / childCount;

            float currentX = m_padding;
            for (auto& child : m_children)
            {
                child->SetPosition(currentX, startY);
                child->SetSize(childWidth, availableHeight);
                currentX += childWidth + m_gap;
            }
        }
    }
}

