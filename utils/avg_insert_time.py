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
    filename = 'run_example.sh'
    # 将标准输出重定向到管道中
    process = subprocess.Popen(['bash', filename], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    # communicate() 方法用于与子进程进行交互，它会等待子进程完成，并返回一个元组，其中包含子进程的标准输出和标准错误输出
    stdout, _ = process.communicate()
    return stdout.decode('utf-8')

def get_insert_time(stdout):
    # \d+：表示匹配一个或多个数字。\d 是表示数字的元字符，+ 表示匹配前面的元素一次或多次。因此，\d+ 匹配任意长度的数字串，例如 123、4567 等。
    # (\d+)：表示使用圆括号将 \d+ 部分括起来，形成一个捕获组。捕获组可以用于提取匹配到的文本内容。
    match = re.search(r'insert, \d+ (\d+)us', stdout)
    if match:
        return int(match.group(1))
    raise ValueError("Insert time not found in the output")

def get_insert_count(stdout):
    match = re.search(r'insert count: (\d+)', stdout)
    if match:
        return int(match.group(1))
    raise ValueError("insert count not found in the output")

def calc_OLC(n):
    sum_time = 0
    sum_count = 0
    for i in range(n):
        stdout = run()
        _time = get_insert_time(stdout)
        _count = get_insert_count(stdout)
        sum_time += _time
        sum_count += _count
        logging.info(f"Insert time: {_time} us, Insert count: {_count}")
    # 计算平均时间
    avg_time = sum_time / n
    # 将平均时间写入日志
    logging.info(f"OLC average Insert Time: {avg_time} us, average Insert Count: {sum_count / n}")

def calc_ROWEX(n):
    sum_time = 0
    sum_count = 0
    for i in range(n):
        stdout = run()
        _time = get_insert_time(stdout)
        _count = get_insert_count(stdout)
        sum_time += _time
        sum_count += _count
        logging.info(f"Insert time: {_time} us, Insert count: {_count}")
    # 计算平均时间
    avg_time = sum_time / n
    # 将平均时间写入日志
    logging.info(f"ROWEX average Insert Time: {avg_time} us, average Insert Count: {sum_count / n}")

if __name__ == '__main__':
    n = 10
    calc_OLC(n)
    # calc_ROWEX(n)
