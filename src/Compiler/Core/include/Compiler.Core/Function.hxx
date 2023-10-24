// ReSharper disable CppClangTidyBugproneForwardingReferenceOverload
#pragma once
#include <type_traits>
#include <utility>


// Type erasing function wrapper.
namespace Weave
{
    template <typename Fn>
    class FunctionRef;

    template <typename R, typename... Ts>
    class FunctionRef<R(Ts...)> final
    {
    private:
        using Callback = R(void* callable, Ts... ts);

        template <typename Callable>
        static R CallbackFunction(void* callable, Ts... ts)
        {
            return (*static_cast<Callable*>(callable))(std::forward<Ts>(ts)...);
        }

    private:
        Callback* _callback = nullptr;
        void* _callable = nullptr;

    public:
        FunctionRef() = default;

        FunctionRef(std::nullptr_t) { }

        template <typename Callable>
            requires(!std::is_same_v<std::remove_cvref_t<Callable>, FunctionRef> && std::is_invocable_r_v<R, Callable, Ts...>)
        FunctionRef(Callable&& callable)
            : _callback{CallbackFunction<std::remove_reference_t<Callable>>}
            , _callable{&callable}
        {
        }

        R operator()(Ts... ts) const
        {
            return this->_callback(this->_callable, std::forward<Ts>(ts)...);
        }

        explicit operator bool() const
        {
            return this->_callback != nullptr;
        }
    };
}
