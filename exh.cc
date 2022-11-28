// Se pueden usar variables globales?
// crono start and end
// tiempo de ejecucion

#include <iostream>
#include <fstream>
#include <climits>
#include <vector>
#include <time.h>

using namespace std;
using matrix = vector<vector<int>>;


static int C, M, K, T;

struct Upgrade {
    int n;
    int c;
};

vector<int> solution;
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

    if (out.is_open()) {
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


int upgrade_pen(const vector<int>& seq, int n, int c, bool complete_seq)
{
    int pen = 0;
    int upgr = 0;
    int size = seq.size();
    
    for(int j = 0; j < n; j++) {
        upgr += seq[j];
        pen += max(upgr-c, 0);
    }
    for(int j = n; j < size; j++) {
        upgr+= seq[j] - seq[j-n];
        pen += max(upgr-c, 0);
    }
    upgr = 0;
    if (complete_seq) {
        for(int j = size-1; j > size-n; j--) {
            upgr += seq[j];
            pen += max(upgr-c, 0);
        }
    }
    return (pen);
}


int sum_penalizations(const matrix& ass_chain, bool complete_seq)
{
    int total_pen = 0;
    for (int m = 0; m < M; m++) {
        int n_e = upgrades[m].n;
        int c_e = upgrades[m].c;
        total_pen += upgrade_pen(ass_chain[m], n_e, c_e, complete_seq);
    }
    return total_pen;    
}


void exh_rec(matrix& ass_chain, vector<int>& curr_sol, int k, const string& output_file, clock_t start)
{
    int curr_pen = sum_penalizations(ass_chain, false);
    if (curr_pen >= T)
        return;
    if (k == C) {
        int curr_pen = sum_penalizations(ass_chain, true);
        if (curr_pen < T) {
            T = curr_pen;
            solution = curr_sol;

            clock_t end = clock() - start;
            double duration = ((double)end)/CLOCKS_PER_SEC;

            ofstream output(output_file, ofstream::out);
            write_output_file(output, duration);
        }
    } else {
        for (int class_id = 0; class_id < K; class_id++) {
           if (car_in_class[class_id] > 0) {
                car_in_class[class_id]--;
                curr_sol[k] = class_id;  // Assign a car from class class_id to position k in the solution.
                for (int m = 0; m < M; m++)  // agregas las mejoras de la clase
                    ass_chain[m][k] = classes[class_id][m];

                exh_rec(ass_chain, curr_sol, k+1, output_file, start);

                car_in_class[class_id]++;
                curr_sol[k] = -1;
                for (int m = 0; m < M; m++)  // quitar las mejoras de la clase
                    ass_chain[m][k] = -1;
           }
        }
    }
}


void exh(const string& output_file)
{
    T = INT_MAX;
    vector<int> curr_sol(C, -1);
    solution = vector<int>(C, -1);
    matrix ass_chain(M, vector<int>(C, -1));

    clock_t start;
    start = clock();
    exh_rec(ass_chain, curr_sol, 0, output_file, start);
}


int main(int argc, const char *argv[])
{
    if (argc != 3) {
        cout << "Two arguments required: input and output file." << endl;
        exit(1);
    }
    ifstream input(argv[1],ifstream::in);
    read_input_file(input);
    exh(argv[2]); // quitar el parametro de exh
    return (0);
}