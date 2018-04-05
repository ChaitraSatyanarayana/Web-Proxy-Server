#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <fstream>
using namespace std;

string open_the_file();

int main(){

    int server = socket(AF_INET,SOCK_STREAM,0);    // create socket
    if (server < 0)
    {
      cout <<strerror(errno);
      return 0;
    }
    cout << "Server is running at 8000"<<endl;
    /*** server address details *****/
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8000);

    if (bind(server, (struct sockaddr *)&server_address, sizeof(struct sockaddr))<0){   // bind the socket address
        cout <<strerror(errno);
        return 0;
    }

    // listen at port 8000
    if (listen(server, 5) <0){
       cout <<strerror(errno);
       return 0;
    }
    while(1){
         struct sockaddr_in client_address;
         int addrlen = sizeof(client_address);
         int con_sock;
         cout << "Server waiting......"<<endl;
         if ((con_sock = accept(server, (struct sockaddr *)&client_address,
                       (socklen_t*)&addrlen))<0);
         cout << "Server connected!"<<endl;
         char recv_buffer[9999] = {0};
         recv(con_sock,recv_buffer,sizeof(recv_buffer),0);    //recieve the request
         string msg = open_the_file();
         const char *ptr = &msg[0];
         char send_buffer[9999];
         strcpy(send_buffer,ptr);
         int s = send (con_sock,ptr ,strlen(ptr),0);

         close(con_sock);
   }
   close(server);
   return 0;
}

string open_the_file(){
    ifstream infile;
    infile.open("index.html");       // open index html
    string msg;
    if (!infile){
        msg =  "HTTP/1.1 404\nContent-Type: text/html\n\n<html><body><H1>404  Not found</H1></body></html>";
    }
    else{
        char index_buffer[9999] = {""};
        int index =0;
        while (infile >> index_buffer[index]){            // read contents of the html file into a buffer
            index++;
         }
        cout << index_buffer;
        infile.close();                              // close the file
        string index_string(index_buffer);
        msg = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n" + index_string;
    }
    return msg;
}
