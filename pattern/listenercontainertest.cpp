#include <iostream>

#include "listenercontainer.hpp"

class MsgListener {
 public:
  virtual ~MsgListener() = default;
  virtual void onUpdate(const std::string& msg) = 0;
};

class Client : public MsgListener, public std::enable_shared_from_this<Client> {
 public:
  void onUpdate(const std::string& msg) override {
    std::cout << "Client:" << msg << std::endl;
  }
};

class Server {
 public:
  Server() = default;
  void registerMsgUpdate(std::weak_ptr<MsgListener> msgListener) {
    m_msgListeners.add(msgListener);
  }

  void unregisterMsgUpdate(std::weak_ptr<MsgListener> msgListener) {
    m_msgListeners.remove(msgListener);
  }

  void notifyListeners() {
    std::string testStr = "test str";
    m_msgListeners.notifyCallbacks(
        [testStr](const std::shared_ptr<MsgListener>& listener) {
          listener->onUpdate(testStr);
        });
  }

 private:
  ListenerContainer<MsgListener> m_msgListeners;
};

int main() {
  auto server = std::make_shared<Server>();
  std::shared_ptr<Client> client = std::make_shared<Client>();
  server->registerMsgUpdate(client);
  server->notifyListeners();
  return 0;
}