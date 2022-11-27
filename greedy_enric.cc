#include <iostream>
#include <fstream>
#include <climits>
#include <vector>
#include <time.h>


using namespace std;
using matrix = vector<vector<int>>;

/* VARIABLE DEFINITION */
static int C, M, K, T;

struct Upgrade {
    int n;  // maximum number of cars per window.
    int c;  // maximum number of cars for upgrade.
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

int find_max_class(vector<int> car_in_class){
    
    int max = 0;
    int max_class = 0;
    for (int class_id = 0; class_id < K; class_id++){
            if(car_in_class[class_id] > max){
                max = car_in_class[class_id];
                max_class = class_id;
            }
        }
    return max_class;
}
// Finds a semi-optimal solution as fast as possible.
void greedy(const vector<Upgrade>& upgrades, vector<int>& car_in_class,
        const vector<vector<bool>>& classes, const string& output_file)
{
    T = INT_MAX;
    vector<int> solution(C, -1);
    matrix ass_chain(M, vector<int>(C, -1));  // assembly chain of cars and their upgrades.

    clock_t start;
    start = clock();
    int max_class = find_max_class(car_in_class); // we look for the most requested class, since the first upgrade won't have a penalization
    car_in_class[max_class]--;
    solution[0] = max_class;
    for (int m = 0; m < M; m++)
                    ass_chain[m][0] = classes[max_class][m];
    int k = 1;
    int curr_pen = 0;
    while(k < C){
        int min_add = T;
        int min_class = 0;
        for (int class_id = 0; class_id < K; class_id++){
            if(car_in_class[class_id] > 0){
                for (int m = 0; m < M; m++) ass_chain[m][k] = classes[class_id][m];
                int class_pen = sum_penalization(upgrades, ass_chain, k);
                if(class_pen < min_add){ 
                    min_add = class_pen;
                    min_class = class_id;
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