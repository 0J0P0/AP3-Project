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


// Computes density of a class, i.e. the number of 1's in the class.
int density_class(const vector<vector<bool>>& classes, int class_id)
{
    int density = 0;
    for (int i = 0; i < (int)classes[class_id].size(); i++)
        if (classes[class_id][i])
            density++;
    return (density);
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


// 
int min_penalization_class(const vector<Upgrade>& upgrades, const vector<int>& car_in_class,
                            const vector<vector<bool>>& classes, const vector<bool>& vis,
                            matrix& ass_chain, /*const vector<int>& solution,*/ int k)
{
    int best_class = -1;
    int new_pen = INT_MAX;
    for (int class_id = 0; class_id < K; class_id++) {
        if (not vis[class_id] and car_in_class[class_id] > 0 /*and solution[k] != class_id*/) {
            for (int m = 0; m < M; m++)
                ass_chain[m][k] = classes[class_id][m];  // se agrega pero no se deberia quitar

            int class_pen = sum_penalization(upgrades, ass_chain, k);
            if (class_pen < new_pen) {  // criterio de penalizacion añadiendo class_id
                new_pen = class_pen;
                best_class = class_id;
            } else if (class_pen == new_pen) { // criterio de numero de coches
                if (car_in_class[best_class] < car_in_class[class_id]) {
                    best_class = class_id;
                    new_pen = class_pen;
                } else if (car_in_class[best_class] == car_in_class[class_id]) {
                        int min_dens = density_class(classes, best_class);
                        int class_dens = density_class(classes, class_id);
                        if (min_dens < class_dens) {
                            best_class = class_id;
                            new_pen = class_pen;
                        }
                }
            }
        }
    }
    return (best_class);
}


// Exhaustive search algorithm.
void exh_rec(const vector<Upgrade>& upgrades, vector<int>& car_in_class, const vector<vector<bool>>& classes,
            matrix& ass_chain, vector<int>& curr_sol, vector<int>& solution, int k, int curr_pen,
            const string& output_file, clock_t start)
{
    if (curr_pen >= T /*or (clock() - start)/(double)CLOCKS_PER_SEC > 60*/) // solucion cutre para que no dure mucho
        return;

    if (k == C) {
        cout << "nueva sol: ";
        if (curr_pen < T) {  // este no esta de mas si esta el if de arriba?
            cout << "escribe sol" << endl;
            T = curr_pen;
            solution = curr_sol;
            clock_t end = clock() - start;
            double duration = ((double)end)/CLOCKS_PER_SEC;
            ofstream output(output_file, ofstream::out);
            write_output_file(solution, output, duration);
        }
    } else {
        vector<bool> vis(K, false);
        for (int i = 0; i < K; i++) {
            // curr_pen menos que pen de la solution
            int class_id = min_penalization_class(upgrades, car_in_class, classes, vis, ass_chain, /*solution,*/ k);
            if (class_id == -1)
                return;
            car_in_class[class_id]--;
            vis[class_id] = true;
        
            // update assembly chain with a new car of class_id and its upgrades.
            curr_sol[k] = class_id;  
            for (int m = 0; m < M; m++)
                ass_chain[m][k] = classes[class_id][m];

            // update penalization.
            int tmp = curr_pen;
            if (k < C-1)  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                curr_pen += sum_penalization(upgrades, ass_chain, k);
            else  // if (k == C-1)
                curr_pen += sum_penalization(upgrades, ass_chain, k);


            // cout << "Rec level: " << k << " iteration: " << i << endl;
            //   cout << "Current solution";
            // for (auto e : curr_sol)
            //     cout << " " << e;
            // cout << " |||| Solution";
            // for (auto e : solution)
            //     cout << " " << e;
            // cout << endl << "------------------------------------------------------------------------" << endl;

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


/******************************************************************************************************/
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
            vector<int>& solution, matrix& ass_chain, const string& output_file, clock_t start)
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


// Finds an optimal order of cars, so that it minimizes the total penalization.
void exh(Production& P, const string& output_file)
{
    T = INT_MAX;
    vector<int> curr_sol(C, -1);
    vector<int> solution(C, -1);
    matrix ass_chain(M, vector<int>(C, -1));  // assembly chain of cars and their upgrades.

    clock_t start;
    start = clock();
    greedy(P.upgrades, P.car_in_class, P.classes, solution, ass_chain, output_file, start);

    exh_rec(P.upgrades, P.car_in_class, P.classes, ass_chain, curr_sol, solution, 0, 0, output_file, start);
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