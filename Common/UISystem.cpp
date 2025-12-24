#include "pch.h"
#include "UISystem.h"
#include "UIRenderer.h"

#include <algorithm>

UISystem::UISystem()
{
}

UISystem::~UISystem()
{
    Shutdown();
}

void UISystem::Shutdown()
{
    m_uiList.clear();
}

void UISystem::AddUI(UIBase* ui)
{
    if (!ui)
        return;
    m_uiList.push_back(ui);
    SortByZOrder();
}

void UISystem::RemoveUI(UIBase* ui)
{
    auto it = std::find(m_uiList.begin(), m_uiList.end(), ui);
    if (it != m_uiList.end())
        m_uiList.erase(it);
}

void UISystem::Update(float deltaTime)
{
    for (UIBase* ui : m_uiList)
    {
        if (ui && ui->IsActive())
            ui->Update(deltaTime);
    }
}

void UISystem::Render()
{
    UIRenderer::Get().BeginFrame();

    for (UIBase* ui : m_uiList)
    {
        if (ui && ui->IsActive())
        {
            ui->Render();
        }
    }

    UIRenderer::Get().EndFrame();
}

void UISystem::ProcessMouseDown(int x, int y)
{
}

void UISystem::ProcessMouseUp(int x, int y)
{
}

void UISystem::ProcessMouseMove(int x, int y)
{
}

void UISystem::SortByZOrder()
{
    std::stable_sort(m_uiList.begin(), m_uiList.end(),
        [](const UIBase* a, const UIBase* b)
        {
            return a->GetZOrder() < b->GetZOrder();
        }
        );
}
