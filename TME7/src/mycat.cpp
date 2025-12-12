#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
using namespace std;

//reprise du td
void fullwrite(int fd, const char* data, size_t size){
    size_t len = 0;
    while(len < size){
        len += write(fd, data + len, size - len);
    }
}


int main(int argc, char *argv[]) {
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    if(argc < 2){
        while(true){
            ssize_t n = read(STDIN_FILENO, buffer, BUFFER_SIZE);
            if(n == 0)break;
            if(n < 0){
                cerr<<"pb read"<<endl;
                continue;
            }
            fullwrite(STDOUT_FILENO, buffer, n);   
        }
    }
    else{
        int fd;
        for(int i = 1; i < argc; ++i){
            
            fd = open(argv[i], O_RDONLY);
            if(fd == -1){
                cerr<<"pb open"<<endl;
                continue;
            }

            while(true){
                ssize_t n = read(fd, buffer, BUFFER_SIZE);
                if(n == 0)break;
                if(n < 0){
                    if(errno == EINTR) continue;
                    cerr<<"pb read"<<endl;
                    close(fd);
                    return 1;
                }
                fullwrite(STDOUT_FILENO, buffer, n);
            }

            close(fd);
        }
    }
    return 0;
}
//Q2
// Oui