import os
import subprocess

def run():
    filename = 'run_example.sh'
    # 将标准输出重定向到管道中
    process = subprocess.Popen(['bash', filename], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    # communicate() 方法用于与子进程进行交互，它会等待子进程完成，并返回一个元组，其中包含子进程的标准输出和标准错误输出
    stdout, _ = process.communicate()
    return stdout.decode('utf-8')


if __name__ == '__main__':
    os.chdir('ARTSynchronized')
    print(run())