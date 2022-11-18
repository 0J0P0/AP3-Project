// Se pueden usar variables globales?

#include <iostream>
#include <fstream>
#include <climits>
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
vector<int> penalization;
vector<Upgrade> upgrades;
vector<int> car_in_class;  // Number of cars in each class.
vector<vector<bool>> classes;  // Matrix with row of classes and each column represents an upgrade.


void read_input_file(ifstream& in)
{
    if (in.is_open()) {
        in >> C >> M >> K;

        upgrades = vector<Upgrade>(M);
        car_in_class = vector<int>(K);
        classes = vector<vector<bool>>(K, vector<bool>(M, false));

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

    if (out.is_open())
    {
        out << T << " " << duration << endl;
        out << solution[0];
        for (int i = 1; i < C; i++)
            out << " " << solution[i];
        out.close();
    } else {
        cout << "Couldn't write." << endl;
        exit(1);
    }
}


int sum_upgrade_pen(const vector<int>& seq, int n, int c)
{
    int i = 1;  // para comparar con n
    int pen = 0;
    int upgraded = 0; // comparar con c

    for (int j = 0; j < (int)seq.size(); j++)
    {
        if (seq[j] == 1)
            ++upgraded;
        if (i > n and j < (int)seq.size() - n + 1)
            upgraded -= seq[j-n];

        if (j > (int)seq.size() - n + 1)
            upgraded -= seq[j-n];
        pen += max(upgraded - c, 0);
        ++i;
    }
    upgraded = 0;
    for (int j = (int)seq.size()-1; j > (int)seq.size()-n; j--)
    {
        if (seq[j] == 1)
            ++upgraded;
        pen += max(upgraded - c, 0);
    }
    return (pen);
}


int sum_penalizations(const vector<vector<int>>& ass_chain)
{
    int total_pen = 0;
    for (int m = 0; m < M; m++)
    {
        int n_e = upgrades[m].n;
        int c_e = upgrades[m].c;
        total_pen += sum_upgrade_pen(ass_chain[m], n_e, c_e);
    }
    return total_pen;    
}


void exh_rec(int k, vector<vector<int>>& ass_chain, const string& output_file)
{
    if (k == C)
    {
        int curr_pen = sum_penalizations(ass_chain);
        if (curr_pen < T) T = curr_pen;

        ofstream output(output_file, ofstream::out);
        write_output_file(output, 0.0);
    } else {
        for (int class_id = 0; class_id < K; class_id++)
        {
           if (car_in_class[class_id] > 0)
           {
                car_in_class[class_id]--;
                solution[k] = class_id;  // agregas la clase del coche a insertar
                for (int m = 0; m < M; m++)  // agregas las mejoras de la clase
                {
                    ass_chain[m][k] = classes[class_id][m];
                }

                exh_rec(k + 1, ass_chain, output_file);

                car_in_class[class_id]++;
                solution[k] = -1;
                for (int m = 0; m < M; m++)  // quitar las mejoras de la clase
                {
                    ass_chain[m][k] = -1;
                }
           }
        }
    }
}


void exh(const string& output_file)
{
    T = INT_MAX;
    solution = vector<int>(C, -1);
    vector<vector<int>> ass_chain(M, vector<int>(C, -1));

    exh_rec(0 ,ass_chain, output_file);
}


int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        cout << "Two arguments required: input and output file." << endl;
        exit(1);
    }
    ifstream input(argv[1],ifstream::in);
    read_input_file(input);
    exh(argv[2]);
    return (0);
}