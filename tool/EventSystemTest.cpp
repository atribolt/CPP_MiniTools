#ifdef _EVENT_SYSTEM_EXAMPLE_

#include <iostream>

#include "EventSystem.hpp"

class T {
public:
   void SetName(std::string name) {
      signal(name);
   }

   void BtnPress() {
      auto calls = btn_press();

      std::cout << "Btn press worked:\n";

      for ( auto& [who, val] : calls ) {
         std::cout << '\t' << who << ' ' << val << '\n';
      }
   }

   ES::Function<std::string, void, std::string> signal;
   ES::Function<int, float> btn_press;
};

class Recv {
public:
   std::string id;

   void PrintName(std::string name) {
      std::cout << "PrintName " << id << ": " << name << '\n';
   }
};

void PrintName(std::string name) {
   std::cout << "PrintName^ " << name << '\n';
}

int main(int argc, char **argv) {
   T t;
   Recv r { "RECV" };


   t.signal.add("RECV", r, &Recv::PrintName);
   t.signal.add("PRINTNAME", &PrintName);
   t.signal.add("LAMBDA", [](std::string name) { std::cout << "Lambda: " << name << '\n'; });

   t.SetName("Vasa");

   t.signal.disconnect("RECV");

   t.SetName("Carel");

   t.btn_press.add(0, [](){ return 0.5; });
   t.btn_press.add(1, [](){ return 0.1; });

   t.BtnPress();

   return 0;
}

#endif