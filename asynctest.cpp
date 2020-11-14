#include <iostream>
#include <unistd.h>
#include "taskflow\taskflow.hpp"

#include <vector>
#include <utility>
#include <chrono>
#include <limits.h>
//g++ asynctest.cpp -std=c++17 -I D:/tasknew/taskflow/ -O2 -pthread -o asynctest

int main(){
tf::Executor executor(1);

  std::vector< tf::Future<int> > fus;

  std::atomic<int> counter(0);
  
  int N = 100000;

  for(int i=0; i<N; ++i) {
    fus.emplace_back(executor.async([&](){
      counter.fetch_add(1, std::memory_order_relaxed);
      return -2;
    }));
  }
  
  executor.wait_for_all();

  assert(counter == N);
  
  int c = 0; 
  for(auto& fu : fus) {
    c += fu.get();
  }

  assert(-c == 2*N);
  std::cout<<"done!";
}