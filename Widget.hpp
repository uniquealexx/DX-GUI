#pragma once

namespace GUI::Framework
{
    class IWidget
    {
    public:
        virtual ~IWidget() = default;

        virtual void Render(float parentX, float parentY) = 0;
        virtual void SetPosition(float x, float y) = 0;
        virtual void SetSize(float width, float height) = 0;

        virtual float GetX() const = 0;
        virtual float GetY() const = 0;
        virtual float GetWidth() const = 0;
        virtual float GetHeight() const = 0;

        virtual bool IsHovered(float mouseX, float mouseY) const = 0;
        virtual bool HandleMouseClick(float mouseX, float mouseY) = 0;
    };

    using WidgetCallback = std::function<void()>;

    class IWidgets
    {
    public:
        virtual ~IWidgets() = default;
        virtual void AddWidget(std::unique_ptr<IWidget> widget) = 0;
    };
}