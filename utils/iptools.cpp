#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace iptools
{
    // 将点分十进制的IP地址转换为uint32_t
    uint32_t ipToUint(const std::string &ip)
    {
        int a, b, c, d, last;
        char dot;
        std::istringstream iss(ip);
        iss >> a >> dot >> b >> dot >> c >> dot >> d;
        uint32_t res = (a << 24) | (b << 16) | (c << 8) | d;
        return res;
    }

    std::vector <uint32_t> readIpAddresses(const std::string &path)
    {
        std::vector <uint32_t> ipAddresses;
        std::ifstream infile(path);
        std::string line;

        while (std::getline(infile, line))
        {
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
}