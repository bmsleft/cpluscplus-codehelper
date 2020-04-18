#include <functional>
#include <initializer_list>
#include <iostream>
#include <type_traits>

int& foo(int& i) {
  std::cout << "foo(int):" << i << std::endl;
  return i;
}
float foo(float f) {
  std::cout << "foo(float):" << f << std::endl;
  return f;
}

float test_1() {
  std::cout << "test_1()" << std::endl;
  return 0.1;
}
float test_2(float f) {
  std::cout << "test_2(float):" << f << std::endl;
  return f;
}

// 返回类型后置写法, 模板函数参数 使用完美转发自动推导类型，重载
template <typename T>
auto func(T&& val) -> decltype(foo(std::forward<T>(val))) {
  return foo(std::forward<T>(val));
}

// 返回类型后置, 完美转发，可变参数，万能函数包装器
template <class Function, class... Args>
auto FuncWrapper(Function&& f, Args&&... args)
    -> decltype(f(std::forward<Args>(args)...)) {
  return f(std::forward<Args>(args)...);
}

// 使用初始化列表接收变长数组，完成展开可变参数模板函数
template <typename T>
void printArg(T& t) {
  std::cout << t << std::endl;
}
template <typename... Args>
void expand(Args... args) {
  std::initializer_list<int>{(printArg(args), 0)...};
}

// 类型萃取，使用enable_if以及类型判断结合，优化重载函数
template <class T>
typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type
ToString(T& t) {
  return std::to_string(t);
}

template <class T>
typename std::enable_if<!std::is_same<T, std::string>::value, std::string>::type
ToString(T& t) {
  return t;
}

// lamda链式调用
template <typename T>
class Task;

template <typename R, typename... Args>
class Task<R(Args...)> {
 public:
  Task(std::function<R(Args...)>&& f) : m_fn(std::move(f)) {}
  Task(std::function<R(Args...)>& f) : m_fn(f) {}

  R Run(Args&&... args) { return m_fn(std::forward<Args>(args)...); }

  template <typename F>
  auto Then(F&& f) -> Task<typename std::result_of<F(R)>::type(Args...)> {
    using return_type = typename std::result_of<F(R)>::type;
    auto func = std::move(m_fn);
    return Task<return_type(Args...)>([func, f](Args&&... args) {
      return f(func(std::forward<Args>(args)...));
    });
  }

 private:
  std::function<R(Args...)> m_fn;
};

void TestTask() {
  Task<int(int)> task([](int i) { return i; });
  auto result = task.Then([](int i) { return i + 1; })
                    .Then([](int i) { return i + 3; })
                    .Run(0);
  std::cout << result << std::endl;
}

int main() {
  int i = 0;
  float f = 0.1;
  func(i);
  func(f);
  FuncWrapper(test_1);
  FuncWrapper(test_2, 1);

  expand(1, 2, 3, 4);
  TestTask();
  return 0;
}