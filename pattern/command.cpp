#include <functional>
#include <iostream>
#include <type_traits>

template <typename Ret = void>
struct CommCommand {
 private:
  std::function<Ret()> m_f;

 public:
  //接受可调用对象的函数包装器
  template <class F, class... Args,
            class = typename std::enable_if<
                !std::is_member_function_pointer<F>::value>::type>
  void Wrap(F&& f, Args&&... args) {
    m_f = [&] { return f(args...); };
  }

  //接受常量成员函数的函数包装器
  template <class R, class C, class... DArgs, class P, class... Args>
  void Wrap(R (C::*f)(DArgs...) const, P&& p, Args&&... args) {
    m_f = [&, f] { return (*p.*f)(args...); };
  }

  //接受非常量成员函数的函数包装器
  template <class R, class C, class... DArgs, class P, class... Args>
  void Wrap(R (C::*f)(DArgs...), P&& p, Args&&... args) {
    m_f = [&, f] { return (*p.*f)(args...); };
  }

  Ret Excecute() { return m_f(); }
};

struct STA {
  int m_a;
  int triple() { return m_a * 3; }
} STA_t;

int main() {
  CommCommand<int> cmd;
  cmd.Wrap([](int n) { return n + 1; }, 1);
  std::cout << "test command:" << cmd.Excecute() << std::endl;

  STA t = {10};
  cmd.Wrap(&STA::triple, &t);
  std::cout << "test command:" << cmd.Excecute() << std::endl;
}
