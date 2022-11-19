#include<iostream>
#include<vector>


using namespace std;


int sum_upgrade_pen(const vector<int>& seq, int n, int c)
{
    int i = 1;  // para comparar con n
    int pen = 0;
    int upgraded = 0; // comparar con c

    for (int j = 0; j < (int)seq.size(); j++)
    {
        if (seq[j] == 1)
            ++upgraded;
        if (i > n and j < (int)seq.size() - n + 1)
        {
            upgraded -= seq[j-n];
        }
        if (j > (int)seq.size() - n + 1)
            upgraded -= seq[j-n];
        pen += max(upgraded - c, 0);
        cout << i << " " << j << " upgraded: " << upgraded << " pen: " << pen << endl;
        ++i;
    }
    upgraded = 0;
    for (int j = (int)seq.size()-1; j > (int)seq.size()-n; j--)
    {
        if (seq[j] == 1)
            ++upgraded;
        pen += max(upgraded - c, 0);
    }
    return (pen);
}

// Mejora
// int sum_upgrade_pen(const vector<int>& seq, int n, int c)
// {
//     int pen = 0;
//     int upgr = 0;
//     int s = seq.size();

//     for(int j = 0; j < n; j++){
//         upgr += seq[j];
//         pen += max(upgr-c, 0);
//     }

//     for(int j = n; j < s; j++){
//         upgr+= seq[j] - seq[j-n];
//         pen += max(upgr-c, 0);
//     }
//     upgr = 0;
//     for(int j = s-1; j > s-n; j--){
//         upgr += seq[j];
//         pen += max(upgr-c, 0);
//     }
//     return pen;
// }

int main()
{
    printf("%d\n", sum_upgrade_pen({0,0,1,0,1,0,0,1,1,1,0}, 3, 1));
    printf("%d\n", sum_upgrade_pen({1,1,0,0,1,0,1,1}, 3, 1));
}