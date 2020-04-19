// 依赖注入类
// 通过类名字注册实例，使用是直接根据类名字获取之前注册的实例
// 可以和单例模式一起使用

#include <memory>

#include "registry.hpp"

/**
 * @class DependencyInjector
 * @brief Simple dependency injector
 */
class DependencyInjector {
 public:
  /**
   * @brief Gets an existing instance
   *        Returns nullptr the desired instance is not found
   *
   * @return std::shared_ptr<T>
   */
  template <typename T>
  std::shared_ptr<T> get_instance() const {
    return m_registry.getInstance<T>();
  }

  /**
   * @brief Registers an instance
   *        Returns false if the provided instance was not registered or if a
   * nullptr provided
   *
   * @param instance - The registered instance
   * @return bool
   */
  template <typename T>
  bool registerInstance(std::shared_ptr<T> instance) {
    if (nullptr == instance) {
      return false;
    }
    return m_registry.registerInstance<T>(instance);
  }

  /**
   * @brief Replaces a registered instance
   *        Returns false if there is no instance of that type alrady registered
   *
   * @note  Instances already obtained from get_instance() won't be replaced!
   *
   * @param instance - The registered instance
   * @return bool
   */
  template <typename T>
  bool replaceInstance(std::shared_ptr<T> instance) {
    if (nullptr == instance) {
      return false;
    }
    return m_registry.replaceInstance<T>(instance);
  }

  /**
   * @brief Unregisters all registered instances
   */
  void unregisterAll() { m_registry.removeAll(); }

 private:
  /**
   * @brief Instances registry
   */
  Registry<std::shared_ptr> m_registry;
};

// #include <iostream>
// int main() {
//     class A {
//     public:
//         A(std::string str):m_a(std::move(str)) {}
//         void print() { std::cout << m_a << std::endl;}
//         std::string m_a;
//     };

//     auto a = std::make_shared<A>("hello");
//     DependencyInjector di;
//     di.registerInstance<A>(a);
//     auto aa = di.get_instance<A>();
//     aa->print();
// }
