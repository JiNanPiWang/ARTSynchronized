#include "../example.cpp"
#include <bits/stdc++.h>

using namespace std;

// 将点分十进制的IP地址转换为uint32_t
uint32_t ipToUint(const std::string &&ip) {
    int a, b, c, d;
    char dot;
    std::istringstream iss(ip);
    iss >> a >> dot >> b >> dot >> c >> dot >> d;
    uint32_t res = (a << 24) | (b << 16) | (c << 8) | d;
    return res;
}

int main(int argc, char **argv) {
    // if (argc != 4) {
    //     printf("usage: %s n 0|1|2 线程数 test需要自己修改代码 原版直接输入\nn: number of keys\n0: sorted keys\n1: dense keys\n2: sparse keys\n样例 ./example 5120000 0 1", argv[0]);
    //     return 1;
    // }
    // test(argv);   //分桶版

    // singlethreaded(argv);

    // multithreaded_ART_OLC(argv);   //原版

    cout << ipToUint("24.230.0.0/19") << endl;

    return 0;
}
