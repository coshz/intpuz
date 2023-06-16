#pragma once
#include <utility>

template<typename Model, int...Ns>
struct ModelArray
{
    template<int Index>
    auto & getIndex()
    {
        static_assert(((Index == Ns) || ... ), "Index is invalid.");
        static Model<Index> model;
        return model;
    }
};

template<typename T=int, T low, T high, T step=1>
auto make_seq()
{
    constexpr size_t n = (high - low + step - 1) / step; // floor[(h-l)/s]
    auto f = [](size_t i) { return low + i * step; };
    auto fmap = []<typename T1, T1 ... args>
                (std::integer_sequence<T1, args...>, auto func) {
        return std::integer_sequence<T, func(args)...>{};
    };
    return fmap(std::make_integer_sequence<T, n+1>{}, f);
}

template<typename M, int min, int max>
auto make_models()
{
    auto fmap = []<int... args>(std::integer_sequence<int, args...>){
        return ModelArray<M, args...>{};
    };
    auto models = fmap(make_seq<int,min,max>());
    return models;
}
