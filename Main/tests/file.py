import sys
from os.path import dirname
sys.path.append(dirname(__file__))

from runasadmin import run_elevated, run_non_elevated
import file_two

@run_elevated
def ElevatedFunctionEvecuted(a=1) -> int:
    print("ElevatedFunctionEvecuted")
    return a

@run_non_elevated
def NonElevatedFunctionEvecuted():
    print("NonElevatedFunctionEvecuted")
    return 0

if __name__ == '__main__':
    print(NonElevatedFunctionEvecuted.__qualname__)
    file_two.NonElevatedFunctionEvecuted()
    ElevatedFunctionEvecuted(1)