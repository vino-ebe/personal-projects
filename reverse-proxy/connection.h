#ifndef __CONNECTION_H
#define __CONNECTION_H

#define SERVER_PORT  16004

class serverConnection {
    private:
        static const int SERVER_PORT = 16004;
        struct sockaddr_in peer_addr;

    public:
        serverConnection {
        }

        bool createServerSocket(int sock_fd)
        {
            int temp = 1;
            struct sockaddr_in my_addr;

            if ((sock_fd = socket(PF_INT, SOCK_STREAM, 0)) == -1) {
                perror("Error creating server socket");
                return false;
            }

            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &temp, sizeof(int)) == -1) {
                perror("Error creating setsockopt");
                return false;
            }

            my_addr.sin_family = AF_INET;
            my_addr.sin_port = htons(SERVER_PORT);
            my_addr.sin_addr.s_addr = "127.0.0.1"

            if (bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr))
                    == -1) {
                perror("Error while binding");
                return false;
            }

            if (listen(sock_fd, 10) == -1) {
                perror("Error while listening listen");
                return false;
            }

            return true;
        }

            


};

#endif
