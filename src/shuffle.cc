#include <cassert>
#include <cstdlib>
#include <numeric>
#include <string>
#include <vector>

#include "shuffle.h"

namespace ashuffle {

void ShuffleChain::Clear() {
    _window.clear();
    _pool.clear();
    _items.clear();
}

void ShuffleChain::Add(ShuffleItem item) {
    _items.emplace_back(item);
    _pool.push_back(static_cast<int>(_items.size()) - 1);
}

unsigned ShuffleChain::Len() { return _items.size(); }
unsigned ShuffleChain::LenURIs() {
    unsigned sum = 0;
    for (auto& group : _items) {
        sum += group._uris.size();
    }
    return sum;
}

/* ensure that our window is as full as it can possibly be. */
void ShuffleChain::FillWindow() {
    while (_window.size() <= _max_window && _pool.size() > 0) {
        /* push a random song from the pool onto the end of the window */
        unsigned idx = rand() % _pool.size();
        _window.push_back(_pool[idx]);
        _pool.erase(_pool.begin() + idx);
    }
}

const std::vector<std::string>& ShuffleChain::Pick() {
    assert(Len() != 0 && "cannot pick from empty chain");
    FillWindow();
    int picked_idx = _window[0];
    _window.pop_front();
    _pool.push_back(picked_idx);
    return _items[picked_idx]._uris;
}

std::vector<std::vector<std::string>> ShuffleChain::Items() {
    std::vector<std::vector<std::string>> result;
    for (auto group : _items) {
        result.push_back(group._uris);
    }
    return result;
}

}  // namespace ashuffle