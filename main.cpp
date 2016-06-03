#include "thread.hpp"

#include <chrono>
#include <ctime>
#include <iostream>
#include <atomic>
#include <random>
#include <string>

threadpool::pool::thread        ap__;

std::mutex                      mutex__;
std::atomic_bool                exit_b__(true);
std::atomic_bool                exit_t__(true);
std::atomic_uint64_t            count__;

std::unique_ptr< std::thread >  radom_s;
std::unique_ptr< std::thread >  radom_b;

void sleep(const unsigned long& milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

unsigned int random(const unsigned int& left, const unsigned int& right)
{
    std::mt19937 rng((unsigned int)std::time(nullptr));
    std::uniform_int_distribution<unsigned int> num(left, right);
    return num(rng);
}

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
    while (!exit_b__)
    {
        ap__.post(std::bind(&test, ++count__));
        sleep(1);
    }
}

void random_test_start_stop()
{
    while (!exit_t__)
    {
        ap__.start();
        sleep(random(100,200));
        ap__.stop();
    }
}

void random_test_wait()
{
    ap__.wait();
}

int main(int, char**)
{
    do
    {
        std::string cmd;

        std::getline(std::cin, cmd);

        if (cmd == "q")
        {
            std::cout << "Stop pool" << std::endl;

            ap__.stop();
        }

        else if (cmd == "s")
        {
            std::cout << "Start pool" << std::endl;

            ap__.start();
        }
        else if (cmd == "p")
        {
            std::cout << "Post 1" << std::endl;

            ap__.post([]{ std::cout << "Post 1 task" << std::endl;  });
        }
        else if (cmd == "t")
        {
            if (!exit_t__)
                continue;
            std::cout << "Test start/stop" << std::endl;
            exit_t__ = false;
            radom_s = std::make_unique< std::thread >([] {random_test_start_stop(); });
        }
        else if (cmd == "w")
        {
            if (!exit_b__)
                continue;
            std::cout << "Start post thread" << std::endl;
            exit_b__ = false;
            radom_b = std::make_unique< std::thread >([] {random_test_broadcast(); });
        }
        else if (cmd == "b")
        {
            std::cout << "Dispatch msg" << std::endl;

            for (auto i = 0; i < 10; ++i)
                ap__.dispatch(&dispatch);
        }
        else if (cmd == "a")
        {
            random_test_wait();
        }
        else if (cmd == "qb")
        {
            std::cout << "Stop broadcast" << std::endl;

            exit_b__ = true;

            radom_b->join();
        }
        else if (cmd == "qt")
        {
            std::cout << "Stop stat/stop" << std::endl;

            exit_t__ = true;

            radom_s->join();
        }
    } while (true);

    system("pause");

    return 0;
}