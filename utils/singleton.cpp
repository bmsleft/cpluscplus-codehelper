
#include <iostream>
#include <functional>
#include <memory>
#include <utility>
#include <mutex>

template <class T>
class Singleton  {
  public:
    template <typename... Args>
    static T& getInstance(Args&&... args) {
      std::call_once(
        get_once_flag(),
        [] (Args&&... args) {
          instance_.reset(new T(std::forward<Args>(args)...));
        }, std::forward<Args>(args)...);
      return *(instance_.get());
    }

  public:
    Singleton(Singleton const &) = delete;
    Singleton& operator=(Singleton const &) = delete;

  protected:
    explicit Singleton<T>() {}
    ~Singleton<T>() {}

  private:
    static std::unique_ptr<T> instance_;
    static std::once_flag& get_once_flag() {
      static std::once_flag once_;
      return once_;
    }
};

template<class T> std::unique_ptr<T> Singleton<T>::instance_ = nullptr;


//////////////////test case/////////////////////
class Foo  {
  public:
    Foo(const std::string& hello) : hello_(hello)  {
        std::cout << "Foo(): " << hello << std::endl;
    }

    Foo() : hello_("") {
        std::cout << "Foo()" << std::endl;
    }

    void print() {
        std::cout << hello_ << std::endl;
    }
  private:
    std::string hello_;
};

int main(int, const char**) {
    const std::string hello("hello world");
    Singleton<Foo>::getInstance(hello).print();
    Singleton<Foo>::getInstance().print();

}

