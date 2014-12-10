#include "threadpool.hpp"

int64_t GetCurrentStamp64()
{
    boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::gregorian::Jan, 1));
    boost::posix_time::time_duration time_from_epoch =
    boost::posix_time::microsec_clock::universal_time() - epoch;
    return time_from_epoch.total_milliseconds();
}

boost::mutex mutex_;

void test(int arg1)
{
    boost::mutex::scoped_lock lock(mutex_);
    printf("Threading : %d %d \n",boost::this_thread::get_id(),arg1);
}

int main(int,char**)
{

    threadpool::pool p;

    bool b=p.start();

    for (int j=0;j<1;j++)
    {
        boost::this_thread::sleep(boost::posix_time::millisec(500));
        int64_t dw=GetCurrentStamp64();
        for (int i=0;i<10000;i++)
        {
            p.post(std::bind(test,i));
        }
        p.stop();
        b=p.waitall();
        p.dispatch(std::bind(test,999));
        boost::this_thread::sleep(boost::posix_time::millisec(1000));
        p.start();

        b=p.waitall();
        printf("Post %d completed : %d \n",j,GetCurrentStamp64()-dw);

    }
    int64_t dw=GetCurrentStamp64();
    p.stop();
    printf("Stop : %d \n",GetCurrentStamp64()-dw);

    system("pause");

    return 0;
}