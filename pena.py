import os
import sys

"""
Programmm that reads all the benchs from a specific mode and computes it's total
penalization and the medium time.
"""

mode = str(sys.argv[1]) + "-"
data_path: str = "./benchs_results/"


def main():
    time = []
    total_T = 0
    for i in range(1, 11):
        bench = mode + str(i) + ".txt"
        with open(os.path.join(data_path, bench), 'r') as data_file:
            line = data_file.readline()
            nums = line.split()
            total_T += int(nums[0])
            time.append(float(nums[1]))
    print(f"\ncTotal penalization for the {mode}mode is: {total_T} with a medium time of: {sum(time)/len(time)} seconds")
    print(f"\nTime for each bench: {time}\n")

if __name__ == "__main__":
    main()