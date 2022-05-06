#pragma once
#include <functional>
#include <utility>


template <typename Callable>
requires std::is_invocable_v<Callable>
class call_at_scope_exit_t{
    Callable _c;
public:
    explicit call_at_scope_exit_t(Callable&& c) : _c(std::forward<Callable>(c)) {}
    ~call_at_scope_exit_t(){
        std::invoke(_c);
    }
};