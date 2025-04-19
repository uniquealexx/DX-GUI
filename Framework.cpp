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

            auto grandparent = std::make_unique<Framework::CChild>();
            auto parent = std::make_unique<Framework::CChild>();
            auto child = std::make_unique<Framework::CChild>();
            auto child2 = std::make_unique<Framework::CChild>();

            grandparent->SetTitle(L"keke");
            parent->SetTitle(L"meme");
            child->SetTitle(L"lol");
            child2->SetTitle(L"xddd");

            parent->AddChild(std::move(child));
            parent->AddChild(std::move(child2));
            grandparent->AddChild(std::move(parent));
            Framework::m_pForm->AddChild(std::move(grandparent));
        }
    }

    void CFramework::Render()
    {
        if (Framework::m_pForm)
            Framework::m_pForm->Render();
    }
}