/* NOTE
      3 потока 

      1 - сервер слушает
         бесконечный цикл в одном потоке
         как только получаем новое соединение вызываем колбэк, в который передаем аддрес соединенный сокет и адрес.
         
      2 - обработка входящих смс
         // метод внутри месенджера

         есть список структур - соединение, которое содержит промис, время последнего входящего

         проходим циклом по этому списку, если кто-то получил результат и готов 
               направляем результат в функцию выполнения с этим соединением по изменяемой ссылке


      3 - проверка живности соединений
         // метод внутри мессенджера

         проверяем список определенный интервал времени на длительность последнего смс
         если смс не приходило больше N минут - отправляем ему статус отключения и закрываем.
         удаляем из списка
*/

#include <vector>
#include <future>
#include <string>
#include <variant>

#include <cassert>

#ifdef __gnu_linux__
   // TODO: добавить линуксовые хедеры
   struct sockaddr {};
   using SOCKET = unsigned int;
#elif (_WIN32 || _WIN64) 
   #pragma comment(lib, "Ws2_32.lib")

   #include <winsock2.h>
   #include <ws2tcpip.h>
#endif

#define ASSERT(msg_var) bool msg_var = false; assert(msg_var);

namespace Messanger {
   using port_t = int;
   using data_t = std::vector<char>;
   using sock_t = std::variant<int, SOCKET>;
   using addr_t = std::variant<std::string, sockaddr>;

   int PF_TYPE   = PF_INET
     , SOCK_TYPE = SOCK_STREAM
     , PROTOCOL  = (int)IPPROTO_TCP;


   /*************** функции обертки для разных ОС ***************/
   auto   CreateSocket  (addr_t addr, port_t port);
   auto   BindSocket    (sock_t sock);
   auto   Listen        (sock_t sock, int queuelen = 1);
   auto   Send          (sock_t sock, data_t data);
   auto   Recv          (sock_t sock, size_t part_sz);
   /************************************************************/


   struct Connection {
      using data_t = std::string;

      sock_t connection;
      data_t data;
   };

   class Server {
   public:
      Server();
      Server(
         addr_t host,  // адрес
         port_t port); // порт

      // прослушивать сокет до первого входящего соединения 
      template<class Functor>
         void Listen(
               Functor func      // колбэк сигнатуры [] (sock_t, adrr_t) -> void;
         );

      // начать прослушивать сокет в цикле
      template<class Functor>
         void StartListenLoop(
            size_t  queue_sz, // размер очереди
            Functor func      // колбэк сигнатуры [] ( #TODO ) -> void
         ){
            auto loop = [ & mtx_is_listen
                        , & is_listen
                        , & queue_sz
                        , & func
                        ] 
               (){
                  while (1) {
                     {
                        std::lock_guard lock(mtx_is_listen);
                        if ( !is_listen ) break;
                     }

                     // TODO: слушаем сокет в цикле
                     ASSERT(is_realised_listen_loop);
                  }
               };

            std::async(loop);
         }

      // остановка прослушивающего цикла
      void StopListenLoop() {
         std::lock_guard lock(mtx_is_listen);
         is_listen = false;
      }

   private:
      std::mutex mtx_is_listen; // мьютекс для bool is_listen

      bool is_listen { false };

      sock_t sock;
      port_t port;
   };

   
   
   void d_Worker();           // проверка и обработка входящих смс
   
   template<class TConnection, class TServer>
      class Messanger {
      public:
         void T() {

         }

         using conn_list = std::vector<TConnection>;
         using server_t  = Server;


      public:
      

      private:
         // d - deamon
         void d_CheckConnections(); // 

      private:
         server_t server;

         conn_list connections;
      };
}