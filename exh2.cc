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

struct Production {
    vector<Upgrade> upgrades;
    vector<int> car_in_class;  // Number of cars in each class.
    vector<vector<bool>> classes;  // Matrix with row of classes and each column represents an upgrade.
};


Production read_input_file(ifstream& in)
{
    Production P;
    if (in.is_open()) {
        in >> C >> M >> K;
        
        P.upgrades = vector<Upgrade>(M);
        P.car_in_class = vector<int>(K);
        P.classes = vector<vector<bool>>(K, vector<bool>(M, false));

        for (int e = 0; e < M; e++)
            in >> P.upgrades[e].c;
        for (int e = 0; e < M; e++)
            in >> P.upgrades[e].n;

        for (int class_id = 0; class_id < K; ++class_id) {
            int aux;
            in >> aux >> P.car_in_class[class_id];
            for (int e = 0; e < M; ++e) {
                in >> aux;
                P.classes[class_id][e] = aux;
            }
        }
        in.close();
    } else {
        cout << "Couldn't read." << endl;
        exit(1);
    }
    return (P);
}


void write_output_file(const vector<int>& solution, ofstream& out, double duration)
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


int class_pen(const vector<int>& seq, int n, int c, int k)
{
    int pen = 0;
    int upg = 0;
    if (k == C-1) {  // complete sequence
        //
        for (int i = 0; i < n; i++)
            upg += seq[k-i];
        pen += max(upg - c, 0);
        
        upg = 0;
        
        // ventanas partidas
        for (int i = 0; i < n-1; i++)
        {
            upg += seq[k-i];
            pen += max(upg - c, 0);
        }
    } else {  // uncomplete sequence
        if (k >= n-1) {
            for (int i = 0; i < n; i++)
                upg += seq[k-i];
            pen += max(upg - c, 0);
        } else {
            for (int i = 0; i < n; i++)
                upg += seq[i];
            pen += max(upg - c, 0);            
        }
    }
    return (pen);
}


int sum_penalization(const vector<Upgrade>& upgrades, const matrix& ass_chain, int k)
{
    int total_pen = 0;
    for (int m = 0; m < M; m++) {
        int n_e = upgrades[m].n;
        int c_e = upgrades[m].c;
        total_pen += class_pen(ass_chain[m], n_e, c_e, k);
    }
    return total_pen;    
}


void exh_rec(const vector<Upgrade>& upgrades, vector<int>& car_in_class, 
                const vector<vector<bool>>& classes, matrix& ass_chain, vector<int>& curr_sol,
                vector<int>& solution, int k, int curr_pen, const string& output_file, clock_t start)
{
    if (curr_pen >= T)
        return;
    
    if (k == C) {
        if (curr_pen < T) {
            T = curr_pen;
            solution = curr_sol;

            clock_t end = clock() - start;
            double duration = ((double)end)/CLOCKS_PER_SEC;

            ofstream output(output_file, ofstream::out);
            write_output_file(solution, output, duration);
        }
    } else {
        for (int class_id = 0; class_id < K; class_id++) {
           if (car_in_class[class_id] > 0) {
                car_in_class[class_id]--;
                curr_sol[k] = class_id;  // Assign a car from class class_id to position k in the solution.
                for (int m = 0; m < M; m++)  // agregas las mejoras de la clase
                    ass_chain[m][k] = classes[class_id][m];

                int tmp = curr_pen;
                if (k < C-1)
                    curr_pen += sum_penalization(upgrades, ass_chain, k);
                else  // if (k == C-1)
                    curr_pen += sum_penalization(upgrades, ass_chain, k);

                exh_rec(upgrades, car_in_class, classes, ass_chain, curr_sol, solution, k+1, curr_pen, output_file, start);

                curr_pen = tmp;
                car_in_class[class_id]++;
                curr_sol[k] = -1;
                for (int m = 0; m < M; m++)  // quitar las mejoras de la clase
                    ass_chain[m][k] = -1;
           }
        }
    }
}


void exh(const vector<Upgrade>& upgrades, vector<int>& car_in_class,
        const vector<vector<bool>>& classes, const string& output_file)
{
    T = INT_MAX;
    vector<int> curr_sol(C, -1);
    vector<int> solution(C, -1);  //
    matrix ass_chain(M, vector<int>(C, -1));  // 

    clock_t start;
    start = clock();
    exh_rec(upgrades, car_in_class, classes, ass_chain, curr_sol, solution, 0, 0, output_file, start);
}


int main(int argc, const char *argv[])
{
    if (argc != 3) {
        cout << "Two arguments required: input and output file." << endl;
        exit(1);
    }
    ifstream input(argv[1],ifstream::in);
    
    Production P = read_input_file(input);
    exh(P.upgrades, P.car_in_class, P.classes, argv[2]);
    
    return (0);
}