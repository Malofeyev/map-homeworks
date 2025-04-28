#include <iostream>
#include <algorithm>
#include <array>
#include <future>
#include <random>
#include <thread>

using namespace std::chrono_literals;

constexpr int SIZE = 100;
using Iter = std::array<int, SIZE>::iterator;

Iter min_index(Iter begin, Iter end) {
    auto result = begin;
    for (; begin != end; begin++) {
        if (*begin < *result) {
            result = begin;
        } 
    }     
    std::this_thread::sleep_for(200ms);
    return result;
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

    const int MIN_BATCH_SIZE = 10;
    int batch_cnt = 5;

    for (Iter it = arr.begin(); it + 1 != arr.end(); it++) {
        int size = std::distance(it, arr.end());
        while (size / batch_cnt < MIN_BATCH_SIZE && batch_cnt > 1) {
            batch_cnt--; 
        }
        if (batch_cnt == 1) {
            auto min_it = min_index(it, arr.end());
            std::iter_swap(it, min_it);
        } else {
            std::vector<std::future<Iter>> fs;
            int div = size / batch_cnt;
            int mod = size % batch_cnt;
            auto begin = it;
            for (int i = 0; i < batch_cnt; i++) {
                auto end = begin + div + (i < mod?1:0);
                fs.push_back(std::async(min_index, begin, end));
                begin = end;
            }
            Iter res = fs[0].get();
            for (int i = 1; i < batch_cnt; i++) {
               Iter other = fs[i].get(); 
               if (*other < *res) {
                   res = other;
               } 
            }
            std::iter_swap(it, res);
        }
        
    }

    for (int e : arr) {
        std::cout << e << ' ';
    }

    return 0;
}
