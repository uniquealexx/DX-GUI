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
            Framework::m_pForm->SetSize(300, 200);
            Framework::m_pForm->SetTitle(L"Main Window");
        }
    }

    void CFramework::Render()
    {
        if (Framework::m_pForm)
            Framework::m_pForm->Render();
    }
}