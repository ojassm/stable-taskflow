// A simple example to capture the following task dependencies.
//
//           +---+                 
//     +---->| B |-----+           
//     |     +---+     |           
//   +---+           +-v-+         
//   | A |           | D |         
//   +---+           +-^-+         
//     |     +---+     |           
//     +---->| C |-----+           
//           +---+
//
#include <iostream>
#include <unistd.h>
#include "taskflow\taskflow.hpp" // the only include you need
//g++ simple.cpp -std=c++17 -I D:/tasknew/taskflow/ -O2 -pthread -o simple
int main(){

  tf::Executor executor;
  tf::Taskflow taskflow("simple");
  tf::Taskflow taskflow2;
  auto [A, B, C, D] = taskflow.emplace(
    []() { std::cout << "TaskA\n"; sleep(10);},
    []() { std::cout << "TaskB\n"; },
    []() { std::cout << "TaskC\n"; },
    []() { std::cout << "TaskD\n"; }
  );

  A.precede(B, C);  // A runs before B and C
  D.succeed(B, C);  // D runs after  B and C

  auto [A2, B2, C2, D2] = taskflow2.emplace(
    []() { std::cout << "TaskA2\n"; },
    []() { std::cout << "TaskB2\n"; },
    []() { std::cout << "TaskC2\n"; },
    []() { std::cout << "TaskD2\n"; }
  );

  A2.precede(B2, C2);  // A runs before B and C
  D2.succeed(B2, C2);  // D runs after  B and C
  executor.run(taskflow2);
  executor.run(taskflow);

  return 0;
}
