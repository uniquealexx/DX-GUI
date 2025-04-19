#include "Common.hpp"

namespace GUI::Framework
{
    std::unique_ptr<CForm> m_pForm;

    CForm::CForm() : m_x(0), m_y(0), m_width(200), m_height(150) {}

    CForm::~CForm() {}

    void CForm::Render()
    {
        if (!GUI::m_pRenderer) return;

        float bgColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        GUI::m_pRenderer->DrawRectFilled(m_x, m_y, m_width, m_height, bgColor);

        float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GUI::m_pRenderer->DrawRect(m_x, m_y, m_width, m_height, borderColor);

        float textColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GUI::m_pRenderer->DrawText(L"meme kekw", m_x + 5, m_y + 5, m_width, m_height, borderColor);
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
}