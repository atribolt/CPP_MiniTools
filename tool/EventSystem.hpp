#ifndef EVENTSYSTEM_HPP
#define EVENTSYSTEM_HPP

#include <map>
#include <tuple>
#include <string>
#include <vector>
#include <functional>

/*
   For preview this works should be define macros _EVENT_SYSTEM_EXAMPLE_
*/

namespace ES {

template<class ID, class _Tout, class ...Args>
   class Function {
   public:
      using return_type   = _Tout;

   private:
      using tmp_t = std::conditional_t<std::is_same_v<_Tout, void>, nullptr_t, _Tout>;

   public:
      using id_type       = ID;
      using exec_return   = std::pair<id_type, tmp_t>;
      using execute_type  = std::conditional_t<std::is_same_v<tmp_t, nullptr_t>, void, std::vector<exec_return>>;
      using function_type = std::function<return_type(Args...)>;
      using value_type    = std::pair<id_type, function_type>;
      
      execute_type operator()(Args ...args) {

         if constexpr ( std::is_same_v<tmp_t, nullptr_t> ) {
            for (auto& [id, func] : executors) {
               func(args...);
            }
         }
         else {
            execute_type res;

            for (auto& [id, func] : executors)
               res.push_back( exec_return(id, func(args...)) );

            return res;
         }
      }

      template<class T>
         void add (id_type id, T& obj, return_type(T::*func)(Args...)) {
            auto wrapper = [&obj, &func] (Args ...arg) {
               return (obj.*func)(arg...);
            };

            add(id, wrapper);
         }

      void add(id_type id, function_type func) {
         auto iter = find(executors.begin(), executors.end(), id);
         if (iter == executors.end()) {
            executors.push_back( { id, func } );
         }
         else {
            iter->second = std::move(func);
         }
      }

      //void add(std::string name, return_type(*func)(Args ...arg)) {
      //
      //}
      //
      void disconnect(id_type id) {
         auto iter = find(executors.begin(), executors.end(), id);

         if (iter != executors.end()) executors.erase(iter);
      }

   private:
      template<class iter>
         auto find(iter beg, iter end, id_type id) {
            return std::find_if(beg, end, [&id](auto& pair) { return pair.first == id; });
         }

      std::vector<value_type> executors;
   };
}

#endif