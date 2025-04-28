#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

template<std::memory_order order>
void client(std::atomic_int& counter, int client_count) {
    while (client_count--) {
        counter.fetch_add(1, order);
        std::this_thread::sleep_for(1s);
    }
}

template<std::memory_order order>
void oper(std::atomic_int& counter) {
    while (counter > 0) {
        counter.fetch_sub(1, order);
        std::this_thread::sleep_for(2s);
    }    
}

template<std::memory_order order>
void start_client_queue(int client_count) {

    std::atomic_int counter = 0;

    auto start = std::chrono::steady_clock::now();

    std::thread t1(client<order>, std::ref(counter), client_count);
    std::thread t2(oper<order>, std::ref(counter));

    t1.join();
    t2.join();

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << duration.count() << "\n";
}

int main(int argc, char *argv[]) {
    int client_count;
    std::cin >> client_count;
    start_client_queue<std::memory_order::memory_order_relaxed>(client_count);
    start_client_queue<std::memory_order::memory_order_acq_rel>(client_count);
    start_client_queue<std::memory_order::memory_order_seq_cst>(client_count);
    return 0;
}
