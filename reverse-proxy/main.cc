#include<iostream>
#include<pthread.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<sys/un.h>
#include<errno.h>
#include<string>
#include<vector>


using namespace std;

bool createServerSocket(int& sock_fd);
bool createServerThread(pthread_t& thread, thread_data tdata);
extern "C" void* server_thread(void* data) ;
bool setEpollEvents(int epfd, int fd, bool nb) ;

struct thread_data {
    int fd;
};

int main()
{
    int server_sock_fd ;
    if (!createServerSocket(server_sock_fd)) {
        cout<<"Error creating server socket. Exiting..."<<endl;
        exit(0);
    }

    pthread_t sthread;
    thread_data tdata;
    tdata.fd = server_sock_fd;

    if (!createServerThread(sthread, tdata)) {
        cout<<"Error creating server thread. Exiting..."<<endl;
        exit(0);
    }

    vector<pthread_t> v_threads;

    v_threads.push_back(sthread);

    // Wait for threads to finish
    for (int i = 0; i < v_threads.size(); i++) {
        pthread_join(v_threads[i], NULL);
    }
}

extern "C" void* server_thread(void* data) {
    int epfd;
    thread_data *tdata = (thread_data*)data;

    if ((epfd = epoll_create(MAX_CLIENT_CONNECTIONS)) == -1) {
        cout<<"Error creating epoll client. Exiting...."<<endl;
        return NULL;
    }

    int server_fd = tdata->fd;

    setEpollEvents(epfd, server_fd, false);

    struct epoll_event events[MAX_CLIENT_CONNECTIONS];
    int num_events;
    struct sockaddr_in client_addr;
    int sock_size = sizeof(struct sockaddr_in);

    while ((num_events = epoll_wait(epfd, events, MAX_CLIENT_CONNECTIONS, -1)) > 0) {
        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == server_fd) {
                int fd;

                if ((fd = accept(server_fd, (struct sockaddr_in*)&client_addr, &sock_size)) == -1) {
                    cout<<"Error in accepting new connection.Exiting..."<<endl;
                    return NULL;
                }

                // Handle new client connection
                // Make it non blocking and set the fd ready for read
                setEpollEvents(epfd, fd, false);

                cout<<"Received connection from client"<<endl;

                // add the connection to map
        } // for num_events
    } // while epoll_wait
}

bool setEpollEvents(int epfd, int fd, bool nb) {
    struct epoll_event epc_ev;

    if (nb) {
        int options = fcntl(fd, F_GETFL);
        fcntl(fd, F_SETFL, options | O_NONBLOCK);
    }

    epc_ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    epc_ev.data.fd = fd;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epc_ev) == -1) {
        close(epfd);
        return false;
    }
}

bool createServerThread(pthread_t& thread, thread_data tdata) {
    int res;
    if (res = pthread_create(&thread, NULL, server_thread, (thread_data*)tdata) != 0) {
        return false;
    }

    return true;
}

bool createServerSocket(int& sock_fd)
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
