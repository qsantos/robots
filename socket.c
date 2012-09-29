/*\
 *  This is an awesome programm simulating awesome battles of awesome robot tanks
 *  Copyright (C) 2012  Thomas GREGOIRE, Quentin SANTOS
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
\*/

#include "socket.h"

#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

Socket TCP_Connect(const char* IP, Port port)
{
  Socket sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
    return -1;
	
  struct sockaddr_in host;
  memset(&host, 0, sizeof(struct sockaddr_in));
  inet_pton(AF_INET, IP, &host.sin_addr);
  host.sin_family = AF_INET;
  host.sin_port   = htons(port);
	
  Socket res = connect(sock, (struct sockaddr*)&host, sizeof(struct sockaddr));
  if (res < 0)
  {
    close(sock);
    return -1;
  }
	
  return sock;
}

Socket TCP_ListenTo(const char* IP, Port port)
{
  Socket sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
    return -1;
	
  struct sockaddr_in host;
  memset(&host, 0, sizeof(struct sockaddr_in));
  inet_pton(AF_INET, IP, &host.sin_addr);
  host.sin_family = AF_INET;
  host.sin_port   = htons(port);
	
  int v = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(int));
	
  Socket res = bind(sock, (struct sockaddr*)&host, sizeof(struct sockaddr));
  if (res < 0)
  {
    close(sock);
    return -1;
  }
	
  res = listen(sock, 10);
  if (res < 0)
  {
    close(sock);
    return -1;
  }
	
  return sock;
}

Socket TCP_Listen(Port port)
{
  return TCP_ListenTo("0.0.0.0", port);
}

Socket TCP_Accept(Socket sock)
{
  Socket client = accept(sock, NULL, NULL);
  if (client < 0)
    return -1;
  
  return client;
}
