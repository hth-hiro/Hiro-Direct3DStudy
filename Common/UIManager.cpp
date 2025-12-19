#include "pch.h"
#include "UIManager.h"
#include "UIRenderer.h"

#include <algorithm>

UIManager::UIManager()
{
}

UIManager::~UIManager()
{
    Shutdown();
}

void UIManager::Shutdown()
{
    m_uiList.clear();
}

void UIManager::AddUI(UIBase* ui)
{
    if (!ui)
        return;
    m_uiList.push_back(ui);
    SortByZOrder();
}

void UIManager::RemoveUI(UIBase* ui)
{
    auto it = std::find(m_uiList.begin(), m_uiList.end(), ui);
    if (it != m_uiList.end())
        m_uiList.erase(it);
}

void UIManager::Update(float deltaTime)
{
    for (UIBase* ui : m_uiList)
    {
        if (ui && ui->IsActive())
            ui->Update(deltaTime);
    }
}

void UIManager::Render()
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

// 추후 추가하기
void UIManager::ProcessMouseDown(int x, int y)
{
}

void UIManager::ProcessMouseUp(int x, int y)
{
}

void UIManager::ProcessMouseMove(int x, int y)
{
}

void UIManager::SortByZOrder()
{
    // 같은 zOrder끼리는 추가된 순서로 유지한다.
    std::stable_sort(m_uiList.begin(), m_uiList.end(),
        [](const UIBase* a, const UIBase* b)
        {
            return a->GetZOrder() < b->GetZOrder();
        });
}
