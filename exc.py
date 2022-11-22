import subprocess as sp
import sys


"""
Programm to execute a programm (exh.cc, greedy.cc, mh.cc) for all the files in 
a specific mode.
"""

mode = str(sys.argv[2]) + "-"
prog = str(sys.argv[1])

def main():
    
    for i in range(1, 11):
        input = "./public_benchs/" + mode + str(i) + ".txt"
        output = "./benchs_results/" + mode + str(i) + ".txt"
        sp.call(["g++", "-Wall", "-O2", "-std=c++17", prog]) 
        print("Excecuting bench: " + mode + str(i) + ".txt")
        sp.call(args=["./a.out", input, output])


if __name__ == "__main__":
    main()