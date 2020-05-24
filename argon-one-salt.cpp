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

constexpr uint8_t kTempTrig_10 {50};
constexpr uint8_t kTempTrig_50 {55};
constexpr uint8_t kTempTrig_99 {60};

uint8_t temp_to_pwm(const uint8_t temp) noexcept
{
    uint8_t pwm = {};

    if      (                        temp < kTempTrig_10) { pwm =  0; }
    else if (kTempTrig_10 <= temp && temp < kTempTrig_50) { pwm = 10; }
    else if (kTempTrig_50 <= temp && temp < kTempTrig_99) { pwm = 50; }
    else if (kTempTrig_99 <= temp                       ) { pwm = 99; }

    return pwm;
}

}

int main()
{
    using namespace std::chrono_literals;

    std::signal(SIGINT,  signal_handler);
    std::signal(SIGTERM, signal_handler);

    std::future<int> thr_temp = std::async(std::launch::async,
        [&]() -> int {
            int32_t temp_milli = 0;
            auto zone_file = std::ifstream("/sys/class/thermal/thermal_zone0/temp");
            std::unique_lock lk(mtx);
            while (is_run) {
                zone_file >> temp_milli;
                std::cout << "pwm = " << temp_to_pwm(temp_milli / 1000.F) << '\n';
                cv.wait_for(lk, 1s, [&]() { return false == is_run; });
                zone_file.seekg(0);
            }
            return 0;
        }
    );

    thr_temp.wait();
}
