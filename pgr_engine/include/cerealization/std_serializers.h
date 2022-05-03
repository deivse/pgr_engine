#pragma once

#include <utility>

namespace std {
    template <class Archive, typename A, typename B>
    void serialize(Archive& arr, std::pair<A,B>& pair){
        arr(pair.first, pair.second);
    }
}