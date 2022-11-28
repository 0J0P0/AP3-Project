import subprocess as sp
import sys


"""
Programm to execute check.cc programm for all the files in 
a specific mode.
"""

mode = str(sys.argv[1]) + "-"
prog = "check.cc"

def main():
    if mode == "easy-" or mode == "med-":
        for i in range(1, 11):
            input = "./public_benchs/" + mode + str(i) + ".txt"
            output = "./benchs_results/" + mode + str(i) + ".txt"
            sp.call(["g++", "-Wall", "-O2", "-std=c++17", prog]) 
            sp.call(args=["./a.out", input, output])
    elif mode == "hard-":
        for i in range(1, 21):
            input = "./public_benchs/" + mode + str(i) + ".txt"
            output = "./benchs_results/" + mode + str(i) + ".txt"
            sp.call(["g++", "-Wall", "-O2", "-std=c++17", prog]) 
            sp.call(args=["./a.out", input, output])


if __name__ == "__main__":
    main()
