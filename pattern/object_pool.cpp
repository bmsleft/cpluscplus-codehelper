#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "noncopyable.hpp"

constexpr const int MaxObjectNum = 10;

template <typename T>
class ObjectPool : public NonCopyable {
  template <typename... Args>
  using Constructor = std::function<std::shared_ptr<T>(Args...)>;

 public:
  // 创建默认个对象，默认析构函数中不直接删除对象，而是回收到对象池中下次继续使用
  template <typename... Args>
  void Init(size_t num, Args... args) {
    if (num <= 0 || num > MaxObjectNum) {
      std::cout << "object num out of range\n";
    }
    auto constructName = typeid(Constructor<Args...>).name();
    std::cout << "objectPool debug: constructname: " << constructName
              << std::endl;
    for (size_t i = 0; i < num; i++) {
      m_object_map.emplace(
          constructName,
          std::shared_ptr<T>(new T(std::forward<Args...>(args)...),
                             [this, constructName](T* p) {
                               m_object_map.emplace(std::move(constructName),
                                                    std::shared_ptr<T>(p));
                             }));
    }
  }

  // 从对象池中获取一个对象
  template <typename... Args>
  std::shared_ptr<T> Get() {
    std::string consturctName = typeid(Constructor<Args...>).name();
    auto range = m_object_map.equal_range(consturctName);
    for (auto it = range.first; it != range.second; ++it) {
      auto ptr = it->second;
      m_object_map.erase(it);
      return ptr;
    }
    std::cout << "objectPool debug: no this constructname: " << consturctName
              << std::endl;
    return nullptr;
  }

 private:
  std::multimap<std::string, std::shared_ptr<T>> m_object_map;
};

struct BigObject {
  BigObject() {}
  BigObject(int a) : m_a(a) {}
  void print() { std::cout << "BigObject test:" << m_a << std::endl; }
  int m_a;
};

int main() {
  ObjectPool<BigObject> pool;
  pool.Init(3);
  {
    auto a = pool.Get();
    auto b = pool.Get();
    a->print();
    b->print();
  }
  pool.Init(2, 6);
  auto c = pool.Get<int>();
  c->print();
}

