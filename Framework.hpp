#pragma once

namespace GUI
{
    class CFramework
    {
    public:
        CFramework();
        ~CFramework();

        void Initialize();
        void Render();
    };

    extern std::unique_ptr<CFramework> m_pFramework;
}