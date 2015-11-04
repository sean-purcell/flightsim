#include <iostream>
#include "angler.hpp"
#include "angler.cpp"

using namespace std;

int main()
{
    check_asserts();
    int x, y, z;
    while (1)
    {
        cin >> x >> y >> z;
        if (x == y && y == z && z == 0)
            break;
        cout << "Change in pitch: " << pitcher(x, y, z) << endl;
        cout << "Change in roll: " << roller(x, y, z) << endl;
        
    }
    return 0;
}
