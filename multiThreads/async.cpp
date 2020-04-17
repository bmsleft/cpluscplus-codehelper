#include <iostream>
#include <utility>
#include <future>
#include <thread>


// promise, 获取异步线程的某个值
void TestPromise() {
    std::promise<int> pr;
    std::thread t1([](std::promise<int>& p){
        p.set_value_at_thread_exit(8);}, std::ref(pr));
    t1.detach();
    std::future<int> f = pr.get_future();
    int r = f.get();
    std::cout << "test promise: " << r << std::endl;
}

// package_task, 获取一个异步操作的返回值
void TestPackage_task() {
    std::packaged_task<int(int)> task([](int x){return x + 1;});
    std::future<int> fut = task.get_future();
    std::thread t2(std::move(task), 2);
    t2.detach();
    int value = fut.get();
    std::cout << "test package_task: " << value << std::endl;
}

void TestAsync() {
    std::future<int> fut1 = std::async(std::launch::async, []{
        std::cout << "async working.." << std::endl;
        return 8;
    });
    std::cout << "test async fut1: " << fut1.get() << std::endl;
}


int main() {
    TestPromise();
    TestPackage_task();
    TestAsync();
    return 0;
}
