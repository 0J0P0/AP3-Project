#include<iostream>
#include<vector>

using namespace std;
using matrix = vector<vector<int>>;

int C = 10;
int M = 5;

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


cout << "Solution" << endl;
for (auto e : solution)
    cout << e << " ";
cout << endl << "Ass chain" << endl;

for (int i = 0; i < M; i++)
{
    for (int j = 0; j < C; j++)
        cout << ass_chain[i][j] << " ";
    cout << endl;
}
cout << endl;

int main()
{

    int T = sum_penalization(upgrades, ass_chain, C-1);
    cout << T << endl;
}