#pragma once
#include "UIBase.h"
#include "Utility/Singleton.h"
#include "UIRenderer.h"
#include "InputSystem.h"

// 아래 코드는 추후 협업을 위한 엔진 작업용 코드입니다.
//namespace engine
//{
//    class UIBase;
//
//    class UIManager : public Singleton<UIManager>
//    {
//    public:
//        std::vector<UIBase*> m_uiList;
//
//    private:
//        UIManager();
//        ~UIManager();
//
//
//    public:
//        void Shutdown();
//
//
//    // UI 등록 /  해제
//        void AddUI(UIBase* ui);
//        void RemoveUI(UIBase* ui);
//
//        void Update(float deltaTime);
//        void Render();
//
//        void ProcessMouseDown(int x, int y);
//        void ProcessMouseUp(int x, int y);
//        void ProcessMouseMove(int x, int y);
//
//    private:
//        void SortByZOrder();
//
//    private:
//        friend class Singleton<UIManager>;
//    };
//}

class UIBase;
class UIRenderer;

class UISystem : public Singleton<UISystem>, public InputProcesser
{
public:
    std::vector<UIBase*> m_roots;

    UISystem();
    ~UISystem();

public:
    bool Initialize(
        HWND hWnd,
        ID3D11Device* device,
        ID3D11DeviceContext* context,
        IDXGISwapChain* swapChain
    )
    {
        return UIRenderer::Get().Initialize(
            hWnd, device, context, swapChain
        );
    }

    void Shutdown();

public:
    //UI 등록 / 해제
    void AddUI(UIBase* ui);
    void RemoveUI(UIBase* ui);

    void Update(float deltaTime);
    void Render();

    void ProcessMouseDown(int x, int y);
    void ProcessMouseUp(int x, int y);
    void ProcessMouseMove(int x, int y);

    void OnInputProcess(const Keyboard::State&, const Keyboard::KeyboardStateTracker&,
        const Mouse::State& mouse, const Mouse::ButtonStateTracker& mouseTracker) override;
private:
    void SortByZOrder();

    UIBase* FindTopHit(const POINT& pt);

    static UIBase* FindTopHitRecursive(UIBase* node, const POINT& pt);

private:
    UIBase* m_hovered = nullptr;
    UIBase* m_captured = nullptr;

    bool m_isShutdown = false;

    friend class Singleton<UISystem>;
};