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

int main()
{
    printf("%d\n", sum_upgrade_pen({0,0,1,0,1,0,0,1,1,1,0}, 3, 1));
    printf("%d\n", sum_upgrade_pen({1,1,0,0,1,0,1,1}, 3, 1));
}