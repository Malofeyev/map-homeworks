#include <iostream>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

void client(volatile int& counter, int client_count) {
    while (client_count--) {
        counter++;
        std::this_thread::sleep_for(1s);
    }
}

void oper(volatile int& counter) {
    while (counter > 0) {
        counter--;
        std::this_thread::sleep_for(2s);
    }    
}

int main(int argc, char *argv[]) {
    int client_count;
    std::cin >> client_count;

    volatile int counter = 0;

    auto start = std::chrono::steady_clock::now();

    std::thread t1(client, std::ref(counter), client_count);
    std::thread t2(oper, std::ref(counter));

    t1.join();
    t2.join();

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << duration.count() << "\n";
    return 0;
}
