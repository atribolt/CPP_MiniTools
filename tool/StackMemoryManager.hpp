#include <optional>
#include <list>

namespace tool {

template<class ptr_t>
   struct pred_object_block {
      size_t m_len { 0 }; // длина объекта
      ptr_t* m_obj { 0 }; // расположение объекта
   };

template<class ptr_t>
   using pob_t = pred_object_block<ptr_t>;
}

template<size_t size_b>
class StackMemory {
   using memory_cell_t = char;
   using pob_t         = tool::pob_t<memory_cell_t>;

public:
   using pobs_list = std::list<pob_t>;

   pobs_list& all_objects_pobs() {
      return m_pobs;
   }

   template<class T, class ...Args>
      T* alloc(Args&& ...args) noexcept {
         T* result = nullptr;

         if (auto pob = first_free_block(sizeof(T))) {
            memory_cell_t* mem_place = pob.value().m_obj;
            result = new (mem_place) T(std::forward<Args>(args)...);

            m_free_memory -= sizeof(T);
         }

         return result;
      }
   template<class T>
      void remove(T* ptr) {
         if (ptr != nullptr) {
            auto pob_it = std::find_if(
                 m_pobs.begin()
               , m_pobs.end()
               , [ptr](auto& pob) { return pob.m_obj == (void*)ptr; }
            );

            if (pob_it != m_pobs.end()) {
               ((T*)(pob_it->m_obj))->~T();
               m_pobs.erase(pob_it);

               m_free_memory += sizeof(T);
            }
         }
      }

      size_t free_space() const { return m_free_memory; }

private: 
   std::optional<pob_t> first_free_block(size_t len) {
      // ищем первый свободный блок с размером >= len
      
      if (size_t free_back = check_back_size(); free_back > len)  
         // если в конце области есть место под объект
      { 
         if (m_pobs.empty()) 
         {
            m_pobs.push_back( pob_t{ len, m_mem } );
         }
         else 
         {
            memory_cell_t* locate_free = m_pobs.back().m_obj + m_pobs.back().m_len;

            m_pobs.push_back( pob_t{ len, locate_free } );
         }

         return m_pobs.back();
      }
      
      for ( auto it_b = m_pobs.begin(); it_b != --m_pobs.end(); ++it_b) {
         auto it_a = it_b++;

         memory_cell_t* a_end_mem = it_a->m_obj + it_a->m_len;
         
         if (size_t _free = it_b->m_obj - a_end_mem; 
            _free > len) {
            auto new_pob = m_pobs.insert(it_a, pob_t { len, a_end_mem } );

            return *new_pob;
         }
      }

      return std::nullopt;
   }

   size_t check_back_size() {
      size_t result = size_b;

      if (!m_pobs.empty()) 
      {
         const auto& pob = m_pobs.back();

         memory_cell_t* ptr_end_mem = m_mem + size_b;

         result = ptr_end_mem - (pob.m_obj + pob.m_len);
      }

      return result;
   }

   pobs_list     m_pobs;
   size_t        m_free_memory { size_b };
   memory_cell_t m_mem[size_b];
};