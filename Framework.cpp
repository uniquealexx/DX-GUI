#include "Common.hpp"

namespace GUI
{
    std::unique_ptr<CFramework> m_pFramework;

    CFramework::CFramework()
    {
        Framework::m_pForm = std::make_unique<Framework::CForm>();
    }

    CFramework::~CFramework()
    {
        Framework::m_pForm.reset();
    }

    void CFramework::Initialize()
    {
        if (Framework::m_pForm)
        {
            Framework::m_pForm->SetPosition(50, 50);
            Framework::m_pForm->SetSize(400, 500);
            Framework::m_pForm->SetTitle(L"Main Window");

            auto child = std::make_unique<Framework::CChild>();
            auto child2 = std::make_unique<Framework::CChild>();

            child->SetTitle(L"Child1");
            child2->SetTitle(L"Child2");

            auto button1 = std::make_unique<Framework::Widget::CButton>(L"Click Me");
            button1->SetPosition(10.0f, 30.0f);
            button1->SetSize(80.0f, 25.0f);
            button1->SetCallback([]() {
                OutputDebugStringA("Button clicked!\n");
                });

            child->AddWidget(std::move(button1));

            Framework::m_pForm->AddChild(std::move(child));
            Framework::m_pForm->AddChild(std::move(child2));
        }
    }

    void CFramework::Render()
    {
        if (Framework::m_pForm)
            Framework::m_pForm->Render();
    }
}