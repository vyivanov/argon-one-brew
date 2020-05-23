#include <future>
#include <csignal>
#include <fstream>
#include <iostream>
#include <condition_variable>

namespace {

std::condition_variable cv = {};
std::mutex mtx = {};

bool is_run = true;

void signal_handler(const int /*sig*/)
{
    std::lock_guard lk(mtx);
    is_run = false;

    std::cout << "Ha!" << '\n';

    cv.notify_all();
}

}

int main()
{
    using namespace std::chrono_literals;

    std::signal(SIGINT,  signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::future<int> thr_temp = std::async(std::launch::async,
        [&]() -> int {
            int32_t temp = 0;
            auto zone = std::ifstream("/sys/class/thermal/thermal_zone0/temp");
            std::unique_lock lk(mtx);
            while (is_run) {
                zone >> temp;
                std::cout << "temp = " << temp / 1000.F << '\n';
                cv.wait_for(lk, 1s, [&]() { return false == is_run; });
                zone.seekg(0);
            }
            return 0;
        }
    );

    thr_temp.wait();
}
