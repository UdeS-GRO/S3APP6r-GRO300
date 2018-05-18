#include <cstdio>
#include <cmath>
#include <cstdlib>

int main(int argc, char** argv)
{
    float x = 0.0;
    for (int i = 0; i < 100; ++i) {
        x += 0.1;

        printf("x: %f, sin(x*pi): %f\n", 
               x,
               sin(x * M_PI));
    }

    return 0;
}
