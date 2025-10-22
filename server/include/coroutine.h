#pragma once

#include <coroutine>

// struct Task {
//   struct promise_type;
//   using handle_type = std::coroutine_handle<promise_type>;
//
//   handle_type coro;
//
//   Task(handle_type h): coro(h) {}
//   Task& operator=(Task&& other) noexcept {
//     if (this != &other) {
//       if (coro) coro.destroy();
//       coro = other.coro;
//       other.coro = nullptr;
//     }
//     return *this;
//   }
//   ~Task() {
//     if (coro) coro.destroy();
//   }
//
//   void resume() {
//     if (coro && !coro.done())
//       coro.resume();
//   }
//
//   struct promise_type {
//     auto get_return_object() {
//       return Task{handle_type::from_promise(*this)};
//     }
//
//     std::suspend_never initial_suspend() { return {}; }
//     std::suspend_always final_suspend() noexcept { return {}; }
//     void return_void() {}
//     void unhandled_exception() {}
//   };
// };

struct promise;

struct Task : std::coroutine_handle<promise>{
  using promise_type = ::promise;
};
struct promise {
  auto get_return_object() {
    return Task{Task::from_promise(*this)};
  }
  std::suspend_never initial_suspend() { return {}; }
  std::suspend_always final_suspend() noexcept { return {}; }
  void return_void() {}
  void unhandled_exception() {}
};
