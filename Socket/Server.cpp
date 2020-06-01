#include "Server.hpp"

#include <memory>

auto Messanger::CreateSocket(addr_t addr, port_t port) {
      sock_t res;

      switch (addr.index()) {
         case 0: // linux (int)
            //ASSERT(socket_not_linux_implementation);
            break;
         case 1: // win (SOCKET - uint)
            res = socket(PF_TYPE, SOCK_TYPE, IPPROTO(PROTOCOL));
            break;
         default:
            res = 0;
      }

      return res;
   }

auto Messanger::BindSocket(sock_t sock) {
      addr_t res;

      switch (sock.index()) {
         case 0: {// linux (int)
            ASSERT(bind_not_linux_implementation);
            break;
         }
         case 1: { // win (SOCKET - uint)
            sockaddr addr;
            bind(get<1>(sock), &addr, sizeof(addr));
            res = addr;
            break;
         }
         default: {
            res = "";
         }
      }

      return res;
   }

auto Messanger::Listen(sock_t sock, int queuelen) {
      addr_t res;

      switch (sock.index()) {
         case 0: {// linux (int) 
            ASSERT(listen_not_linux_implementation);
            break;
         }
         case 1: { // win (SOCKET - uint)
            listen(get<1>(sock), queuelen);
            res = BindSocket(sock);
            break;
         }
         default: {
            res = "";
         }
      }

      return res;
   }

auto Messanger::Send(sock_t sock, data_t data) {
      int res = 0;

      switch (sock.index()) {
         case 0: { // linux (int) 
            ASSERT(send_not_linux_implementation);
            break;
         }
         case 1: { // win (SOCKET - uint)
            res = send(get<1>(sock), data.data(), data.size(), MSG_OOB);
            break;
         }
         default: { break; }
      }

      return res;
   }

auto Messanger::Recv(sock_t sock, size_t part_size) {
      data_t res;

      switch (sock.index()) {
         case 0: { // linux (int) 
            ASSERT(send_not_linux_implementation);
            break;
         }
         case 1: { // win (SOCKET - uint)
            std::vector<char> tmp(part_size, '\0');

            auto count = recv(get<1>(sock), tmp.data(), part_size, MSG_OOB);

            while (count) {
               std::for_each(tmp.data(), tmp.data() + count, [&res](auto& el){ res.push_back(el); });
               count = recv(get<1>(sock), tmp.data(), part_size, MSG_OOB);
            }

            break;
         }
         default: { break; }
      }

      return res;
   }

