// 同步队列的简单实现
#include <thread>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <list>

template<typename T>
class SimpleSyncQueue
{
public:
    SimpleSyncQueue(){}

    void Put(const T& t) {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_queue.push_back(t);
        m_notEmpty.notify_one();
    }

    void Take(T& t) {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_notEmpty.wait(locker, [this]{return !m_queue.empty();});
        t = m_queue.front();
        m_queue.pop_front();
    }

    bool Empty() {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.empty();
    }

    size_t Size() {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_queue.size();
    }


private:
    std::list<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_notEmpty;
};


// 多线程中只调用一次
std::once_flag oneFlag;
void do_once() {
    std::call_once(oneFlag, []{std::cout << "called once" << std::endl;});
}

int main() {
    std::thread t1(do_once);
    std::thread t2(do_once);

    t1.join();
    t2.join();

    return 0;
}

