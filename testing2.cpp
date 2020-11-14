
#include <iostream>
#include <unistd.h>
#include "taskflow\taskflow.hpp"
#include <vector>
#include <utility>
#include <chrono>
#include <limits.h>
//g++ testing2.cpp -std=c++17 -I D:/tasknew/taskflow/ -O2 -pthread -o testing2

int main(){
using namespace std::chrono_literals;

  size_t num_tasks = 100;
  size_t n_thread;
  tf::Executor executor;
  tf::Taskflow taskflow;

  std::atomic<int> counter {0};
  std::vector<tf::Task> silent_tasks;
    
  for (size_t i=0;i<num_tasks;i++){
    silent_tasks.emplace_back(
      taskflow.emplace([&counter]() {counter += 1;})
    );
  }
  
  //SUBCASE("RunOnce"){
    
  auto fu = executor.run(taskflow);
  assert(taskflow.num_tasks() == num_tasks);
  fu.get();
  assert(counter == num_tasks);
  //}

  //SUBCASE("WaitForAll") {
    counter=0;
  executor.run(taskflow);
  executor.wait_for_all();
  assert(counter == num_tasks); 
  //}

  //SUBCASE("RunWithFuture") {

    std::atomic<size_t> count {0};
    tf::Taskflow f;
    
    auto A = f.emplace([&](){ count ++; });
    auto B = f.emplace([&](tf::Subflow& subflow){ 
      count ++; 
      auto B1 = subflow.emplace([&](){ count++; });
      auto B2 = subflow.emplace([&](){ count++; });
      auto B3 = subflow.emplace([&](){ count++; });
      B1.precede(B3); B2.precede(B3);
    });
    auto C = f.emplace([&](){ count ++; });
    auto D = f.emplace([&](){ count ++; });

    A.precede(B, C);
    B.precede(D); 
    C.precede(D);

    std::list<tf::Future<void>> fu_list;
    for(size_t i=0; i<500; i++) {
      if(i == 499) {
        executor.run(f).get();   // Synchronize the first 500 runs
        executor.run_n(f, 500);  // Run 500 times more
      }
      else if(i % 2) {
        fu_list.push_back(executor.run(f));
      }
      else {
        fu_list.push_back(executor.run(f, [&, i=i](){ 
          assert(count == (i+1)*7); })
        );
      }
    }
    
    executor.wait_for_all();

    for (auto& fu: fu_list) {
      assert(fu.valid());
      //std::cout<<fu.valid();
      assert(fu.wait_for(std::chrono::seconds(1)) == std::future_status::ready);
    }

    assert(count == 7000);

} 
  
  
