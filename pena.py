import os


mode = "easy" + "-"
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
    print(f"Total penalization for the {mode}mode is: {total_T} with a medium time of: {sum(time)/len(time)} seconds")
    print(f"Time for each bench: {time}")

if __name__ == "__main__":
    main()