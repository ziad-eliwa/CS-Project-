#include <iostream>
#include <sodium.h>
using namespace std;


int main() {


    if (sodium_init() == -1) {
        return 1;
    }

    



    return 0;
}