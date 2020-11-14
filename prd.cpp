#include <unistd.h>
#include "taskflow\taskflow.hpp"
#include <future>

//g++ prd.cpp -std=c++17 -I D:/tasknew/taskflow/ -O2 -pthread -o prd
enum TYPE {
  GUIDED,
  DYNAMIC,
  STATIC
};

void reduce(unsigned W, TYPE type) {

  tf::Executor executor(W);
  tf::Taskflow taskflow;

  std::vector<int> vec(1000);

  for(auto& i : vec) i = ::rand() % 100 - 50;

  for(size_t n=1; n<vec.size(); n++) {
    for(size_t c=0; c<=17; c=c*2+1) {

      int smin = std::numeric_limits<int>::max();
      int pmin = std::numeric_limits<int>::max();
      auto beg = vec.end();
      auto end = vec.end();

      taskflow.clear();
      auto stask = taskflow.emplace([&](){
        beg = vec.begin();
        end = vec.begin() + n;
        for(auto itr = beg; itr != end; itr++) {
          smin = std::min(*itr, smin);
        }
      });

      tf::Task ptask;

      switch (type) {
        case GUIDED:
          ptask = taskflow.reduce_guided(
            std::ref(beg), std::ref(end), pmin, [](int& l, int& r){
            return std::min(l, r);
          }, c);
        break;

        case DYNAMIC:
          ptask = taskflow.reduce_dynamic(
            std::ref(beg), std::ref(end), pmin, [](int& l, int& r){
            return std::min(l, r);
          }, c);
        break;
        
        case STATIC:
          ptask = taskflow.reduce_static(
            std::ref(beg), std::ref(end), pmin, [](int& l, int& r){
            return std::min(l, r);
          }, c);
        break;
      }

      stask.precede(ptask);
        //std::cout<<"before wait";
      auto c1= executor.run(taskflow);
      c1.wait();
      assert(smin != std::numeric_limits<int>::max());
      assert(pmin != std::numeric_limits<int>::max());
      assert(smin == pmin);
    }
  }
}

int main(){
    reduce(8,GUIDED);
}
