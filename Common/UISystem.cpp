#include "pch.h"
#include "UISystem.h"
#include "UIRenderer.h"

#include <algorithm>

UISystem::UISystem()
{
}

UISystem::~UISystem()
{
    //Shutdown();
}

void UISystem::Shutdown()
{
    if (m_isShutdown) return;
    m_isShutdown = true;

    m_hovered = nullptr;
    m_captured = nullptr;

    for (auto* ui : m_roots)
    {
        ui->Shutdown();
        delete ui;
    }

    m_roots.clear();

    UIRenderer::Get().ShutDown();
}

void UISystem::AddUI(UIBase* root)
{
    if (!root)
        return;
    m_roots.push_back(root);
    SortByZOrder();
}

void UISystem::RemoveUI(UIBase* root)
{
    auto it = std::find(m_roots.begin(), m_roots.end(), root);
    if (it != m_roots.end())
        m_roots.erase(it);

    if (m_hovered == root) m_hovered = nullptr;
    if (m_captured == root) m_captured = nullptr;
}

void UISystem::Update(float deltaTime)
{
    for (UIBase* ui : m_roots)
    {
        if (ui && ui->IsActive())
            ui->Update(deltaTime);
    }
}

void UISystem::Render()
{
    UIRenderer::Get().BeginFrame();

    for (UIBase* ui : m_roots)
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
    POINT pt{ x, y };

    UIBase* hit = FindTopHit(pt);
    m_captured = hit;

    if (hit)
    {
        // Debug
        //char buf[256];
        //sprintf_s(buf, "MouseDown at (%d, %d)\n", x, y);
        //OutputDebugStringA(buf);
        //OutputDebugStringA("\n");

        //hit->SetState(UIBase::State::Pressed);
        hit->OnMouseDown(pt);
    }
}

void UISystem::ProcessMouseUp(int x, int y)
{
    POINT pt{ x , y };
    UIBase* upHit = FindTopHit(pt);
    UIBase* captured = m_captured;

    if (captured)
    {
        captured->OnMouseUp(pt);
        
        if (captured == upHit)
        {
            if (captured->IsEnabled() && captured->IsVisible() && captured->IsActive())
                captured->OnClick();
        }

        m_captured = nullptr;
    }

    ProcessMouseMove(x, y);
}

void UISystem::ProcessMouseMove(int x, int y)
{
    POINT pt{ x, y };

    if (m_captured)
    {
        m_captured->OnMouseMove(pt);
        return;
    }

    UIBase* hit = FindTopHit(pt);

    if (hit != m_hovered)
    {
        if (m_hovered)
        {
            m_hovered->OnMouseLeave();

            //if (m_hovered->GetState() != UIBase::State::Disabled)
            //    m_hovered->SetState(UIBase::State::Normal);
        }

        m_hovered = hit;

        if (m_hovered)
        {
            m_hovered->OnMouseEnter();
            //if (m_hovered->GetState() != UIBase::State::Disabled)
            //    m_hovered->SetState(UIBase::State::Hovered);
        }
    }

    if (m_hovered)
        m_hovered->OnMouseMove(pt);
}

void UISystem::Resize(int x, int y)
{
    m_uiX = x;
    m_uiY = y;
}

void UISystem::SortByZOrder()
{
    std::stable_sort(m_roots.begin(), m_roots.end(),
        [](const UIBase* a, const UIBase* b)
        {
            return a->GetZOrder() < b->GetZOrder();
        }
     );
}

UIBase* UISystem::FindTopHit(const POINT& pt)
{
    for (int i = (int)m_roots.size() - 1; i >=0; --i)
    {
        UIBase* root = m_roots[i];
        if (!root) continue;

        // 만약 입력 차단 패널이면
        //if (root->BlocksInput())
        //{
        //    UIBase* hit = hit = FindTopHitRecursive(root, pt);
        //    return hit;
        //}

        //UIBase* hit = FindTopHitRecursive(root, pt);
        //if (hit)
        //    return hit;

        if (UIBase* hit = root->Pick(pt))
            return hit;
    }

    return nullptr;
}

UIBase* UISystem::FindTopHitRecursive(UIBase* node, const POINT& pt)
{
    if (!node || !node->IsActive() || !node->IsVisible() || !node->IsEnabled())
        return nullptr;

    // 자식이 위에 그려짐.
    for (int i = (int)node->GetChildCount() - 1; i >=0; --i)
    {
        UIBase* child = node->GetChild(i);
        if (!child) continue;

        UIBase* hitChild = FindTopHitRecursive(child, pt);

        if (hitChild)
            return hitChild;
    }

    // 자식에서 못찾으면 자신을 검사
    if (node->IsEnabled() && node->HitTest(pt))
        return node;

    return nullptr;
}

void UISystem::OnInputProcess(const Keyboard::State&, const Keyboard::KeyboardStateTracker&, const Mouse::State& mouse, const Mouse::ButtonStateTracker& mouseTracker)
{
    //POINT pt{
    //    static_cast<int>(mouse.x),
    //    static_cast<int>(mouse.y)
    //};

    //ProcessMouseMove(pt.x, pt.y);

    //if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
    //    ProcessMouseDown(pt.x, pt.y);

    //if (mouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED)
    //    ProcessMouseUp(pt.x, pt.y);

    ProcessMouseMove(m_uiX, m_uiY);

    if (mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
        ProcessMouseDown(m_uiX, m_uiY);

    if (mouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED)
        ProcessMouseUp(m_uiX, m_uiY);
}
