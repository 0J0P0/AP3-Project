// crono start and end
// tiempo de ejecucion

#include <iostream>
#include <fstream>
#include <climits>
#include <vector>
#include <time.h>


using namespace std;
using matrix = vector<vector<int>>;


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


// Computes and returns the penalization of a class.
int class_pen(const vector<int>& seq, int n, int c, int k)
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


/*vector<vector<int>> permutation(vector<int> seq, int k)
{
    int n = seq.size();
    vector<vector<int> perm;
    for (int i = 0; i < k; i++)
    {
        vector<int> tmp = seq;
        vector<int> index(n, 0);
        for (int j = 0; j < n; j++)
        {
            int aux = tmp[j];
            for (int l = 0; l < n and l != i; l++)
            {
                if (not index[l])
                    swap(aux, tmp[l]);
                index[l] = 1;
            }
            
        }
    }
    
    
}*/

/******************************************************************************************************/
// Computes density of a class, i.e. the number of 1's in the class.
int density_class(const vector<vector<bool>>& classes, int class_id)
{
    int density = 0;
    for (int i = 0; i < (int)classes[class_id].size(); i++)
        if (classes[class_id][i])
            density++;
    return (density);
}


// Searchs the most requested class, in terms of cars per class.
int find_max_class(vector<int> car_in_class){
    
    int max_class = 0;
    int argmax_class = 0;
    for (int class_id = 0; class_id < K; class_id++) {
            if(car_in_class[class_id] > max_class) {
                max_class = car_in_class[class_id];
                argmax_class = class_id;
            }
        }
    return (argmax_class);
}


// Finds a semi-optimal solution as fast as possible.
void greedy(const vector<Upgrade>& upgrades, vector<int> car_in_class, const vector<vector<bool>>& classes,
            vector<int>& solution, matrix ass_chain, const string& output_file, clock_t start)
{
    int max_class = find_max_class(car_in_class); // we look for the most requested class, since the first upgrade won't have a penalization
    car_in_class[max_class]--;
    solution[0] = max_class;
    for (int m = 0; m < M; m++)
        ass_chain[m][0] = classes[max_class][m];
    int k = 1;
    int curr_pen = 0;
    while(k < C) {
        int min_add = T;
        int min_class = 0;
        for (int class_id = 0; class_id < K; class_id++){
            if(car_in_class[class_id] > 0) {
                for (int m = 0; m < M; m++)
                    ass_chain[m][k] = classes[class_id][m];
                int class_pen = sum_penalization(upgrades, ass_chain, k);
                if (class_pen < min_add) {
                    min_add = class_pen;
                    min_class = class_id;
                } else if (class_pen == min_add) { // criterio de numero de coches
                    if (car_in_class[min_class] < car_in_class[class_id]) {
                        min_class = class_id;
                        min_add = class_pen;
                    } else if (car_in_class[min_class] == car_in_class[class_id]) {
                          int min_dens = density_class(classes, min_class);
                          int class_dens = density_class(classes, class_id);
                          if (min_dens < class_dens) {
                              min_class = class_id;
                              min_add = class_pen;
                          }
                    }
                }
            }
        }
        for (int m = 0; m < M; m++) ass_chain[m][k] = classes[min_class][m];
        curr_pen += min_add;
        solution[k] = min_class;
        car_in_class[min_class]--;
        k++;
    }
    
    T = curr_pen;
    clock_t end = clock() - start;
    double duration = ((double)end)/CLOCKS_PER_SEC;

    ofstream output(output_file, ofstream::out);
    write_output_file(solution, output, duration);
}
/******************************************************************************************************/

void fill_ass_chain_and_sum_pen(const vector<Upgrade>& upgrades, const vector<int>& solution, matrix& ass_chain,  const vector<vector<bool>>& classes,
 int & pen){

    int n = solution.size();
    for(int k = 0; k < n; k++){
        int class_id = solution[k];  
        for (int m = 0; m < M; m++)
            ass_chain[m][k] = classes[class_id][m];
        pen += sum_penalization(upgrades, ass_chain, k);
    }
}

void grasp(const vector<Upgrade>& upgrades, vector<int> car_in_class, const vector<vector<bool>>& classes,
            vector<int>& solution, vector<int>& curr_sol, matrix ass_chain, const string& output_file, clock_t start, bool & no_better, int swaps){
        int n = solution.size();
        //ahora revisamos cada permutación de esta solución obtenida haciendo un swap entre dos elementos,
        //y si encontramos una solución estrictamente mejor repetiremos el proceso con esta.

        for(int i = 0; i < n; i++){
            for(int j = 0;j < n; j++){ // j > i para no repetir cálculos
                int swap_var = curr_sol[i];
                curr_sol[i] = curr_sol[j];
                curr_sol[j] = swap_var;
                int new_pen = 0;
                fill_ass_chain_and_sum_pen(upgrades, curr_sol, ass_chain, classes, new_pen);
                if(new_pen < T){
                    cout << "mejoró" << endl;
                    solution = curr_sol;
                    T = new_pen;
                    clock_t end = clock() - start;
                    double duration = ((double)end)/CLOCKS_PER_SEC;
                    ofstream output(output_file, ofstream::out);
                    write_output_file(solution, output, duration);
                    no_better = false;
                    return;
                }
                /*else if(swaps == 3) return; // los vecinos son aquellas secuencias que se obtienen haciendo como mucho tres swaps sobre la solución actual.
                else{
                    swaps++;
                    grasp(upgrades, car_in_class, classes, solution, curr_sol, ass_chain, output_file, start, no_better, swaps);
                }*/
                curr_sol[j] = curr_sol[i];
                curr_sol[i] = swap_var;
                }               
            }
            no_better = true; 
    }
       

// Finds the local minima of a set of neighbours, starting from a solution generated with a greedy algorithm.
void mh(Production& P, const string& output_file)
{
    T = INT_MAX;
    vector<int> curr_sol(C, -1);
    vector<int> solution(C, -1);
    matrix ass_chain(M, vector<int>(C, -1));  // assembly chain of cars and their upgrades.

    clock_t start;
    start = clock();
    greedy(P.upgrades, P.car_in_class, P.classes, solution, ass_chain, output_file, start);
        // vecinos: de momento todas las permutaciones haciendo un swap entre dos elementos de la mejor solución actual.

        // buscar numero de iteraciones optima para vaciar pila
        // guardar T y vector de solution
        // si la T acutal y T son iguales mirar en el tabu por el vector comprarndo con el resto (esto de momento no)
    bool no_better = false;
    while(not no_better){
        curr_sol = solution;
        grasp(P.upgrades, P.car_in_class, P.classes, solution, curr_sol, ass_chain, output_file, start, no_better, 0);
    }
    
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