libthreadpool
=============

一个比较轻量级通用简单的线程池
vs2013编译,完整std支持,移除了boost依赖

使用方法详见test.cpp

/////////////////////////////////////////////////////////////////////////
{
    BrinK::threadpool::pool p;

    p.post(...);	//推送一个任务
    p.waitall();	//等待所有任务完成
    p.post(...);	//推送一个任务（队列）
    p.stop();		//停止
    p.dispatch();	//立刻执行一个任务（队列最前）
    p.start();		//重新开始（与stop成对）
    p.waitall();	//等待所有任务完成

    return 0;
}