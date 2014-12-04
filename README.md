libthreadpool
=============

一个比较轻量级通用简单的线程池
该库基于boost::thread，使用了部分c++11特性（因为vs2010对c++11支持不完整）
使用方法详见test.cpp


int main(int,char**)
{
    threadpool::pool p;

    p.post(...);//推送一个任务
    p.waitall();//等待所有任务完成
    p.post(...);//推送一个任务
    p.stop();   //停止
    p.start();  //重新开始（与stop成对）
    p.waitall();//等待所有任务完成

    return 0;
}