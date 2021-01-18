#include "../Header/socketCommunicate.h"
#include "../Header/utility.h"
#include <pthread.h>
#include <time.h>
static bool connect_status = true, check_rev = true;
static unsigned int send_time;
pthread_t thread_check;
static bool connected = false;
//static unsigned long time_re, last_time, time_de = 4 * 1024 * 1024;

void socketCommunicate::Init() // init the value
{
    bzero((struct sockaddr *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(LOCAL_HOST_SOCK);
    power = false;
    saveVideo = false;
    pw_CutMotor = false;
    m_clockwise = false;
    roll = pitch = yaw = 1500;
    throttle = 1000;
    int rc = 0;
    int temp = 1;
    int chek;
    //setup_motor();

    while (true)
    {
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        rc = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &temp, sizeof(int));

        if (rc < 0)
        {
            cout << "setsockopt failed!" << strerror(errno) << endl;
        }

        chek = bind(listenfd, (struct sockaddr *)&server_address, sizeof(server_address));

        if (chek < 0)
        {
            cout << "Sorry port is occupied so can't start server,please waitting ! " << strerror(errno) << endl;
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
    for (i = 1; i < POLL_SIZE; i++)
    {
        client_fd[i].fd = -1;
    }

    maxi = 0;
    cout << "Waiting for connection........ \n"
         << endl;
}
void *checkClient(void *fd)
{
    char buf = 'a';
    static unsigned int pre_time;
    pre_time = micros();
    while (connected)
    {

        if (check_rev == false)
        {
            connect_status = false;
        }
        else
        {
            connect_status = true;
        }

        if (write(*((int *)fd), &buf, sizeof(buf)) < 0)
        {
            connect_status = false;
        }
  //      cout<<buf<<endl;
        check_rev = false;
        send_time = micros();
        int32_t sleepTime = pre_time + 3000000 - micros();
        if(sleepTime > 0&& sleepTime < 3000000)
            usleep(sleepTime);
        pre_time = micros();
    }
    return 0;
}

void socketCommunicate::EventServer()
{

    if (client_fd[0].revents & POLLRDNORM) /*New client connection*/
    {

        clilen = sizeof(client_address);
        connfd = accept(listenfd, (struct sockaddr *)&client_address, &clilen);

        int temp = 1, so_keepalive = 1, tcp_timeout = 1000;
        for (i = 1; i < POLL_SIZE; i++)
        {
            if (client_fd[i].fd < 0)
            {
                cout << "Socket have fd: " << connfd << " is connected" << endl;
                client_fd[i].fd = connfd; /*Save descriptor*/
                connect_status = true;
                check_rev = true;
                //pthread_cancel(thread_check);
                //usleep(100);
                connected = true;
                pthread_create(&thread_check, NULL, checkClient, (void *)&connfd);
                break;
            }
        }
        sleep(2);
        if (i == POLL_SIZE)
        {
            //char buf[20];
            //strcpy(buf, "too many connection"); /*Max index in client[] array*/
            //write(connfd, &buf, 20);
        }

        client_fd[i].events = POLLRDNORM;

        if (i > maxi)
            maxi = i;

        if (--nready <= 0)
            return; /*No more readable descriptors*/
    }
}

void socketCommunicate::ConnectClient()
{
    static unsigned int rev_time = micros();
    bool cout_dis = true;
    while (true)
    {
        nready = poll(client_fd, maxi + 1, 2000);

        EventServer();
        //cout<< "where 3"<<endl;
        for (i = 1; i <= maxi; i++) /**Check all client for data*/
        {
//            cout<< "where 4"<<endl;
            //place to declare package
            //unsigned char byte_r;
            // check here for status
            usleep(100);
            if ((sockfd = client_fd[i].fd) < 0)
                continue;
            if (!connect_status)
            {
                power = false;
                cout << "Socket have fd : " << sockfd << " is disconnected" << endl;
                close(sockfd);
                client_fd[i].fd = -1;
                connect_status = true;
                connected = false;
                // pthread_cancel(thread_check);
                sleep(1);
            }
            else
            {
                if (client_fd[i].revents & (POLLRDNORM | POLLERR))
                {
                    
                    if ((n = read(sockfd, &byte_r, sizeof(byte_r))) < 0)
                    {
                        if (errno == ECONNRESET)
                        {
                            /**connection rest by client*/
                            power = false;
                            cout << "Socket have fd : " << sockfd << "is disconnected" << endl;
                            close(sockfd);
                            client_fd[i].fd = -1;
                            connected = false;
                            // pthread_cancel(thread_check);
                            sleep(1);
                        }
                        /*else if (errno == ENETRESET)
                        {
                            /**connection rest by client*/
                           // power = false;
                           // cout << "Socket have fd : " << sockfd << "disconnected" << endl;
                            //close(sockfd);
                           // client_fd[i].fd = -1;
                           // connected = false;
                            // pthread_cancel(thread_check);
                       // }*/
                        else

                        {
                            cout << "Socket have fd : " << sockfd << "is disconnected eror" << endl;
                            cerr << "read eror" << endl;
                            connected = false;
                            // pthread_cancel(thread_check);
                        }
                    }
                    else if (n == 0)
                    {
                        /**connection close by client*/
                        cout << "Socket have fd : " << sockfd << "is disconnected" << endl;
                        power = false;
                        close(sockfd);
                        client_fd[i].fd = -1;
                        connected = false;
                        // pthread_cancel(thread_check);
                    }
                    else
                    {
                        //recieved package in here
                        if (byte_r == '!')
                            analyReceivingData();
                        else if (byte_r = 'b')
                        {
                            cout << "receive b" << endl;
                            check_rev = true;
                        }
                    }

                    if (--nready <= 0)
                        break; /** no more readable descriptors */
                }
            }
        }
    }

    close(client_fd[0].fd);
}

void socketCommunicate::analyReceivingData()
{
    int lenMess = 0;
    string message;
    char *p;
    while (true)
    {
        read(sockfd, &byte_r, sizeof(byte_r));
        if (byte_r < '0' || byte_r > '9')
            break;
        lenMess = lenMess * 10 + (byte_r - '0');
    }
    for (int i = 0; i < lenMess; ++i)
    {
        /* code */
        read(sockfd, &byte_r, sizeof(byte_r));
        message.push_back(byte_r);
    }
    cout << message << " : ";

    switch (str2int(str2Char(message)))
    {
    case str2int("pitch"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        if (lenMess == 0)
            break;
        pitch = 0;
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        pitch = atoi(str2Char(message));
        cout << pitch << endl;

        break;
    case str2int("roll"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        if (lenMess == 0)
            break;
        roll = 0;
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        roll = atoi(str2Char(message));
        cout << pitch << endl;

        break;
    case str2int("power"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        cout << message << endl;
	throttle = 1000;
	if(strcmp(str2Char(message),"on")==0)
	{
		power = true;
	}
	else
	{
		power = false;
	}
        break;
    case str2int("throttle"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        throttle = 0;
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        throttle = atoi(str2Char(message));
        cout << throttle << endl;

        //cout<<yaw<<endl;
        break;
    case str2int("yaw"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        yaw = 0;
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        yaw = atoi(str2Char(message));
        cout << yaw << endl;
        break;

    case str2int("motor"):
        message.clear();
        lenMess = 0;
        while (true)
        {
            read(sockfd, &byte_r, sizeof(byte_r));
            if (byte_r < '0' || byte_r > '9')
                break;
            lenMess = lenMess * 10 + (byte_r - '0');
        }
        for (int i = 0; i < lenMess; ++i)
        {
            /* code */
            read(sockfd, &byte_r, sizeof(byte_r));
            message.push_back(byte_r);
        }
        cout << message << endl;
        handleMotorCut(message);
        break;
    }
}

void socketCommunicate::handleMotorCut(string message)
{
    // if (strcmp(str2Char(message), "on") == 0)
    // {

    //     active_motor();
    // }
    // else if (strcmp(str2Char(message), "off") == 0)
    // {
    //     stop_motor();
    // }

    // if(strcmp(str2Char(message),"on")==0)
    // {
    //     pw_CutMotor = true;
    //     setUp_motor();

    // }else if(strcmp(str2Char(message),"off")==0)
    // {
    //     pw_CutMotor = false;
    //     dowm_motor();

    // } else if(strcmp(str2Char(message),"cw")==0)
    // {
    //     m_clockwise = true;
    //     if(pw_CutMotor&&m_clockwise)
    //     {
    //         cw_motor(125);
    //     }
    // } else if(strcmp(str2Char(message),"ccw")==0)
    // {
    //     m_clockwise = false;
    //     if(pw_CutMotor&&!m_clockwise)
    //     {
    //         ccw_motor(125);
    //     }
    // }
}

bool socketCommunicate::get_powerStatus()
{
    return power;
}
bool socketCommunicate::get_saveVideoStatus()
{
    return saveVideo;
}
bool socketCommunicate::get_pw_CutMotorStatus()
{
    return pw_CutMotor;
}
bool socketCommunicate::get_m_clockwiseStatus()
{
    return m_clockwise;
}

int socketCommunicate::get_throttle()
{
    return throttle;
}
int socketCommunicate::get_yaw()
{
    return yaw;
}
int socketCommunicate::get_roll()
{
    return roll;
}
int socketCommunicate::get_pitch()
{
    return pitch;
}
