#pragma once
#include "UIBase.h"
#include "Utility/Singleton.h"

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

class UIManager : public Singleton<UIManager>
{
public:
    std::vector<UIBase*> m_uiList;

private:
    UIManager();
    ~UIManager();

public:
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

private:
    void SortByZOrder();

private:
    friend class Singleton<UIManager>;
};