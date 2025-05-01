#include <iostream>

#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <iomanip>
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

const int NUM_WIDTH = 2;
const int ID_WIDTH = 20;
const int NORMALIZED_PROGRESS_CNT = 20;
const int TIME_WIDTH = 10;

void setNextLine(int y) {
    std::cout << "\033[" << y + 1 << ";" << NUM_WIDTH + ID_WIDTH + NORMALIZED_PROGRESS_CNT + TIME_WIDTH + 1 << "H" << '\n'; 
    std::cout.flush();
}

void setProgressAtPosition(int left, int right, int y) {
    for (int i = left; i <= right; i++) {
        std::cout << "\033[" << y + 1 << ";" << i + NUM_WIDTH + ID_WIDTH << "H" << "\u2588";
    }
}

std::mutex write_lock;

void work(int num, int work_cnt) {
    std::chrono::steady_clock::time_point start = 
        std::chrono::steady_clock::now();
    const auto idle = 500ms;
    int left = 1;
    for (int finished_cnt = 1; finished_cnt < work_cnt; finished_cnt++) {
        int right = 
            finished_cnt * NORMALIZED_PROGRESS_CNT / work_cnt;
        std::this_thread::sleep_for(idle);
        std::lock_guard<std::mutex> lock(write_lock);
        setProgressAtPosition(left, right, num);
        std::cout.flush();
        left = right + 1;
    }

    int right = NORMALIZED_PROGRESS_CNT;
    std::this_thread::sleep_for(idle);
    std::lock_guard<std::mutex> lock(write_lock);
    setProgressAtPosition(left, right, num);
    std::chrono::steady_clock::time_point end = 
        std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << std::setw(TIME_WIDTH) << duration.count(); 
    std::cout.flush();
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
    threads.reserve(thread_cnt);


    // # id Progress Bar Time
    clearConsole();
    std::cout << std::setw(NUM_WIDTH) << "#";
    std::cout << std::setw(ID_WIDTH) << "Id";
    std::cout << std::setw(NORMALIZED_PROGRESS_CNT) << "Progress Bar";
    std::cout << std::setw(TIME_WIDTH) << "Time" << std::endl;

    {
        std::lock_guard<std::mutex> lock(write_lock);
        for (int i = 0; i < thread_cnt; i++) {

            std::cout << std::setw(NUM_WIDTH) << i;

            threads.emplace_back(work, i + 1, div + (i < mod?1:0));
            std::cout << std::setw(ID_WIDTH) << threads.back().get_id() << std::endl;
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    setNextLine(thread_cnt);

    return 0;
}
