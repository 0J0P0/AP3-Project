#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>


using namespace std;
using namespace std::chrono;


static int C, M, K, T;

struct Upgrade {
    int n;
    int c;
};

vector<Upgrade> upgrades;
vector<int> car_in_class;
vector<vector<bool>> Classes;

void read_input_file(const string& input_file)
{
    ifstream f(input_file);
    if (f.is_open()) {
        f >> C >> M >> K;
        upgrades = vector<Upgrade>(M);
        Classes = vector<vector<bool>>(K,vector<bool>(M,false));  // Matrix with row of classes and each column represents an upgrade.
        car_in_class = vector<int>(K);  // Number of cars in each class.
        for (int e = 0; e < M; e++)
            f >> upgrades[e].c;
        for (int e = 0; e < M; e++)
            f >> upgrades[e].n;

        for (int class_id = 0; class_id < K; ++class_id) {
            int aux;
            f >> aux >> car_in_class[class_id];
            for (int o = 0; o < M; ++o) {
                f >> aux;
                Classes[class_id][o] = aux;
            }
        }
    } else
        cout << "Couldn't open " << input_file << '.' << endl;
    f.close();
}


int main(int argc, const char *argv[])
{
    cout.setf(ios::fixed);
    cout.precision(1);

    if (argc != 3)
    {
        cout << "Two arguments required: input and output file." << endl;
        exit(1);
    }
    read_input_file(argv[1]);
    
    // Get starting timepoint
    auto start = high_resolution_clock::now();
    //...
    //...

    //...
    //...
    // Get ending timepoint
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    cout << "Time taken by function: " << (double)duration.count() << " seconds" << endl;
    // write_output_file(argv[2], ... , (double)duration.count());
}