#pragma once

#include <random>
namespace Math {
   class Random {
      std::default_random_engine dre;

   public:
      void ReSeed() { dre.seed(std::random_device()()); }

      template<class T>
         auto Pick(T min, T max) {
            static_assert(std::is_fundamental_v<T>, "This function do work whith fundamental type only");

            ReSeed();
            T res{};

            if constexpr (std::is_floating_point_v<T>) {
               std::uniform_real_distribution<T> uid{};
               res = uid(dre, decltype(uid)::param_type(min, max));
            }
            else {
               std::uniform_int_distribution<T> uid{};
               res = uid(dre, decltype(uid)::param_type(min, max));
            }

            return res;
         }
   };

   inline Random Rand;
}