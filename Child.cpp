#include "Common.hpp"

namespace GUI::Framework
{
    CChild::CChild() = default;

    void CChild::Render(float parentX, float parentY)
    {
        if (!GUI::m_pRenderer) return;

        const float absX = parentX + m_x;
        const float absY = parentY + m_y;

        GUI::m_pRenderer->DrawRectFilled(absX, absY, m_width, m_height, m_color);

        GUI::m_pRenderer->DrawRectFilled(absX, absY, m_width, m_titleBarHeight, m_titleBarColor);

        if (!m_title.empty())
        {
            GUI::m_pRenderer->DrawText(
                m_title,
                absX + 5.0f,
                absY + (m_titleBarHeight - 15.0f) / 2,
                m_width - 10.0f,
                15.0f,
                m_titleTextColor
            );
        }

        float borderColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
        GUI::m_pRenderer->DrawRect(absX, absY, m_width, m_height, borderColor);

        for (auto& child : m_children)
        {
            child->Render(absX, absY); 
        }
    }
    void CChild::SetPosition(float x, float y)
    {
        m_x = x;
        m_y = y;
    }

    void CChild::SetSize(float width, float height)
    {
        m_width = width;
        m_height = height;
        UpdateChildLayout(); 
    }

    void CChild::SetColor(const float color[4])
    {
        memcpy(m_color, color, sizeof(float) * 4);
    }

    void CChild::SetTitle(const std::wstring& title)
    {
        m_title = title;
    }

    void CChild::SetTitleBarHeight(float height)
    {
        m_titleBarHeight = std::clamp(height, 15.0f, m_height - 10.0f);
    }

    void CChild::SetTitleBarColor(const float color[4])
    {
        memcpy(m_titleBarColor, color, sizeof(float) * 4);
    }

    void CChild::SetTitleTextColor(const float color[4])
    {
        memcpy(m_titleTextColor, color, sizeof(float) * 4);
    }

    void CChild::AddChild(std::unique_ptr<CChild> child)
    {
        m_children.emplace_back(std::move(child));
        UpdateChildLayout();
    }

    void CChild::UpdateChildLayout()
    {
        const float contentHeight = m_height - m_titleBarHeight - 2 * m_childPadding;
        const float contentWidth = m_width - 2 * m_childPadding;
        const size_t childCount = m_children.size();

        if (childCount == 0) return;

        const float startY = m_titleBarHeight + m_childPadding;

        if (childCount == 1)
        {
            auto& child = m_children[0];
            child->SetPosition(m_childPadding, startY);
            child->SetSize(contentWidth, contentHeight);
        }
        else
        {
            const float totalGaps = (childCount - 1) * m_childGap;
            const float childWidth = (contentWidth - totalGaps) / childCount;

            float currentX = m_childPadding;
            for (auto& child : m_children)
            {
                child->SetPosition(currentX, startY);
                child->SetSize(childWidth, contentHeight);
                currentX += childWidth + m_childGap;
            }
        }
    }

    const std::vector<std::unique_ptr<CChild>>& CChild::GetChildren() const
    {
        return m_children;
    }

    void CForm::AddChild(std::unique_ptr<CChild> child)
    {
        m_children.emplace_back(std::move(child));
        UpdateLayout();
    }
}