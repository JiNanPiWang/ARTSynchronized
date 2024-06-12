import os
import subprocess
import re
import logging
from datetime import datetime, timedelta, timezone

# 配置日志信息，获取当前UTC+8时间并格式化为字符串
utc_plus_8 = timezone(timedelta(hours=8))
current_time = datetime.now(utc_plus_8).strftime('%Y%m%d%H%M%S')
logging.basicConfig(filename=f'logs/run_log_{current_time}.log', level=logging.INFO,
                    format='%(asctime)s:%(levelname)s:%(message)s', datefmt='%Y-%m-%d %H:%M:%S', force=True)
logging.Formatter.converter = lambda *args: datetime.now(utc_plus_8).timetuple()

def run():
    filename = 'run_compiled_example.sh'
    # 将标准输出重定向到管道中
    process = subprocess.Popen(['bash', filename], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    # communicate() 方法用于与子进程进行交互，它会等待子进程完成，并返回一个元组，其中包含子进程的标准输出和标准错误输出
    stdout, _ = process.communicate()
    return stdout.decode('utf-8')

def get_insert_time(stdout):
    # \d+：表示匹配一个或多个数字。\d 是表示数字的元字符，+ 表示匹配前面的元素一次或多次。因此，\d+ 匹配任意长度的数字串，例如 123、4567 等。
    # (\d+)：表示使用圆括号将 \d+ 部分括起来，形成一个捕获组。捕获组可以用于提取匹配到的文本内容。
    match = re.search(r'insert time: (\d+)', stdout)
    if match:
        return int(match.group(1))
    raise ValueError("insert time not found in the output")

def get_lookup_time(stdout):
    # \d+：表示匹配一个或多个数字。\d 是表示数字的元字符，+ 表示匹配前面的元素一次或多次。因此，\d+ 匹配任意长度的数字串，例如 123、4567 等。
    # (\d+)：表示使用圆括号将 \d+ 部分括起来，形成一个捕获组。捕获组可以用于提取匹配到的文本内容。
    match = re.search(r'lookup time: (\d+)', stdout)
    if match:
        return int(match.group(1))
    raise ValueError("insert time not found in the output")

def calc_insert_time_us(n):
    sum_time = 0
    for i in range(n):
        stdout = run()
        _time = get_insert_time(stdout)
        sum_time += _time
        logging.info(f"insert time: {_time} us") # insert time: 277 us
    # 计算平均时间
    avg_time = sum_time / n
    return avg_time

def calc_insert_time_ms(n):
    sum_time = 0
    for i in range(n):
        stdout = run()
        _time = get_insert_time(stdout)
        sum_time += _time
        logging.info(f"insert time: {_time} ms") # insert time: 277 ms
    # 计算平均时间
    avg_time = sum_time / n
    return avg_time

def calc_lookup_time_us(n):
    sum_time = 0
    for i in range(n):
        stdout = run()
        _time = get_lookup_time(stdout)
        sum_time += _time
        logging.info(f"Lookup time: {_time} us")
    # 计算平均时间
    avg_time = sum_time / n
    return avg_time

if __name__ == '__main__':
    n = 10

    # avg_insert_time = calc_insert_time_ms(n)
    avg_insert_time = calc_insert_time_us(n)

    # logging.info(f"ROWEX average insert Time: {avg_insert_time} ms")
    logging.info(f"ROWEX average insert Time: {avg_insert_time} us")
