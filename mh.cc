#include <iostream>
#include <fstream>
#include <climits>
#include <queue>
#include <vector>
#include <time.h>


using namespace std;


/* VARIABLE DEFINITION */
static int C, M, K;  // Number of cars, number of upgrades, number of classes
static int T;  // Total penalization.

using Vec = vector<int>;
using Class = vector<bool>;
using Matrix = vector<Vec>;
using TabuList = queue<vector<int>>;

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
    return total_pen;    
}


//
bool is_in_list(const vector<int>& sol, TabuList tabu)
{
    while (not tabu.empty()) {
        if (sol == tabu.front())
            return true;
        tabu.pop();
    }
    return false;
}


// 
void fill_ass_chain_and_sum_pen(const vector<Upgrade>& upgrades, const Vec& solution, Matrix& ass_chain,
                                const vector<Class>& classes, int & pen)
{
    for(int k = 0; k < C; k++) {
        int class_id = solution[k];
        for (int m = 0; m < M; m++)
            ass_chain[m][k] = classes[class_id][m];
        pen += sum_penalization(upgrades, ass_chain, k);
    }
}


//
bool grasp(const vector<Upgrade>& upgrades, Vec car_in_class, const vector<Class>& classes,
            Vec& solution, Vec& curr_sol, Matrix ass_chain,
            const string& output_file, clock_t start)
{
    //ahora revisamos cada permutación de esta solución obtenida haciendo un swap entre dos elementos,
    //y si encontramos una solución estrictamente mejor repetiremos el proceso con esta.
    for (int i = 0; i < C; i++) {
        for (int j = 0; j < C; j++) { // j > i para no repetir cálculos
            int swap_var = curr_sol[i];
            curr_sol[i] = curr_sol[j];
            curr_sol[j] = swap_var;
            int new_pen = 0;
            fill_ass_chain_and_sum_pen(upgrades, curr_sol, ass_chain, classes, new_pen);
            if (new_pen < T) {
                cout << "mejoró" << endl;
                T = new_pen;
                solution = curr_sol;
                ofstream output(output_file, ofstream::out);
                write_output_file(solution, output, duration(start));
                return true;
            }
            curr_sol[j] = curr_sol[i];
            curr_sol[i] = swap_var;
        }               
    }
    return false; 
}


/***********************************************************************************************************/
// Computes the density of a class, i.e. the number of 1's (upgrades needed) in that class.
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


// Updates assembly chain by adding a new car of class_id and its upgrades.
void add_car_to_chain(Vec& car_in_class, const vector<Class>& classes, Matrix& ass_chain, Vec& curr_sol,
                        int class_id, int k)
{
    car_in_class[class_id]--;
    curr_sol[k] = class_id;  
    for (int m = 0; m < M; m++)
        ass_chain[m][k] = classes[class_id][m];
}


// Search the most requested class, in terms of cars per class.
int most_requested_class(Vec car_in_class)
{
    
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


// Finds the local minima of a set of neighbours, starting from a solution generated with a greedy algorithm.
void mh(Production& P, const string& output_file)
{
    T = INT_MAX;
    // Vec curr_sol(C, -1);
    Vec solution(C, -1);
    Matrix ass_chain(M, Vec(C, -1));  // assembly chain of cars and their upgrades.

    // Phase 1: solution construction
    greedy(P.upgrades, P.car_in_class, P.classes, ass_chain, solution, output_file, clock());
    // Phase 2: solution improvement (tabu search)
    // TabuList tabu;
    Vec curr_sol = solution;
    bool improved = true;
        // vecinos: de momento todas las permutaciones haciendo un swap entre dos elementos de la mejor solución actual.
    while(improved)
        improved = grasp(P.upgrades, P.car_in_class, P.classes, solution, curr_sol, ass_chain,
                            output_file, clock());
}


int main(int argc, const char *argv[])
{
    if (argc != 3) {
        cout << "Two arguments required: input and output file." << endl;
        exit(1);
    }
    ifstream input(argv[1],ifstream::in);
    
    Production P = read_input_file(input);
    mh(P, argv[2]);
    
    return (0);
}