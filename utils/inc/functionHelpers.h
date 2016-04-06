#ifndef _FUNCTION_HELPERS_H_
#define _FUNCTION_HELPERS_H_

#include <thread>
#include <chrono>

template <typename Function>
void callAfter(long millis, Function const& function) {
  std::thread([millis, function]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    function();
  }).detach();
}

#endif  //_FUNCTION_HELPERS_H_
