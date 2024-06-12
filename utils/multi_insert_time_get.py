# 批处理获取多线程插入时间
import os
import re
import subprocess
import time
import logging
import time
from datetime import datetime, timedelta, timezone
# import avg_insert_time


def setup_logging(thread_num, file, utc_plus_8):
    current_time = datetime.now(utc_plus_8).strftime('%Y%m%d%H%M%S')
    log_filename = f'logs/run_log_{thread_num}_{file}_{current_time}.log'
    logging.basicConfig(filename=log_filename, level=logging.INFO, 
                        format='%(asctime)s:%(levelname)s:%(message)s', datefmt='%Y-%m-%d %H:%M:%S', force=True)
    logging.Formatter.converter = lambda *args: datetime.now(utc_plus_8).timetuple()


def get_file_num(filename):
    match = re.search(r'ips_(\d+)k', filename)
    if match:
        return int(match.group(1))
    raise ValueError("file num not found in the filename")


def compile_example(key_count, distribution_type, thread_num, file_path):
    # 创建一个新的build目录并进入
    subprocess.run(['mkdir', '-p', 'build'], check=True)
    subprocess.run(['rm', '-rf', './build/*'], check=True)  # 清理build目录
    subprocess.run(['cmake', '..'], cwd='build', check=True)  # 运行cmake生成Makefile
    subprocess.run(['make'], cwd='build', check=True)  # 使用make编译项目


def run_example(key_count, distribution_type, thread_num, file_path):
    # 构建example程序的运行命令
    example_command = f'./example {key_count} {distribution_type} {thread_num} {file_path}'

    # 运行example程序，将标准输出重定向到管道中
    process = subprocess.Popen(example_command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, cwd='build')
    stdout, _ = process.communicate()  # 等待子进程完成

    return stdout.decode('utf-8')


def get_insert_time(stdout):
    match = re.search(r'insert time: (\d+)', stdout)
    if match:
        return int(match.group(1))
    raise ValueError("insert time not found in the output")


def calc_insert_time_us(n, key_count, distribution_type, thread_num, file_path):
    sum_time = 0
    for i in range(n):
        stdout = run_example(key_count, distribution_type, thread_num, file_path)
        _time = get_insert_time(stdout)
        sum_time += _time
        logging.info(f"insert time: {_time} us") # insert time: 277 ms
    # 计算平均时间
    avg_time = sum_time / n
    return avg_time    


if __name__ == '__main__':
    utc_plus_8 = timezone(timedelta(hours=8))
    thread_num = [2, 4, 8, 16]
    # thread_num = [16]
    files = os.listdir('datasets/generated_ips')  # run in project root, or change this
    file_head = '../datasets/generated_ips/'

    for num in thread_num:
        for file in files:
            # if 'random_ips_10000k' not in file:
            #     continue
            # Call the new setup_logging function to create a unique logger for each iteration
            setup_logging(num, file, utc_plus_8)

            file_path = f'{file_head}{file}'
            logging.info(f'thread num: {num}, file: {file}')

            keys_count = int(get_file_num(file)) * 1000
            compile_example(keys_count, 0, num, file_path)

            n = 10
            avg_insert_time = calc_insert_time_us(n, keys_count, 0, num, file_path)

            logging.info(f"ROWEX average insert Time: {avg_insert_time} us")

