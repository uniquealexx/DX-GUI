#include "Common.hpp"

namespace GUI::Framework
{
    std::unique_ptr<CForm> m_pForm;

    CForm::CForm() : m_x(0), m_y(0), m_width(200), m_height(150) {}

    CForm::~CForm() {}

    void CForm::Render()
    {
        if (!GUI::m_pRenderer) return;

        // Отрисовка фона формы
        float bgColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GUI::m_pRenderer->DrawRectFilled(m_x, m_y, m_width, m_height, bgColor);

        // Отрисовка заголовка
        float titleBarColor[4] = { 0.3f, 0.3f, 0.3f, 1.0f };
        GUI::m_pRenderer->DrawRectFilled(m_x, m_y, m_width, m_titleBarHeight, titleBarColor);

        // Отрисовка текста заголовка
        float textColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GUI::m_pRenderer->DrawText(
            m_title,
            m_x + 5.0f, // Отступ слева
            m_y + (m_titleBarHeight - 15.0f) / 2, // Вертикальное выравнивание
            m_width - 10.0f, // Максимальная ширина
            15.0f,           // Высота текста
            textColor
        );

        // Отрисовка границы
        float borderColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
        GUI::m_pRenderer->DrawRect(m_x, m_y, m_width, m_height, borderColor);
    }

    void CForm::SetTitle(const std::wstring& title)
    {
        m_title = title;
        // Можно добавить обрезку длинного текста при необходимости
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