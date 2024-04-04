#!/bin/bash

# 创建一个新的build目录并进入
mkdir -p build
cd build

rm * -rf

# 运行cmake生成Makefile
cmake ..

# 使用make编译项目
make

# 参数：键的数量；键的分布类型（0表示已排序，1表示密集随机，2表示稀疏）；线程数 
./example 80784 0 16

rm * -rf

# 返回到脚本所在目录
cd ..

