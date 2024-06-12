#include <iostream>
#include <fstream>
#include <random>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <random>
#include <algorithm>

namespace iptools
{
    // 将点分十进制的IP地址转换为uint32_t
    uint32_t ipToUint32(const std::string &ip)
    {
        int a, b, c, d, last;
        char dot;
        std::istringstream iss(ip);
        iss >> a >> dot >> b >> dot >> c >> dot >> d;
        uint32_t res = (a << 24) | (b << 16) | (c << 8) | d;
        return res;
    }

    // 将点分十进制的IP地址转换为uint32_t
    uint64_t ipToUint64(const std::string &ip)
    {
        uint64_t a, b, c, d, last;
        char dot;
        std::istringstream iss(ip);
        iss >> a >> dot >> b >> dot >> c >> dot >> d;
        uint64_t res = (a << 48) | (b << 32) | (c << 16) | d;
        return res;
    }

    // 分割4字节的IP地址为第一个字节和后三个字节
    std::pair<uint8_t, uint32_t> splitIP(uint32_t ip)
    {
        uint8_t firstByte = (ip >> 24) & 0xFF; // 提取第一个字节
        uint32_t remainingBytes = ip & 0x00FFFFFF; // 提取后三个字节
        return {firstByte, remainingBytes};
    }

    std::vector<uint64_t> readIpAddresses(const std::string &path)
    {
        std::vector<uint64_t> ipAddresses;
        std::ifstream infile(path);
        std::string line;

        while (std::getline(infile, line))
        {
            std::istringstream iss(line);
            std::string ip;
            char slash;
            int subnet;
            // 这里支持读入24.230.0.0/19和169.215.70.48
            // 由于数据类型是 std::string，这个操作会读取连续的字符直到遇到默认的分隔符（通常是空格、换行符或其他空白字符）。
            // 点号（.）并不是默认的分隔符，因此点号和数字一起被视为字符串的一部分，并全部被读入 ip 变量中。
            iss >> ip >> slash >> subnet;
            // 只读取IP地址部分，忽略子网掩码
            ipAddresses.push_back(ipToUint32(ip));
        }
        return ipAddresses;
    }

    std::vector<std::pair<uint8_t, uint32_t>> readSplitIpAddresses(const std::string &path)
    {
        std::vector<std::pair<uint8_t, uint32_t>> ipAddresses;
        std::ifstream infile(path);
        std::string line;

        while (std::getline(infile, line))
        {
            std::istringstream iss(line);
            std::string ip;
            iss >> ip;
            ipAddresses.push_back({splitIP(ipToUint32(ip))});
        }
        return ipAddresses;
    }

    // Function to generate n sequential IPv4 addresses starting from 0.0.0.0
    std::vector<std::string> generateSequentialIPs(uint32_t n)
    {
        std::vector<std::string> ipAddresses;
        uint32_t ip = 0;
        while (n--)
        {
            std::ostringstream oss;
            oss << ((ip >> 24) & 0xFF) << '.'
                << ((ip >> 16) & 0xFF) << '.'
                << ((ip >> 8) & 0xFF) << '.'
                << (ip & 0xFF);
            ipAddresses.push_back(oss.str());
            ip++;
        }
        return ipAddresses;
    }

    // Function to generate n unique random IPv4 addresses
    std::vector<std::string> generateRandomIPs(uint32_t n)
    {
        std::set<uint32_t> uniqueIPs;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(0, UINT32_MAX);

        while (uniqueIPs.size() < n)
        {
            uniqueIPs.insert(dis(gen));
        }

        std::vector<std::string> ipAddresses;
        for (auto ip: uniqueIPs)
        {
            std::ostringstream oss;
            oss << ((ip >> 24) & 0xFF) << '.'
                << ((ip >> 16) & 0xFF) << '.'
                << ((ip >> 8) & 0xFF) << '.'
                << (ip & 0xFF);
            ipAddresses.push_back(oss.str());
        }
        std::shuffle(ipAddresses.begin(), ipAddresses.end(), std::mt19937(std::random_device()()));
        return ipAddresses;
    }
}