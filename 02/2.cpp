#include <iostream>

#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstdlib>
#endif

using namespace std::chrono_literals;

void clearConsole() {
    #ifdef _WIN32
        system("cls");  // Windows
    #else
        system("clear"); // Linux/MacOS
    #endif
}

void work(std::atomic_int& work_cnt) {
    while (work_cnt.load(std::memory_order::memory_order_acquire) > 0) {
        work_cnt.fetch_sub(1, std::memory_order::memory_order_release);  
        std::this_thread::sleep_for(1s);
    }
}


void print_head(int num, const std::thread& thread) {
    std::cout << num << ") thread:" << thread.get_id() << '\n';
}

int main(int argc, char *argv[]) {
    int thread_cnt, work_cnt;
    std::cout << "Number of threads: ";
    std::cin >> thread_cnt;
    std::cout << "Amout of work: ";
    std::cin >> work_cnt;

    int div = work_cnt / thread_cnt;
    int mod = work_cnt % thread_cnt;

    std::vector<std::thread> threads;
    std::vector<int> work_cnts;
    std::vector<std::atomic_int> cur_work_cnts(thread_cnt);
    std::vector<std::chrono::steady_clock::time_point> starts;
    std::vector<std::chrono::duration<double>> durations;
    threads.reserve(thread_cnt);
    work_cnts.reserve(thread_cnt);
    starts.reserve(thread_cnt);
    durations.reserve(thread_cnt);

    for (int i = 0; i < thread_cnt; i++) {
        int work_per_thread = div + (i < mod?1:0);
        work_cnts.push_back(work_per_thread);
        cur_work_cnts.at(i).store(work_per_thread);
        durations.emplace_back(0);
    }

    for (int i = 0; i < thread_cnt; i++) {
        threads.emplace_back(work, std::ref(cur_work_cnts.at(i)));
        starts.push_back(std::chrono::steady_clock::now());
    }

    bool is_finished = false;
    int normalized_cnt = 20;
    while (!is_finished) {
        clearConsole();
        is_finished = true;
        for (int i = 0; i < thread_cnt; i++) {
            print_head(i, threads[i]);
            int cur_work = cur_work_cnts.at(i).load(std::memory_order_acquire);
            if (cur_work > 0) {
                int finished_cnt = work_cnts[i] - cur_work; 
                int normalized_finished_cnt = finished_cnt * normalized_cnt / work_cnts[i];
                std::cout << '[' << 
                    std::string(normalized_finished_cnt, '#') << 
                    std::string(normalized_cnt - normalized_finished_cnt, '.') << 
                    "]\n";
                is_finished = false;
            } else {
                if (durations[i].count() == 0) {
                    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                    durations[i] = end - starts[i];
                }
                std::cout << "Time: " << durations[i].count() << '\n';
                
            }
        }

        std::cout << std::endl;
        std::this_thread::sleep_for(2s);
    }

    for (int i = 0; i < thread_cnt; i++) {
        threads[i].join();
    }

    clearConsole();
    for (int i = 0; i < thread_cnt; i++) {
        print_head(i, threads[i]);
        std::cout << "Time: " << durations[i].count() << '\n';
    }
    return 0;
}
