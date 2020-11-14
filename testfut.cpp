
#include <unistd.h>
#include "taskflow\taskflow.hpp"
#include <future>
//g++ testfut.cpp -std=c++17 -I D:/tasknew/taskflow/ -O2 -pthread -o testfut
//public std::future<T> 
template <typename T>
class Future : {
  public:
    //future object to store futre object returned by executor.run_until
    std::future<T> future_obj;

    //function for running wait_until method of future object
    template <class Clock, class Duration>
    auto wait_until (const std::chrono::time_point<Clock,Duration>& abs_time) const {
      return future_obj.wait_until(abs_time);
    }

    //function for running wait_for method of future object
    template <class Rep, class Period>
    auto wait_for (const std::chrono::duration<Rep,Period>& rel_time) const{
      return future_obj.wait_for( rel_time);
    }
    
    //function for running wait method of future object
    void wait() const{
      future_obj.wait();
      return;
    }

    //function for running valid method of future object
    bool valid() const noexcept{
      return future_obj.valid();
    }

    //function for running get method of future object
    template<class _Res>
    T get() {
      T fut= future_obj.get();
      return fut;
    }

    //function for running share method of future object
    auto share(){
      return future_obj.share();
    }

    //operator equivalent to "=" operator of future object
    auto operator=(std::future<T>&& rhs) noexcept{
      future_obj=rhs;
    }

    //method for setting is_cancel variable of the topology to true
//     void cancel(){
//       std::cout<<"\nCANCEL CALLED\n";
//       _topology->set_cancel();
//       return;
//     }

//     //method for setting the topology for this class
//     void set_tpg(Topology* tpg){
//       _topology=tpg;
//       return;
//     }

//     //method for setting async task node for this class
//     void set_async_node(Node* node){
//       _node=node;
//       return;
//     }

//     //method for setting async_cancelled of an async node to true.
//     void cancel_async(){
//       _node->async_cancelled=true;
//       return;
//     }

//   private:
//     Topology* _topology;
//     Node* _node;

};

int main(){

    std::promise<int> p;
    Future<int> f;
    p.set_value(1);
    f.future_obj=p.get_future();
    f.valid();


}