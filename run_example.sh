#!/bin/bash

# 创建一个新的build目录并进入
mkdir -p build
cd build

rm * -rf

# 运行cmake生成Makefile
cmake ..

# 使用make编译项目
make

./example

rm * -rf

# 返回到脚本所在目录
cd ..

