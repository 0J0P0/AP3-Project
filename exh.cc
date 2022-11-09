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

vector<int> solution;
vector<Upgrade> upgrades;
vector<int> car_in_class;
vector<vector<bool>> classes;

void read_input_file(ifstream& in)
{
    if (in.is_open()) {
        in >> C >> M >> K;

        upgrades = vector<Upgrade>(M);
        classes = vector<vector<bool>>(K, vector<bool>(M, false));  // Matrix with row of classes and each column represents an upgrade.
        car_in_class = vector<int>(K);  // Number of cars in each class.

        for (int e = 0; e < M; e++)
            in >> upgrades[e].c;
        for (int e = 0; e < M; e++)
            in >> upgrades[e].n;

        for (int class_id = 0; class_id < K; ++class_id) {
            int aux;
            in >> aux >> car_in_class[class_id];
            for (int e = 0; e < M; ++e) {
                in >> aux;
                classes[class_id][e] = aux;
            }
        }
        in.close();  //close here or back at main?
    } else {
        cout << "Couldn't read." << endl;
        exit(1);
    }
}

void write_output_file(ofstream& out, double duration)
{
    out.setf(ios::fixed);
    out.precision(1);

    int s = solution.size();
    if (out.is_open())
    {
        out << T << " " << duration << endl;
        out << solution[0];
        for (int i = 1; i < s; i++)
            out << " " << solution[i];
        out.close();
    } else {
        cout << "Couldn't write." << endl;
        exit(1);
    }
}

int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        cout << "Two arguments required: input and output file." << endl;
        exit(1);
    }
    ifstream input_file(argv[1],ifstream::in);
    read_input_file(input_file);

    T = 0;
    solution = vector<int>(C, -1);
    auto start = high_resolution_clock::now();  // Get starting timepoint

    exh()

    auto end = high_resolution_clock::now();  // Get ending timepoint
    auto duration = duration_cast<seconds>(end - start);

    ofstream output_file(argv[2],ofstream::out);
    write_output_file(output_file, (double)duration.count());
}