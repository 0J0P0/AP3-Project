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
    int c;  // Maximum number of cars for upgrade.
};

struct Production {
    vector<Upgrade> upgrades;  // Upgrades for the production.
    Vec car_in_class;  // Number of cars in each class.
    vector<Class> classes;  // Matrix with row of classes and each column represents an upgrade.
};
/* END VARIABLE DEFINITION */


// Time to find an optimal solution.
double duration(clock_t start)
{
    clock_t end = clock() - start;
    return (((double)end)/CLOCKS_PER_SEC);
}


// Read the input file. Returns the attributes of the production from the input file.
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


// Write a optimal solution to the output file.
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


// Computes and returns the penalization of a class.
int class_pen(const Vec& seq, int n, int c, int k)
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


// Computes and returns the penalization for all the classses.
int sum_penalization(const vector<Upgrade>& upgrades, const Matrix& ass_chain, int k)
{
    int total_pen = 0;
    for (int m = 0; m < M; m++) {
        int n_e = upgrades[m].n;
        int c_e = upgrades[m].c;
        total_pen += class_pen(ass_chain[m], n_e, c_e, k);
    }
    return (total_pen);    
}


// Updates assembly chain by adding a new car of class_id and its upgrades.
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

                int tmp = curr_pen;
                curr_pen += sum_penalization(upgrades, ass_chain, k);

                exh_rec(upgrades, car_in_class, classes, ass_chain, curr_sol, solution, k+1, curr_pen, output_file, start);

                car_in_class[class_id]++;
                curr_pen = tmp;
            }
        }
    }
}


/***********************************************************************************************************/
// Search the most requested class, in terms of cars per class.
int most_requested_class(Vec car_in_class){
    
    int nc = 0;
    int best_class = -1;
    for (int class_id = 0; class_id < K; class_id++) {
            if (car_in_class[class_id] > nc) {
                nc = car_in_class[class_id];
                best_class = class_id;
            }
        }
    return (best_class);
}


// Computes density of a class, i.e. the number of 1's in the class.
int density_class(const vector<Class>& classes, int class_id)
{
    int density = 0;
    for (int i = 0; i < (int)classes[class_id].size(); i++)
        if (classes[class_id][i])
            density++;
    return (density);
}


pair<int, int> min_penalization_class(const vector<Upgrade>& upgrades, const Vec& car_in_class,
                                        const vector<Class>& classes, Matrix ass_chain, int k)
{
    int tmp = INT_MAX; // T
    int best_class = -1;

    for (int class_id = 0; class_id < K; class_id++) {
        if (car_in_class[class_id] > 0) {
            for (int m = 0; m < M; m++)
                ass_chain[m][k] = classes[class_id][m];

            int class_pen = sum_penalization(upgrades, ass_chain, k);

            if (class_pen < tmp) {  // criterio de penalizacion añadiendo class_id
                tmp = class_pen;
                best_class = class_id;
            } else if (class_pen == tmp) { // criterio de numero de coches
                if (car_in_class[best_class] < car_in_class[class_id]) {
                    best_class = class_id;
                    tmp = class_pen;
                } else if (car_in_class[best_class] == car_in_class[class_id]) {
                        if (density_class(classes, best_class) < density_class(classes, class_id)) {
                            best_class = class_id;
                            tmp = class_pen;
                        }
                }
            }
        }
    }
    return {best_class, tmp};
}


// Finds a semi-optimal solution as fast as possible.
void greedy(const vector<Upgrade>& upgrades, Vec car_in_class, const vector<Class>& classes,
            Matrix ass_chain, Vec& solution, const string& output_file, clock_t start)
{
    int curr_pen = 0;
    // base case: choose the class with the most number of cars since there is no penalization.
    int class_id = most_requested_class(car_in_class);
    add_car_to_chain(car_in_class, classes, ass_chain, solution, class_id, 0);

    int k = 1;
    while (k < C) {
        pair<int, int> min_class = min_penalization_class(upgrades, car_in_class, classes, ass_chain, k);
        if (min_class.first != -1) {
            add_car_to_chain(car_in_class, classes, ass_chain, solution, min_class.first, k);
            curr_pen += min_class.second;
        }
        k++;
    }
    T = curr_pen;
    ofstream output(output_file, ofstream::out);
    write_output_file(solution, output, duration(start));
}
/***********************************************************************************************************/


// Finds an optimal order of cars, so that it minimizes the total penalization.
void exh(Production& P, const string& output_file)
{
    T = INT_MAX;
    Vec curr_sol(C, -1);
    Vec solution(C, -1);
    Matrix ass_chain(M, Vec(C, -1));  // assembly chain of cars and their upgrades.

    // greedy(P.upgrades, P.car_in_class, P.classes, ass_chain, solution, output_file, clock());
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