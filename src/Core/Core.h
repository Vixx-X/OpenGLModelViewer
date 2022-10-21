#pragma once

#include <memory>

#ifdef GLMV_DEBUG
    #define GLMV_ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion Failed: %s", __VA_ARGS__); __debugbreak(); } }
#else
    #define GLMV_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace GLMV {

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Scope<T> CreateScope(Args&& ... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename ... Args>
    constexpr Ref<T> CreateRef(Args&& ... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

}

#include "Core/Log.h"
