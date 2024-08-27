#include <iostream>
#include <string>

#include "lru_cache.h"

template class LRUCache<std::string, std::string>;

using namespace std;

int main() {
    LRUCache<string, string> cache(10);

    for (int i = 0; i < 15; i++) {
        cache.put(string(1, 'a' + i), string(1, 'a' + i));
        cache.print();
    }

    for (int i = 0; i < 5; i++) {
        cache.put(string(1, 'a' + i), string(1, 'a' + i));
        cache.print();
    }

    for (int i = 0; i < 24; i++) {
        cout << "LOOKING UP: " << string(1, 'a' + i) << ": " <<
                cache.retrieve(string(1, 'a' + i)).first << endl;
        if (cache.retrieve(string(1, 'a' + i)).first.empty()) cout << "IT'S NOT THERE DUDE" << endl;
        cache.print();
    }

    for (int i = 0; i < 20; i++) {
        cout << "REMOVING " << string(1, 'a' + i) << ": " <<
                cache.remove(string(1, 'a' + i)) << endl;
        cache.print();
    }

    cache.evict_all();
    cache.print();
}
