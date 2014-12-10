#include "threadpool.hpp"
#include <chrono>
#include <ctime>
#include <iostream>

unsigned __int64 get_current_stamp()
{
    return std::time(nullptr);
}

std::mutex mutex__;

void test(const unsigned __int64& i)
{
    std::unique_lock< std::mutex > lock(mutex__);

    std::cout << "num : " << i << " thread_id : " << std::this_thread::get_id() << std::endl;
}

int main(int,char**)
{

    BrinK::threadpool::pool p;

    p.start();

    for (int j=0;j<1;j++)
    {
        int64_t dw = get_current_stamp();
        for (int i=0;i<1000;i++)
        {
            p.post(std::bind(&test,i));
        }
        p.stop();
        p.waitall();
        std::cout << "Dispatch new message" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        p.dispatch(std::bind(&test,99999999));
        p.start();
        p.waitall();

        std::cout << "Post " << j << " completed " << get_current_stamp() - dw << std::endl;

    }
    int64_t dw = get_current_stamp();
    p.stop();

    std::cout << "Stop " << get_current_stamp() - dw << std::endl;

    system("pause");

    return 0;
}