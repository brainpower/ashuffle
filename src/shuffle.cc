#include <cassert>
#include <cstdlib>
#include <numeric>
#include <string>
#include <vector>
#include <algorithm>

#include "shuffle.h"

namespace ashuffle {

void ShuffleChain::Clear() {
    _window.clear();
    _pool.clear();
    _items.clear();
}

void ShuffleChain::Add(ShuffleItem item) {
    _items.emplace_back(item);
    _pool.push_back(_items.size() - 1);
}

size_t ShuffleChain::Len() { return _items.size(); }
size_t ShuffleChain::LenURIs() {
    return std::accumulate(_items.begin(), _items.end(), 0,
        [](size_t sum, const ShuffleItem& group){ return sum + group._uris.size(); });
}

/* ensure that our window is as full as it can possibly be. */
void ShuffleChain::FillWindow() {
    while (_window.size() <= _max_window && _pool.size() > 0) {
        /* push a random song from the pool onto the end of the window */
        size_t idx = rand() % _pool.size();
        _window.push_back(_pool[idx]);
        _pool.erase(_pool.begin() + idx);
    }
}

const std::vector<std::string>& ShuffleChain::Pick() {
    assert(Len() != 0 && "cannot pick from empty chain");
    FillWindow();
    size_t picked_idx = _window[0];
    _window.pop_front();
    _pool.push_back(picked_idx);
    return _items[picked_idx]._uris;
}

std::vector<std::vector<std::string>> ShuffleChain::Items() {
    std::vector<std::vector<std::string>> result;
    std::transform(_items.begin(), _items.end(), std::back_inserter(result),
       [](auto group) { return group._uris; });
    return result;
}

}  // namespace ashuffle
