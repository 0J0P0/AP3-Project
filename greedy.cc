// crono start and end
// tiempo de ejecucion

#include <algorithm>
#include <iostream>
#include <fstream>
#include <climits>
#include <vector>
#include <time.h>


using namespace std;
using Matrix = vector<vector<int>>;


/* VARIABLE DEFINITION */
static int C, M, K;  // Number of cars, number of upgrades, number of classes
static int T;  // Total penalization.

struct Upgrade {
    int n;  // Maximum number of cars per window.
    int c;  // Maximum number of cars for upgrade.
};

struct Production {
    vector<Upgrade> upgrades;  // Upgrades for the production.
    vector<int> car_in_class;  // Number of cars in each class.
    vector<vector<bool>> classes;  // Matrix with row of classes and each column represents an upgrade.
};
/* END VARIABLE DEFINITION */


bool density(const pair<int, double>& d1, const pair<int, double>& d2)
{
    return d1.second > d2.second;
}


bool sort_upgrades(const Upgrade& u1, const Upgrade& u2)
{
    return (u1.c > u2.c);
}


vector<pair<int, double>> density_class(const vector<vector<bool>>& classes, const vector<int>& car_in_class)
{
    vector<pair<int, double>> density_c(K, {-1, -1});  // class id and density.
    for (int k = 0; k < K; k++)
    {
        double dens = 0;
        for (auto m : classes[k])
            dens += m;
        density_c[k] = {k, dens};
    }
    return (density_c);
}

// Read the input file. Returns the attributes of the production from the input file.
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


// Write a optimal solution to the output file.
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


int sum_penalization(const vector<Upgrade>& upgrades, const matrix& ass_chain, bool complete_seq)
{
    int total_pen = 0;
    for (int m = 0; m < M; m++) {
        int n_e = upgrades[m].n;
        int c_e = upgrades[m].c;
        total_pen += upgrade_pen(ass_chain[m], n_e, c_e, complete_seq);
    }
    return total_pen;    
}


// Finds an optimal order of cars, so that it minimizes the total penalization.
void greedy(const vector<Upgrade>& upgrades, vector<int>& car_in_class,
        const vector<vector<bool>>& classes, const string& output_file)
{
    vector<int> solution(C, -1);
    matrix ass_chain(M, vector<int>(C, -1));  // assembly chain of cars and their upgrades.

    clock_t start;
    start = clock();

    vector<pair<int, double>> density_c = density_class(classes, car_in_class);  // class id and density.
    vector<Upgrade> upgrades_sorted = upgrades;

    sort(density_c.begin(), density_c.end(), density);
    sort(upgrades_sorted.begin(), upgrades_sorted.end(), sort_upgrades);

    int jump = upgrades_sorted[0].c;
    for (int d = 0; d < (int)density_c.size(); d++) {
        int j = 0;
        int nc = car_in_class[density_c[d].first];

        bool put = true;
        while (nc) {
            int m = 0;
            while (m < M) {
                if (ass_chain[m][j] != -1) { // crear vis
                    ++j;
                    put = false;
                    break;
                } else {
                    put = true;
                    solution[j] = density_c[d].first;
                    ass_chain[m][j] = classes[density_c[d].first][m];
                    ++m;
                }
            }
            if (put) {
                --nc;
                j += jump;
                if (j >= C)
                    j %= C;
            }
        }
    }

    T = sum_penalization(upgrades, ass_chain, true);
    
    clock_t end = clock() - start;
    double duration = ((double)end)/CLOCKS_PER_SEC; 

    ofstream output(output_file, ofstream::out);
    write_output_file(solution, output, duration);
}


int main(int argc, const char *argv[])
{
    if (argc != 3) {
        cout << "Two arguments required: input and output file." << endl;
        exit(1);
    }
    ifstream input(argv[1],ifstream::in);
    
    Production P = read_input_file(input);
    greedy(P.upgrades, P.car_in_class, P.classes, argv[2]);
    
    return (0);
}