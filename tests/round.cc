#include "all.h"

#define max_hold 100

int main() {

    ll reps = 5;
    ll hold = 5;
    
    for (int i = 0; i < reps; i++) {
        cout << "rep " << i  << endl;
        long long start = time(NULL);
        long long elapsed = 0;
        long long last = 0;
        bool block = false;
        
        while(elapsed < max_hold) {
            if (elapsed >= hold) {
                if (!block) {
                    cout << "hold exceeded" << endl;
                    block = true;
                }
                if (GetAsyncKeyState(VK_UP)) {
                    cout << "next rep" << endl;
                    block = false;
                    break;
                }
            }
            last = elapsed;
            elapsed = time(NULL) - start;
        }

    }
    get();
    return 0;
}
