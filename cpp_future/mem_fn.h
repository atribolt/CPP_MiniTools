#ifndef UV_MEM_FN_H
#define UV_MEM_FN_H

template<class Functor>
class mem_fn
{
public:
   using value_type = Functor;

public:
   mem_fn(Functor t) 
      : t(t)
   {}

   template<class _T, class ...Args>
      auto operator()(_T& obj, Args ...arg) {
         if constexpr (std::is_member_function_pointer_v<value_type>) {
            return (obj.*t)(arg...);
         }
         else {
            return t(obj, arg...);
         }

      }

private:
   value_type t;
};

#endif // UV_MEM_FN_H


/****************************** EXAMPLE ******************************
   #include <vector>
   #include <algorithm>
   #include <iostream>

   struct TestType {
      static int a;

      TestType() { ++a; }
   
      void Print() { std::cout << a << ' '; }
   };

   int Test() {
      using namespace std;

      vector<TestType> vec(10, TestType());

      for_each(begin(vec), end(vec), mem_fn(&TestType::Print));
   }

   //Output:
   //   1 2 3 4 5 6 7 8 9 10
*********************************************************************/