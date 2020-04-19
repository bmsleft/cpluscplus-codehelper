#include <algorithm>
#include <iostream>

#include "listenercontainer.h"

template <typename ListenerType, template <class> class RefType,
          typename Container>
template <typename RefTypeListener>
void ListenerContainer<ListenerType, RefType, Container>::add(
    RefTypeListener&& listener) {
  std::lock_guard<std::recursive_mutex> lock(m_containerMutex);
  m_addQueue.emplace_back(std::forward<RefType<ListenerType>>(listener));
}

template <typename ListenerType, template <class> class RefType,
          typename Container>
bool ListenerContainer<ListenerType, RefType, Container>::remove(
    RefType<ListenerType>& listener) {
  std::lock_guard<std::recursive_mutex> lock(m_containerMutex);
  try {
    m_removeQueue.emplace_back(listener);
  } catch (const std::exception& e) {
    std::cout << "ListenerContainer: error remove listener :" << e.what()
              << '\n';
    return false;
  }
  return true;
}

template <typename ListenerType, template <typename> class RefType,
          typename Container>
template <typename ListenerFunc>
bool ListenerContainer<ListenerType, RefType, Container>::notifyCallbacks(
    const ListenerFunc& fun) {
  std::lock_guard<std::recursive_mutex> lock(m_containerMutex);

  processAddQueue();
  processRemoveQueue();
  processCleanup();
  bool called = false;
  for (auto& listener : m_container) {
    auto lockerListener = listener.lock();
    if (lockerListener) {
      fun(lockerListener);
      called = true;
    }
  }
  return called;
}

template <typename ListenerType, template <typename> class RefType,
          typename Container>
void ListenerContainer<ListenerType, RefType, Container>::processAddQueue() {
  for (auto& listener : m_addQueue) {
    m_container.emplace_back(listener);
  }
  m_addQueue.clear();
}

template <typename ListenerType, template <typename> class RefType,
          typename Container>
void ListenerContainer<ListenerType, RefType, Container>::processRemoveQueue() {
  for (auto& listener : m_removeQueue) {
    removeImpl(listener);
  }
  m_removeQueue.clear();
}

template <typename ListenerType, template <typename> class RefType,
          typename Container>
bool ListenerContainer<ListenerType, RefType, Container>::removeImpl(
    RefType<ListenerType>& listener) {
  auto lockedListener = listener.lock();
  auto foundElem = m_container.end();
  if (lockedListener) {
    foundElem = std::find_if(m_container.begin(), m_container.end(),
                             [lockedListener](RefType<ListenerType>& elem) {
                               auto lockedElem = elem.lock();
                               if (lockedElem) {
                                 return lockedElem == lockedListener;
                               }
                               return false;
                             });
  }

  if (foundElem != m_container.end()) {
    m_container.erase(foundElem);
    return true;
  }

  return false;
}

template <typename ListenerType, template <typename> class RefType,
          typename Container>
void ListenerContainer<ListenerType, RefType, Container>::processCleanup() {
  m_container.erase(
      std::remove_if(m_container.begin(), m_container.end(),
                     [](RefType<ListenerType>& elem) { return !elem.lock(); }),
      m_container.end());
}

template <typename ListenerType, template <typename> class RefType,
          typename Container>
std::size_t ListenerContainer<ListenerType, RefType, Container>::size() {
  std::lock_guard<std::recursive_mutex> lock(m_containerMutex);
  processAddQueue();
  processRemoveQueue();
  processCleanup();
  return m_container.size();
}

template <typename ListenerType, template <typename> class RefType,
          typename Container>
bool ListenerContainer<ListenerType, RefType, Container>::empty() {
  return size() == 0;
}
