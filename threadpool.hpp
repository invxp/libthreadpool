#ifndef INVXP_THREAD_POOL_H
#define INVXP_THREAD_POOL_H

#include <list>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>
#include <algorithm>

namespace InvXp
{
    namespace threadpool
    {
        typedef std::function< void() >                 task_t;

        typedef std::shared_ptr< std::thread >          thread_ptr_t;

        class pool
        {
        public:
            pool( const unsigned int& pool_size = std::thread::hardware_concurrency() );
            virtual ~pool();

        public:
            void post( const task_t& f );
            void dispatch( const task_t& f );
            bool waitall();
            void stop();
            bool start( const unsigned int& pool_size = std::thread::hardware_concurrency() );

        private:
            void    pool_func_();
            void    awake_thread_();
            bool    get_task_( task_t& t );
            void    clear_();
            void    run_( const unsigned int & pool_size );

        private:

            std::list< task_t >                                     tasks_;
            std::mutex                                              tasks_mutex_;

            std::list< thread_ptr_t >                               threads_;

            std::condition_variable                                 condition_;

            std::mutex                                              wait_all_mutex_;
            std::condition_variable                                 wait_all_condition_;

            volatile    bool                                        stop_flag_;
        };

        pool::pool( const unsigned int& pool_size ) :stop_flag_(false)
        {
            run_(pool_size);
        }

        pool::~pool()
        {
            clear_();
        }

        void pool::post( const task_t& f )
        {
            std::unique_lock< std::mutex > lock(tasks_mutex_);
            tasks_.push_back(f);
            lock.unlock();
            awake_thread_();
        }

        void pool::dispatch( const task_t& f )
        {
            std::unique_lock< std::mutex > lock(tasks_mutex_);
            tasks_.push_front(f);
            lock.unlock();

            awake_thread_();
        }

        bool pool::get_task_( task_t& t )
        {
            std::unique_lock< std::mutex > lock(tasks_mutex_);

            while (tasks_.empty())
            {
                wait_all_condition_.notify_all();

                if (stop_flag_)
                {
                    t = []{};
                    return false;
                }

                condition_.wait(lock);
            }

            t = tasks_.front();
            tasks_.pop_front();
            return true;
        }

        bool pool::waitall()
        {
            if (stop_flag_)
                return false;

            std::unique_lock< std::mutex > lock(wait_all_mutex_);
            wait_all_condition_.wait(lock);
            return true;
        }

        void pool::pool_func_()
        {
            while (!stop_flag_)
            {
                task_t task;
                get_task_(task);
                task();
                std::this_thread::yield();
            }
        }

        void pool::awake_thread_()
        {
            condition_.notify_one();
        }

        void pool::stop()
        {
            stop_flag_ = true;

            condition_.notify_all();

            std::for_each(threads_.begin(), threads_.end(), [this]( thread_ptr_t& thread ){ thread->join(); });

            threads_.clear();

        }

        void pool::clear_()
        {
            threads_.clear();
            std::unique_lock< std::mutex > lock(tasks_mutex_);
            tasks_.clear();
        }

        bool pool::start( const unsigned int& pool_size /*= std::thread::hardware_concurrency() */ )
        {
            if (!stop_flag_)
                return false;

            run_(pool_size);

            return true;
        }

        void pool::run_( const unsigned int & pool_size )
        {
            stop_flag_ = false;
            for (unsigned int i = 0; i < pool_size; i++)
            threads_.emplace_back(std::make_shared<std::thread>(std::bind(&threadpool::pool::pool_func_, this)));
        }


    }
}
#endif