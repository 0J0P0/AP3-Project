import subprocess as sp


def main():
    
    mode = "easy" + "-"
    prog = "exh.cc"
    for i in range(1, 11):
        input = "./public_benchs/" + mode + str(i) + ".txt"
        output = "./benchs_results/" + mode + str(i) + ".txt"
        sp.call(["g++", "-Wall", "-O2", "-std=c++17", prog]) 
        sp.call(args=["./a.out", input, output])


if __name__ == "__main__":
    main()