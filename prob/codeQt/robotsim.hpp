#ifndef ROBOTSIM_HPP
#define ROBOTSIM_HPP

namespace s3gro {
    class RobotDiag;
}

namespace robotsim {
    void init(s3gro::RobotDiag* diag, int n_motors, int delay, int delta);

    void stop_and_join();
}

#endif
