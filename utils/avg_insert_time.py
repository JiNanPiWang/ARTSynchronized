import os
import subprocess

os.chdir('ARTSynchronized')
command = 'bash run_example.sh'
subprocess.run(command, shell=True)