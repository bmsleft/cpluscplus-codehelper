#include <functional>
#include <memory>
#include <mutex>
#include <vector>

/**
 * @class ListenerContainer
 * @brief Implements a weak reference based listener container that allows for
 * listeners addition/removal and notification
 * @note Not thread safe!
 *
 * @tparam ListenerType - Listener type
 * @tparam RefType - Listener reference type
 * @tparam Container - Container of listeners, defaults to std::vector
 *
 */

template <typename ListenerType, template <class> class RefType = std::weak_ptr,
          typename Container = std::vector<RefType<ListenerType>>>
class ListenerContainer {
 public:
  /**
   * @brief Default constructor
   */
  ListenerContainer() = default;

  /**
   * Add a listener
   * @tparam RefTypeListener. The type needs to be of type RefType<ListenerType>
   * @param listener
   */
  template <typename RefTypeListener>
  void add(RefTypeListener&& listener);

  /**
   * Removes a listener
   *
   * @param listener - Listener to remove
   *        Returns true if removal happened
   * @return bool
   */
  bool remove(RefType<ListenerType>& listener);

  /**
   * Notifies all listeners. (calls all listeners with a given function)
   * @tparam ListenerFunc
   * @param fun
   * @return true if at least one notification was made
   */
  template <typename ListenerFunc>
  bool notifyCallbacks(const ListenerFunc& fun);

  /**
   * @brief Determines the real size of container by processing add and remove
   * queues
   * @return std::size_t
   */
  std::size_t size();

  /**
   * @brief Determines is the container is empty
   * @return true is empty, false otherwise
   */
  bool empty();

 private:
  /**
   * Adds queued listeners
   */
  void processAddQueue();

  /**
   * Removes queued listeners
   */
  void processRemoveQueue();

  /**
   * Removes a listener
   * @param listener
   * @return true if removal happened
   */
  bool removeImpl(RefType<ListenerType>& listener);

  /**
   * Cleans-up
   */
  void processCleanup();

 private:
  /**
   * @brief Current listeners container
   */
  Container m_container;

  /**
   * @brief Candidate listeners container. Listeners are queued here before they
   * are actually added to the active container
   */
  Container m_addQueue;

  /**
   * @brief Listeners to be removed container. Listeners to be removed are
   * queued here before they are removed from the active listeners container
   */
  Container m_removeQueue;

  /**
   * @brief Mutex to protect main container: both size and notify listeners can
   * modify it from different threads.
   */
  std::recursive_mutex m_containerMutex;
};
