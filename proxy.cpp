#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>

using namespace std;

string message_to_client();
string open_and_read_file();
void cache_file(string msg);
int get_code(string msg);

int main()
{
    int proxy = socket(AF_INET,SOCK_STREAM,0);    // create socket
    if (proxy < 0)
    {
      cout <<strerror(errno);
      return 0;
    }
    cout << "Proxy is running at 9000"<<endl;
    /*** server address details *****/
    struct sockaddr_in proxy_address;
    proxy_address.sin_family = AF_INET;
    proxy_address.sin_addr.s_addr = INADDR_ANY;
    proxy_address.sin_port = htons(9000);

    if (bind(proxy, (struct sockaddr *)&proxy_address, sizeof(struct sockaddr))<0){   // bind the socket address
        cout <<strerror(errno);
        return 0;
    }
    // listen at port 8000
    if (listen(proxy, 5) <0){
       cout <<strerror(errno);
       return 0;
    }
    while(1){
         struct sockaddr_in client_address;
         int addrlen = sizeof(client_address);
         int con_sock;
         cout << "Proxy wating....."<<endl;
         if ((con_sock = accept(proxy, (struct sockaddr *)&client_address,
                       (socklen_t*)&addrlen))<0);
         cout << "proxy connected to client"<<endl;
         char recv_buffer[1024] = {0};
         recv(con_sock,recv_buffer,sizeof(recv_buffer),0);    //recieve the request
         //cout <<recv_buffer<<endl;
         ifstream infile;
         infile.open("index.html");
         string msg;
         if (!infile){           // if the file is not there recive the file contents from server
              cout << "REQUESTED NOT IN PROXY"<<endl;
              msg = message_to_client();
              if(get_code(msg)==200){
                  cache_file(msg);
              }

         }
         else{
            infile.close();
            msg = open_and_read_file();   // if the file is cached ,open the file and read the contents
            cout <<msg<<endl;

         }
         const char *ptr = &msg[0];
         int s = send (con_sock,ptr ,strlen(ptr),0);
         close(con_sock);
   }
   close(proxy);
   return 0;

}


int get_code(string msg){
     string code="";
     code = msg.substr(9,3);
     return stoi(code);
}
string message_to_client(){
     string h = "Hello from proxy";
     char *hello = &h[0];
     int proxy_client = socket(AF_INET,SOCK_STREAM,0);
     if (proxy_client<0){
          cout << strerror(errno);
          return 0;
     }
     //server address details
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8000);
    if(connect(proxy_client, (struct sockaddr *)&server_address, sizeof(server_address)) < 0){
         string msg ;
         msg = "HTTP/1.1 500\nContent-Type: text/html\n\n<html><body><H1>500  Internal Error</H1>\n<p>Cannot connect to the server</p></body></html>";
         return msg;
    }
    cout << "Connected to the server"<<endl;
    char buffer[1024] = {0};
    send(proxy_client , hello , strlen(hello) , 0 );
    int r  = read( proxy_client , buffer, 1024);
    cout << "Recieving from server......"<<endl;
    string msg(buffer);
    return msg;
}

void cache_file(string msg){
     ofstream outfile;
     outfile.open("index.html");
     const char* ptr = &msg[0];
     outfile.write(ptr,strlen(ptr));
     outfile.close();
}

string open_and_read_file(){
    ifstream infile;
    infile.open("index.html");       // open index html
    string msg;
    char index_buffer[9999] = {""};
    int index =0;
    while (infile >> index_buffer[index]){            // read contents of the html file into a buffer
       index++;
    }
    infile.close();                              // close the file
    string index_string(index_buffer);
    msg =  index_string;
    return msg;
}
