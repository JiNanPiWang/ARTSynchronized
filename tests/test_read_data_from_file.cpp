#include "../example.cpp"
#include <bits/stdc++.h>

using namespace std;

// 将点分十进制的IP地址转换为uint32_t
uint32_t ipToUint(const std::string &ip) {
    int a, b, c, d, last;
    char dot;
    std::istringstream iss(ip);
    iss >> a >> dot >> b >> dot >> c >> dot >> d;
    uint32_t res = (a << 24) | (b << 16) | (c << 8) | d;
    return res;
}

std::vector<uint32_t> readIpAddresses(const std::string &path) {
    std::vector<uint32_t> ipAddresses;
    std::ifstream infile(path);
    std::string line;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string ip;
        char slash;
        int subnet;
        iss >> ip >> slash >> subnet;
        // 只读取IP地址部分，忽略子网掩码
        ipAddresses.push_back(ipToUint(ip));
    }

    return ipAddresses;
}

int main(int argc, char **argv) {
    // auto str = "24.230.0.0/19";
    // cout << std::hex << ipToUint(str) << endl;
    auto data_dir = "../datasets/ipgeo/by-continent/EU.txt";
    auto ipAddresses = readIpAddresses(data_dir);
    cout << std::hex << ipAddresses[4] << endl;

    return 0;
}
