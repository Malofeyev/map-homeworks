#include <iostream>

#include <mutex>

class Data {
public:
    friend void swap1(Data& data1, Data& data2);
    friend void swap2(Data& data1, Data& data2);
    friend void swap3(Data& data1, Data& data2);

    Data(int i, double d, char c, bool b) 
        : i(i), d(d), c(c), b(b) {}
    void print() {
        std::cout << i << ' ' << d << ' ' << c << ' ' << b << std::endl;
    }
private:
    std::mutex m;
    int i;
    double d;
    char c;
    bool b;
};

void swap1(Data& data1, Data& data2) {
    std::lock(data1.m, data2.m);
    std::lock_guard<std::mutex> lk1(data1.m, std::adopt_lock);
    std::lock_guard<std::mutex> lk2(data2.m, std::adopt_lock);

    std::swap(data1.i, data2.i);
    std::swap(data1.d, data2.d);
    std::swap(data1.c, data2.c);
    std::swap(data1.b, data2.b);
}

void swap2(Data& data1, Data& data2) {
    std::scoped_lock lock(data1.m, data2.m);

    std::swap(data1.i, data2.i);
    std::swap(data1.d, data2.d);
    std::swap(data1.c, data2.c);
    std::swap(data1.b, data2.b);
}

void swap3(Data& data1, Data& data2) {
    std::unique_lock lock1{data1.m, std::defer_lock};
    std::unique_lock lock2{data2.m, std::defer_lock};
 
    std::lock(lock1, lock2);

    std::swap(data1.i, data2.i);
    std::swap(data1.d, data2.d);
    std::swap(data1.c, data2.c);
    std::swap(data1.b, data2.b);
}

int main(int argc, char *argv[]) {
    Data d1(1, 1.0, '1', false);
    Data d2(2, 2.0, '2', true);
    swap1(d1, d2);
    d1.print();
    d2.print(); 
    swap2(d1, d2);
    d1.print();
    d2.print(); 
    swap3(d1, d2);
    d1.print();
    d2.print(); 
    return 0;
}
