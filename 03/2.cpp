#include <iostream>
#include <algorithm>
#include <array>
#include <atomic>
#include <functional>
#include <future>
#include <random>

using namespace std::chrono_literals;

constexpr int SIZE = 10;
using Iter = std::array<int, SIZE>::iterator;

template<class ForwardIt, class UnaryFunc>
void for_each(ForwardIt first, ForwardIt last, UnaryFunc f) {
    auto size = std::distance(first, last);
    if (size == 0) {
        return; 
    } else if (size == 1) {
        f(*first);
    } else {
        auto mid = first + size / 2;
        std::packaged_task<void(ForwardIt, ForwardIt, UnaryFunc)> task1(for_each<ForwardIt, UnaryFunc>);
        std::packaged_task<void(ForwardIt, ForwardIt, UnaryFunc)> task2(for_each<ForwardIt, UnaryFunc>);
        std::future<void> f1 = task1.get_future();
        std::future<void> f2 = task2.get_future();

        task1(first, mid, f);
        task2(mid, last, f);
        f1.get();
        f2.get();
    }

}

int main(int argc, char *argv[]) {
    
    std::array<int, SIZE> arr;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(-100, 100);
    std::generate(arr.begin(), arr.end(), [&distrib, &gen](){ return distrib(gen); });

    for (int e : arr) {
        std::cout << e << ' ';
    }
    std::cout << std::endl;

    for_each(arr.begin(), arr.end(), [](int& e) { e--; });


    for (int e : arr) {
        std::cout << e << ' ';
    }
    std::cout << std::endl;
    return 0;
}
