#include <iostream>
#include <fstream>
#include <climits>
#include <vector>
#include <time.h>


using namespace std;


/* VARIABLE DEFINITION */
static int C, M, K;  // Number of cars, number of upgrades, number of classes
static int T;  // Total penalization.

using Vec = vector<int>;
using Class = vector<bool>;
using Matrix = vector<Vec>;

struct Upgrade {
    int n;  // Maximum number of cars per window.
    int c;  // Maximum number of cars that can be upgraded without penalization in every n(or lower)-sized window .
};

struct Production {
    vector<Upgrade> upgrades;  // Upgrades for the production.
    Vec car_in_class;  // Number of cars in each class.
    vector<Class> classes;  // Matrix with row of classes and each column represents an upgrade.
};
/* END OF VARIABLE DEFINITION */


// Returns the needed time to find a solution.
double duration(clock_t start)
{
    clock_t end = clock() - start;
    return (((double)end)/CLOCKS_PER_SEC);
}


// Reads the input file and returns the production attributes from that file.
Production read_input_file(ifstream& in)
{
    Production P;
    if (in.is_open()) {
        in >> C >> M >> K;
        
        P.upgrades = vector<Upgrade>(M);
        P.car_in_class = Vec(K);
        P.classes = vector<Class>(K, Class(M, false));

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


// Writes a solution and the time needed to find it in the output file.
void write_output_file(const Vec& solution, ofstream& out, double duration)
{
    out.setf(ios::fixed);
    out.precision(5);

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


// Computes and returns the penalization of adding the k'th element to the solution for an upgrade station (with atributes n and c).
// The sequence analized to compute this penalization is the row of the Assembly Chain corresponding to that upgrade.
int upgrade_pen(const Vec& seq, int n, int c, int k)
{
    int pen = 0;
    int upg = 0;
    if (k == C-1) {  // complete sequence
        for (int i = 0; i < n; i++) {
            upg += seq[k-i];
            pen += max(upg - c, 0);
        }
    } else {  // uncomplete sequence
        if (k >= n-1) {
            for (int i = 0; i < n; i++)
                upg += seq[k-i];
            pen += max(upg - c, 0);
        } else {
             for (int i = 0; i <= k; i++)
                upg += seq[i];
            pen += max(upg - c, 0);           
        }
    }
    return (pen);
}


// Computes and returns the total penalization of adding the k'th element to the current partial solution.
int sum_penalization(const vector<Upgrade>& upgrades, const Matrix& ass_chain, int k)
{
    int total_pen = 0;
    for (int m = 0; m < M; m++) {
        int n_e = upgrades[m].n;
        int c_e = upgrades[m].c;
        total_pen += upgrade_pen(ass_chain[m], n_e, c_e, k);
    }
    return (total_pen);    
}


// Updates the current solution and the Assembly Chain matrix by adding a new car of class_id and its upgrades.
void add_car_to_chain(Vec& car_in_class, const vector<Class>& classes, Matrix& ass_chain, Vec& curr_sol,
                        int class_id, int k)
{
    car_in_class[class_id]--;
    curr_sol[k] = class_id;  
    for (int m = 0; m < M; m++)
        ass_chain[m][k] = classes[class_id][m];
}


// Exhaustive search algorithm.
void exh_rec(const vector<Upgrade>& upgrades, Vec& car_in_class, const vector<Class>& classes,
            Matrix& ass_chain, Vec& curr_sol, Vec& solution, int k, int curr_pen,
            const string& output_file, clock_t start)
{
    if (curr_pen >= T)
        return;
    
    if (k == C) {
        T = curr_pen;
        solution = curr_sol;
        ofstream output(output_file, ofstream::out);
        write_output_file(solution, output, duration(start));
    } else { // if lower_bound // < T
        for (int class_id = 0; class_id < K; class_id++) {
            if (car_in_class[class_id] > 0) {
                add_car_to_chain(car_in_class, classes, ass_chain, curr_sol, class_id, k);
                // car_in_class[class_id]--;
                // curr_sol[k] = class_id;  
                // for (int m = 0; m < M; m++)
                //     ass_chain[m][k] = classes[class_id][m];

                // if (k < C-1)
                //     curr_pen += sum_penalization(upgrades, ass_chain, k);
                // else
                int tmp = curr_pen;
                curr_pen += sum_penalization(upgrades, ass_chain, k);

                exh_rec(upgrades, car_in_class, classes, ass_chain, curr_sol, solution, k+1, curr_pen, output_file, start);

                // restore assembly chain.
                curr_pen = tmp;
                car_in_class[class_id]++;
                // curr_sol[k] = -1;
                // for (int m = 0; m < M; m++)
                //     ass_chain[m][k] = -1;
            }
        }
    }
}


// Finds an optimal order of cars (solution), so that it minimizes the total penalization.
void exh(Production& P, const string& output_file)
{
    T = INT_MAX;
    Vec curr_sol(C, -1);
    Vec solution(C, -1); // order of production of the cars
    Matrix ass_chain(M, Vec(C, -1));  // assembly chain of cars and their upgrades.

    exh_rec(P.upgrades, P.car_in_class, P.classes, ass_chain, curr_sol, solution, 0, 0, output_file, clock());
}


int main(int argc, const char *argv[])
{
    if (argc != 3) {
        cout << "Two arguments required: input and output file." << endl;
        exit(1);
    }
    ifstream input(argv[1],ifstream::in);
    
    Production P = read_input_file(input);
    exh(P, argv[2]);
    
    return (0);
}