#pragma once

#include <type_traits>
#include <tuple>

namespace vtable_bind_details {
    template<typename T>
    struct remove_first;

    template<typename T, typename... Ts>
    struct remove_first<std::tuple<T, Ts...>> {
        using type = std::tuple<Ts...>;
    };

    template<typename T>
    struct get_first;

    template<typename T, typename... Ts>
    struct get_first<std::tuple<T, Ts...>> {
        using type = T;
    };

    template<typename T>
    struct fn_ptr_args;

    template<typename R, typename... Ts>
    struct fn_ptr_args<R(__stdcall*)(Ts...)> {
        using first = std::remove_pointer_t<typename get_first<std::tuple<Ts...>>::type>;
        using args = typename remove_first<std::tuple<Ts...>>::type;
    };

    template<typename T>
    struct mem_fn_class_type;

    template<typename R, typename ClassT, typename... Ts>
    struct mem_fn_class_type<R(__stdcall ClassT::*)(Ts...)> {
        using type = ClassT;
    };

    template<auto CppMemFn, typename CVtablePtrT, typename ArgsT>
    struct vtable_bind_helper;

    template<auto CppMemFn, typename CVtablePtrT, typename... ArgsT>
    struct vtable_bind_helper<CppMemFn, CVtablePtrT, std::tuple<ArgsT...>> {
        static void bind(CVtablePtrT& cPtr)
        {
            using CObjectT = typename fn_ptr_args<CVtablePtrT>::first;
            using CppObjectT = typename mem_fn_class_type<decltype(CppMemFn)>::type;

            cPtr = [](CObjectT* This, ArgsT... args)
            {
                auto cppThis = static_cast<CppObjectT*>(This);
                return (cppThis->*CppMemFn)(args...);
            };
        }
    };
}

template<auto CppMemFn, typename CVtablePtrT>
void vtable_bind(CVtablePtrT& cPtr) {
    using namespace vtable_bind_details;
    using helper = vtable_bind_helper<CppMemFn, CVtablePtrT, typename fn_ptr_args<CVtablePtrT>::args>;
    helper::bind(cPtr);
}

template<typename CppClassT, typename CVtable>
void vtable_bind_base(CVtable& vtable) {
    vtable_bind<&CppClassT::QueryInterface>(vtable.QueryInterface);
    vtable_bind<&CppClassT::AddRef>(vtable.AddRef);
    vtable_bind<&CppClassT::Release>(vtable.Release);
}
