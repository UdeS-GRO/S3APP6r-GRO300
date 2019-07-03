#include "../robotdiag.hpp"
#include "../robotsim.hpp"
#include <signal.h>
#include <unistd.h>

using namespace s3gro;

namespace {
    bool run_;

}

void sighandler(int)
{
    run_ = false;
}

int main(int argc, char** argv)
{
    run_ = true;

    signal(SIGINT, sighandler);

    RobotDiag diag;

    while (run_) {
        pause();
    }

    return 0;
}
