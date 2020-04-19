#include <map>
#include <memory>
#include <mutex>
#include <typeindex>

namespace reg_helper {
/**
 * @brief Generates a key from a type
 */
template <typename T>
std::type_index mapkey() {
  return std::type_index(typeid(T));
}

/**
 * @brief Stattes if a weak pointer now refers to a null pointer (or about to be
 * null)
 */
template <typename T>
bool isNull(std::weak_ptr<T> value) {
  return value.expired();
}

/**
 * @brief States if an instance is null
 */
template <typename T, template <class> class RefType>
bool isNull(RefType<T> value) {
  return (nullptr == value);
}

/**
 * @brief Static cast for a weak pointer
 */
template <typename T>
std::weak_ptr<T> static_cast_to(std::weak_ptr<void> value) {
  return std::static_pointer_cast<T>(value.lock());
}

/**
 * @brief Generic static cast
 */
template <typename T, template <class> class RefType>
RefType<T> static_cast_to(RefType<void> value) {
  return std::static_pointer_cast<T>(value);
}
}  // namespace reg_helper

/**
 * @class Registry
 * @brief Generic references registry
 */
template <template <class> class RefType>
class Registry {
 public:
  /**
   * @brief Gets an existing instance
   *        Returns nullptr the desired instance is not found
   *
   * @return RefType<T>
   */
  template <typename T>
  RefType<T> getInstance() const {
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    try {
      return reg_helper::static_cast_to<T>(
          m_instances.at(reg_helper::mapkey<T>()));
    } catch (std::out_of_range) {
      return RefType<T>();
    }
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
  bool registerInstance(RefType<T> instance) {
    if (reg_helper::isNull(instance)) {
      return false;
    }
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    const auto ret =
        m_instances.insert(std::make_pair(reg_helper::mapkey<T>(), instance));
    return ret.second;
  }

  /**
   * @brief Replaces an existing instance
   *        Returns false if the provided instance Type was not already
   * registered or if a nullptr provided
   *
   * @param instance - The replacement instance
   * @return bool
   */
  template <typename T>
  bool replaceInstance(RefType<T> instance) {
    if (reg_helper::isNull(instance)) {
      return false;
    }
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    auto it = m_instances.find(reg_helper::mapkey<T>());
    if (it == std::end(m_instances)) {
      return false;
    }
    it->second = instance;
    return true;
  }

  /**
   * @brief Removes an instance of a given type T
   *        Returns false if the instance type was not registered
   *
   * @return bool
   */
  template <typename T>
  bool removeInstance() {
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    auto it = m_instances.find(reg_helper::mapkey<T>());
    if (it == std::end(m_instances)) {
      return false;
    }
    m_instances.erase(it);
    return true;
  }

  /**
   * @brief Remove all registered instances
   */
  void removeAll() {
    std::lock_guard<std::mutex> lock(m_instanceMutex);
    m_instances.clear();
  }

 private:
  using Container = std::map<std::type_index, RefType<void>>;
  mutable std::mutex m_instanceMutex;
  /**
   * @brief Registered instances container
   */
  Container m_instances;
};
