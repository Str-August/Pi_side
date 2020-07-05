#include "../Header/socketCommunicate.h"
void socketCommunicate::Init()
{
    bzero((struct sockaddr *)&server_address,sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(LOCAL_HOST_SOCK);
    int rc = 0;
    int temp = 1;
    int chek;

    while(true)
    {
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        rc = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &temp, sizeof(int));

        if (rc < 0)
        {
            cout << "setsockopt failed!" << strerror(errno) <<endl;
        }

        chek = bind(listenfd, (struct sockaddr *)&server_address, sizeof(server_address));

        if(chek<0)
        {
            cout << "Sorry port is occupied so can't start server,please waitting ! "<< strerror(errno) <<endl;
            sleep(3);
        }
        else
        {
            break;
        }

    }
    listen(listenfd, POLL_SIZE);


    client_fd[0].fd = listenfd;
    client_fd[0].events = POLLRDNORM;

    /*intit client_fd array is emty which value -1*/
    for( i = 1 ; i< POLL_SIZE ; i++)
    {
        client_fd[i].fd = -1;

    }

    maxi = 0;
    cout<<"Waiting for client......... \n"<<listenfd<<endl;


}
void socketCommunicate::EventServer()
{
    if(client_fd[0].revents & POLLRDNORM) /*New client connection*/
    {
        clilen = sizeof(client_address);
        connfd = accept(listenfd, (struct sockaddr *)&client_address, &clilen);

        for(i =1 ; i< POLL_SIZE; i++)
        {
            if(client_fd[i].fd<0)
            {
                cout<<"Socket have fd: "<< connfd <<" is connected"<<endl;
                client_fd[i].fd = connfd;/*Save descriptor*/
                break;
            }
        }

        if(i == POLL_SIZE)
        {
            strcpy(buf,"too many client");/*Max index in client[] array*/
            write(connfd,buf,MAXLINE);
        }

        client_fd[i].events = POLLRDNORM;

        if(i>maxi)
            maxi = i;

        if(--nready <=0 )
            return; /*No more readable descriptors*/
    }

}

void socketCommunicate::ConnectClient()
{

    while(true)
    {

        nready = poll(client_fd, maxi +1, -1);
        EventServer();

        for( i = 1; i<= maxi ; i++) /**Check all client for data*/
        {
            //place to declare package
            unsigned char BYTE;

            if((sockfd = client_fd[i].fd) < 0)
                continue;

            if( client_fd[i].revents & (POLLRDNORM | POLLERR))
            {
                if ( (n = read(sockfd, &BYTE , sizeof(BYTE))) <0)
                {
                    if( errno == ECONNRESET)
                    {
                        /**connection rest by client*/
                        
                        close(sockfd);
                        client_fd[i].fd =-1;
                    }
                    else
                    {
                        cerr<<"read eror"<<endl;
                    }
                }
                else if (n==0)
                {
                    /**connection close by client*/
                    cout<<"Socket have fd : "<< sockfd <<"is closing"<<endl;
                    
                    close(sockfd);
                    client_fd[i].fd = -1;
                }
                else
                {
                    //recieved package in here
                    cout<< "recieved package!!"<<endl;
                }

                if (--nready <= 0)
                    break; /** no more readable descriptors */
            }
        }
    }

    close(client_fd[0].fd);
}