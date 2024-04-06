import os
import subprocess
import re

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

if __name__ == '__main__':
    os.chdir('ARTSynchronized')
    n = 2
    sum_time = 0
    for i in range(n):
        stdout = run()
        _time = get_insert_time(stdout)
        sum_time += _time
        print(f"Insert time: {_time} us")
