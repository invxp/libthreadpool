#include "threadpool.hpp"
#include <chrono>
#include <ctime>
#include <iostream>
#include <atomic>
#include <random>
#include <string>
#include <future>

BrinK::pool::thread             tp__;
std::mutex                      mutex__;
volatile std::atomic_bool       exit_b__;
volatile std::atomic_bool       exit_t__;
std::atomic_uint64_t            count__;

unsigned __int64 get_current_stamp()
{
    return std::time(nullptr);
}

void test(const unsigned __int64& i)
{
    std::unique_lock< std::mutex > lock(mutex__);
    std::cout << "Num : " << i << std::endl;
}

void dispatch()
{
    std::unique_lock< std::mutex > lock(mutex__);
    std::cout << "Dispatch Message" << std::endl;
}

void random_test_broadcast()
{
    std::mt19937 engine(std::time(nullptr));
    std::uniform_int_distribution<int> num(100, 200);

    static int send_count = 0;

    while (!exit_b__)
    {
        int rand = num(engine);
        tp__.post(std::bind(&test, ++count__));
        std::this_thread::sleep_for(std::chrono::milliseconds(rand));
    }

}

void random_test_start_stop()
{
    std::mt19937 rng(std::time(nullptr));
    std::uniform_int<> ui(100, 200);

    while (!exit_t__)
    {
        int rand = ui(rng);
        tp__.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(rand));
        tp__.stop();
        tp__.cancel();
    }
}

int main(int, char**)
{
    do
    {
        std::string cmd;

        std::getline(std::cin, cmd);

        if (cmd == "q")
        {
            std::cout << "stop pool" << std::endl;

            std::async([]
            {
                tp__.stop();
            });
        }

        else if (cmd == "s")
        {
            std::cout << "start 1 pool" << std::endl;

            std::async([]
            {
                tp__.start(1);
            });

        }
        else if (cmd == "p")
        {
            std::cout << "post 1" << std::endl;

            std::async([]
            {
                tp__.post([]{});
            });

        }
        else if (cmd == "t")
        {
            std::cout << "test start/stop" << std::endl;
            exit_t__ = false;
            std::async([]{random_test_start_stop(); });
        }

        else if (cmd == "w")
        {
            std::cout << "start post thread" << std::endl;
            exit_b__ = false;
            std::async([]
            {
                random_test_broadcast();
            });
        }
        else if (cmd == "b")
        {
            std::cout << "broadcast msg" << std::endl;

            std::async([]
            {
                for (auto i = 0; i < 10; ++i)
                    tp__.dispatch(&dispatch);
            });
        }
        else if (cmd == "a")
        {
            std::cout << "wait" << std::endl;

            std::async([]
            {
                if (tp__.wait())
                    std::cout << "wait_all_ok!!!" << std::endl;
                else
                    std::cout << "no_wait!!!" << std::endl;
            });
        }
        else if (cmd == "o")
        {
            std::cout << "wait_one" << std::endl;

            std::async([]
            {
                if (tp__.wait_one())
                    std::cout << "wait_one_ok!!!" << std::endl;
                else
                    std::cout << "no_wait!!!" << std::endl;
            });
        }
        else if (cmd == "qb")
        {
            std::cout << "stop broadcast" << std::endl;

            exit_b__ = true;
        }
        else if (cmd == "qt")
        {
            std::cout << "stop stat/stop" << std::endl;

            exit_t__ = true;
        }
    } while (true);

    system("pause");

    return 0;
}