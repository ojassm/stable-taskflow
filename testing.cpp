#include <iostream>
#include <unistd.h>
#include "taskflow\taskflow.hpp"
  // Taskflow is header-only

//g++ testing.cpp -std=c++17 -I D:/tasknew/taskflow/ -O2 -pthread -o testing
/*40 - SerialRuns.2threads (Child aborted)
         57 - ParallelRuns.3threads (Child aborted)
         58 - ParallelRuns.4threads (Child aborted)
         61 - ParallelRuns.7threads (Child aborted)
         63 - NestedRuns.1thread (Child aborted)
         64 - NestedRuns.2threads (Child aborted)
         77 - ParallelForIndex.1thread (Child aborted)
         86 - ReduceMin (Child aborted)
         87 - ReduceMax (Child aborted)
        112 - Composition-1 (Child aborted)
        135 - BTreeCondition (Child aborted)
        162 - CondSubflow.2threads (Child aborted)
        169 - Async.1thread (SEGFAULT)
        170 - Async.2threads (SEGFAULT)
        171 - Async.4threads (SEGFAULT)
        172 - Async.8threads (SEGFAULT)
        174 - NestedAsync.1thread (SEGFAULT)
        175 - NestedAsync.2threads (SIGHUP)
        179 - MixedAsync.1thread (SEGFAULT)
        180 - MixedAsync.2threads (SEGFAULT)
        181 - MixedAsync.4threads (SEGFAULT)
        184 - pfg.1thread (Child aborted)
        185 - pfg.2threads (Child aborted)
        196 - pfd.1thread (Child aborted)
        197 - pfd.2threads (Child aborted)
        198 - pfd.3threads (Child aborted)
        208 - pfs.1thread (Child aborted)
        209 - pfs.2threads (Child aborted)
        213 - pfs.6threads (Child aborted)
        220 - statefulpfg.1thread (Child aborted)
        221 - statefulpfg.2threads (Child aborted)
        225 - statefulpfg.6threads (Child aborted)
        232 - statefulpfd.1thread (Child aborted)
        233 - statefulpfd.2threads (Child aborted)
        235 - statefulpfd.4threads (Child aborted)
        244 - statefulpfs.1thread (Child aborted)
        245 - statefulpfs.2threads (Child aborted)
        247 - statefulpfs.4threads (Child aborted)
        256 - prg.1thread (Child aborted)
        266 - prg.11threads (Child aborted)
        268 - prd.1thread (Child aborted)
        269 - prd.2threads (Child aborted)
        275 - prd.8threads (Child aborted)
        292 - ptrg.1thread (Child aborted)
        293 - ptrg.2threads (Child aborted)
        304 - ptrd.1thread (Child aborted)
        305 - ptrd.2threads (Child aborted)
        316 - ptrs.1thread (Child aborted)
        317 - ptrs.2threads (Child aborted)
        327 - ptrs.12threads (Child aborted)
        */

int main(){
  
  tf::Executor executor;
  tf::Taskflow taskflow, taskflow2;

  auto [A, B, C, D] = taskflow.emplace(
    [] () { std::cout << "TaskA\n"; },               //  task dependency graph sleep(10);
    [] () { std::cout << "TaskB\n";  },               // 
    [] () { std::cout << "TaskC\n"; },               //          +---+          
    [] () { std::cout << "TaskD\n"; }                //    +---->| B |-----+   
  );                                                 //    |     +---+     |
                                                     //  +---+           +-v-+ 
  A.precede(B);  // A runs before B                  //  | A |           | D | 
  A.precede(C);  // A runs before C                  //  +---+           +-^-+ 
  B.precede(D);  // B runs before D                  //    |     +---+     |    
  C.precede(D);  // C runs before D                  //    +---->| C |-----+    
                                                     //          +---+       

  auto [A2, B2, C2, D2] = taskflow2.emplace(
    [] () { std::cout << "TaskA2\n"; },               //  task dependency graph sleep(5);
    [] () { std::cout << "TaskB2\n"; },               // 
    [] () { std::cout << "TaskC2\n"; },               //          +---+          
    [] () { std::cout << "TaskD2\n"; }                //    +---->| B |-----+   
  );                                                 //    |     +---+     |
                                                     //  +---+           +-v-+ 
  A2.precede(B2);  // A runs before B                  //  | A |           | D | 
  A2.precede(C2);  // A runs before C                  //  +---+           +-^-+ 
  B2.precede(D2);  // B runs before D                  //    |     +---+     |    
  C2.precede(D2);  // C runs before D                  //    +---->| C |-----+    
                                                     //          +---+                           
  auto tf1= executor.run(taskflow);
  //auto tf3= executor.run(taskflow);
  //auto tf4= executor.run(taskflow);
  //auto tf5= executor.run(taskflow);
  //taskflow.clear();
  //std::cout <<  "here\n" << executor.num_topologies() ;
  
  auto tf2= executor.run(taskflow2);
  //tf1.get();
  //tf3.cancel();
  //executor.canceltf();
  //std::cout << taskflow.num_tasks();
  //executor.canceltf();
  //std::cout<<tf2.valid();
  //tf1.cancel();
  ///std::cout <<  "here\n"  ;
  //executor.wait_for_all();
  //std::cout << taskflow.num_tasks();
  //
  executor.wait_for_all();
  //std::cout <<  "here\n" << executor.num_topologies() ;
  /*
  terminate called after throwing an instance of 'std::system_error'
  what():  Invalid argument
      0 [testing] testing 1003 cygwin_exception::open_stackdumpfile: Dumping stack trace to testing.exe.stackdump
    */
  return 0;
}

