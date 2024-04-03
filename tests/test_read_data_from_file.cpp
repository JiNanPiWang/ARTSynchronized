#include "../example.cpp"
#include <bits/stdc++.h>

using namespace std;

// 将点分十进制的IP地址转换为uint32_t
uint32_t ipToUint(const std::string &ip) {
    int a, b, c, d, last;
    char dot;
    std::istringstream iss(ip);
    iss >> a >> dot >> b >> dot >> c >> dot >> d >> dot >> last;
    uint32_t res = (a << 24) | (b << 16) | (c << 8) | d;
    return res;
}

// 从文件中读取IP地址并转换为uint32_t的列表
std::vector<uint32_t> readIpAddresses(const std::string &path) {
    std::ifstream ifs(path);
    std::vector<uint32_t> res;
    string line;
    if (!ifs.is_open()) {
        std::cerr << "Error: cannot open file " << path << std::endl;
        return {};
    }

    while (getline(ifs, line))
    {
        res.push_back(ipToUint(line));
    }
    return res;
}

int main(int argc, char **argv) {
    // auto str = "24.230.0.0/19";
    // cout << std::hex << ipToUint(str) << endl;
    auto data_dir = "../datasets/ipgeo/by-continent/EU.txt";
    auto ipAddresses = readIpAddresses(data_dir);
    cout << std::hex << ipAddresses[1] << endl;

    return 0;
}
