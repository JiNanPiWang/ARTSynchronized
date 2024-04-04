#include "../example.cpp"
#include "../utils/iptools.cpp"
#include <bits/stdc++.h>

using namespace std;

int main(int argc, char **argv)
{
    string data_dir = "../datasets/ipgeo/by-continent/EU.txt";
    auto ipAddresses = iptools::readIpAddresses(data_dir);
    cout << std::hex << ipAddresses[0] << endl;

    return 0;
}
