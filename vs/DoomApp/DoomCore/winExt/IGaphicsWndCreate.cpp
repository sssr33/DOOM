#include "IGaphicsWndCreate.h"
#include "InterfacePtr.h"

#include <mutex>

class CreateIGraphicsWndSingleton {
public:
    static CreateIGraphicsWndSingleton& Instance() {
        static CreateIGraphicsWndSingleton instance;
        return instance;
    }

    std::mutex& GetMtx() {
        return this->mtx;
    }

    const InterfacePtr<IGraphicsWndFactory>& GetIGraphicsWndFactory() const {
        return this->graphicsWndFactory;
    }

    void SetIGraphicsWndFactory(InterfacePtr<IGraphicsWndFactory>&& factory) {
        this->graphicsWndFactory = std::move(factory);
    }

private:
    CreateIGraphicsWndSingleton() = default;

    std::mutex mtx;
    InterfacePtr<IGraphicsWndFactory> graphicsWndFactory;
};

HRESULT CreateIGraphicsWnd(const char* implName, IGraphicsWnd** res) {
    auto& inst = CreateIGraphicsWndSingleton::Instance();
    auto lk = std::lock_guard(inst.GetMtx());
    auto& graphicsWndFactory = inst.GetIGraphicsWndFactory();

    if (graphicsWndFactory) {
        return graphicsWndFactory->vtable->CreateGraphicsWnd(graphicsWndFactory.get(), implName, res);
    }

    return E_NOTIMPL;
}

HRESULT GetIGraphicsWndFactory(IGraphicsWndFactory** factory) {
    if (!factory) {
        return E_POINTER;
    }

    auto& inst = CreateIGraphicsWndSingleton::Instance();
    auto lk = std::lock_guard(inst.GetMtx());
    auto graphicsWndFactory = inst.GetIGraphicsWndFactory();

    *factory = graphicsWndFactory.detach();

    return S_OK;
}

HRESULT SetIGraphicsWndFactory(IGraphicsWndFactory* factory) {
    auto& inst = CreateIGraphicsWndSingleton::Instance();
    auto lk = std::lock_guard(inst.GetMtx());

    inst.SetIGraphicsWndFactory(InterfacePtr<IGraphicsWndFactory>(factory, AddRefKey{}));

    return S_OK;
}
