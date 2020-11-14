#include "taskflow\taskflow.hpp" 
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <unistd.h> 
//#include <taskflow/taskflow.hpp>
#include <vector>
#include <utility>
#include <chrono>
#include <limits.h>

void sequential_runs(unsigned W) {

  using namespace std::chrono_literals;
  
  size_t num_tasks = 100;
  
  tf::Executor executor(W);
  tf::Taskflow taskflow;

  std::atomic<int> counter {0};
  std::vector<tf::Task> silent_tasks;
    
  for(size_t i=0;i<num_tasks;i++){
    silent_tasks.emplace_back(
      taskflow.emplace([&counter]() {counter += 1;})
    );
  }
  
  SUBCASE("RunOnce"){
    auto fu = executor.run(taskflow);
    REQUIRE(taskflow.num_tasks() == num_tasks);
    fu.get();
    REQUIRE(counter == num_tasks);
  }

  SUBCASE("WaitForAll") {
    executor.run(taskflow);
    executor.wait_for_all();
    REQUIRE(counter == num_tasks); 
  }
  
  SUBCASE("RunWithFuture") {

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

    std::list<std::future<void>> fu_list;
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
          REQUIRE(count == (i+1)*7); })
        );
      }
    }
    
    executor.wait_for_all();

    for(auto& fu: fu_list) {
      REQUIRE(fu.valid());
      REQUIRE(fu.wait_for(std::chrono::seconds(1)) == std::future_status::ready);
    }

    REQUIRE(count == 7000);
    
  }

  SUBCASE("RunWithChange") {
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

    executor.run_n(f, 10).get();
    REQUIRE(count == 70);    

    auto E = f.emplace([](){});
    D.precede(E);
    executor.run_n(f, 10).get();
    REQUIRE(count == 140);    

    auto F = f.emplace([](){});
    E.precede(F);
    executor.run_n(f, 10);
    executor.wait_for_all();
    REQUIRE(count == 210);    
    
  }

  SUBCASE("RunWithPred") {
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

    executor.run_until(f, [run=10]() mutable { return run-- == 0; }, 
      [&](){
        REQUIRE(count == 70);
        count = 0;
      }
    ).get();


    executor.run_until(f, [run=10]() mutable { return run-- == 0; }, 
      [&](){
        REQUIRE(count == 70);
        count = 0;
        auto E = f.emplace([&](){ count ++; });
        auto F = f.emplace([&](){ count ++; });
        A.precede(E).precede(F);
    });

    executor.run_until(f, [run=10]() mutable { return run-- == 0; }, 
      [&](){
        REQUIRE(count == 90);
        count = 0;
      }
    ).get();
    
  }

  SUBCASE("MultipleRuns") {
    std::atomic<size_t> counter(0);

    tf::Taskflow tf1, tf2, tf3, tf4;

    for(size_t n=0; n<16; ++n) {
      tf1.emplace([&](){counter.fetch_add(1, std::memory_order_relaxed);});
    }
    
    for(size_t n=0; n<1024; ++n) {
      tf2.emplace([&](){counter.fetch_add(1, std::memory_order_relaxed);});
    }
    
    for(size_t n=0; n<32; ++n) {
      tf3.emplace([&](){counter.fetch_add(1, std::memory_order_relaxed);});
    }
    
    for(size_t n=0; n<128; ++n) {
      tf4.emplace([&](){counter.fetch_add(1, std::memory_order_relaxed);});
    }
    
    for(int i=0; i<200; ++i) {
      executor.run(tf1);
      executor.run(tf2);
      executor.run(tf3);
      executor.run(tf4);
    }
    executor.wait_for_all();
    REQUIRE(counter == 240000);
    
  }
}
TEST_CASE("SerialRuns.1thread" * doctest::timeout(300)) {
  sequential_runs(1);
}
