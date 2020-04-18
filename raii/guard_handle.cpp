#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>

// 通过智能指针管理第三方库分配的内存
// 这种内存一般都需要通过第三方库释放
// 使用智能指针时传入析构函数释放内存
void* p = GetHandle()->Create();
std::shared_ptr<void> sp(p, [this](void* p) { GetHandle()->Release(p); });

//提炼出来一个公共函数方便调用
std::shared_ptr<void> Guard(void* p) {
  return std::shared_ptr<void> sp(p,
                                  [this](void* p) { GetHandle()->Release(p); });
}
Guard(p);  // 不安全，这是一个右值，此句之后释放

// 增加一个赋值操作避免指针提前释放
#define GUARD(p) \
  std::shared_ptr<void> sp##p(p, [](void* p) { GetHandle()->Release(p); });
GUARD(p);  // 安全
