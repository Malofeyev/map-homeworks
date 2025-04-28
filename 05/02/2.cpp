#include <iostream>
#include <array>
#include <chrono>
#include <iomanip>
#include <thread>
#include <vector>

const int FIELD_SIZE = 12;

void parallel_add(std::size_t thread_count, std::size_t element_count,
        const std::vector<int>& arg1, 
        const std::vector<int>& arg2,
        std::vector<int>& res) {

    std::vector<std::thread> ts;
    ts.reserve(thread_count);

    int div = element_count / thread_count;
    std::size_t mod = element_count % thread_count;
    
    auto add_part = [&arg1, &arg2, &res](int begin, int end) {
        for (int i = begin; i < end; i++) {
            res[i] = arg1[i] + arg2[i];
        }
    };

    std::size_t i = 0;
    int beg = 0;
    for (; i < mod; i++) {
        ts.emplace_back(add_part, beg, beg + div + 1);
        beg += div + 1;    
    }

    for (; i < std::min(thread_count, element_count); i++) {
        ts.emplace_back(add_part, beg, beg + div);
        beg += div;    
        
    }

    for (auto& thread : ts) {
        thread.join();
    }
}

void fill_vector(std::vector<int>& arr, std::size_t element_count) {
    arr.reserve(element_count);
    for (std::size_t i = 0; i < element_count; i++) {
        arr.push_back(rand());
    }
}

void test_parallel_add(std::size_t thread_count, std::size_t element_count) {
    
    std::vector<int> arg1, arg2, res;
    fill_vector(arg1, element_count);
    fill_vector(arg2, element_count);
    res.resize(element_count);

    auto start = std::chrono::steady_clock::now();

    parallel_add(thread_count, element_count, arg1, arg2, res);

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << std::setw(FIELD_SIZE) << duration.count();
    
}

void test_parallel_add(std::size_t thread_count, const std::vector<std::size_t>& element_counts) {
    std::cout << std::setw(FIELD_SIZE) << thread_count;
    
    for (std::size_t element_count : element_counts) {
        test_parallel_add(thread_count, element_count);
    }

    std::cout << std::endl;
}

void print_head(const std::vector<std::size_t>& element_counts) {
    std::cout << "\n" << std::setw(FIELD_SIZE) << "thread/size";
    for (std::size_t element_count : element_counts) {
        std::cout << std::setw(FIELD_SIZE) << element_count;
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {

    std::cout << "hardware concurrency - " << std::thread::hardware_concurrency() << std::endl;
    
    std::vector<std::size_t> element_counts = {1000, 10000, 100000, 1000000};
    std::vector<std::size_t> thread_counts = {1, 2, 4, 8, 16};

    print_head(element_counts);

    for (std::size_t thread_count : thread_counts) {
        test_parallel_add(thread_count, element_counts);
    }
    
    return 0;
}
