#include "../Header/socketCommunicate.h"
#include "../Header/utility.h"
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
    setup_motor();

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
void socketCommunicate::EventServer()
{
    if (client_fd[0].revents & POLLRDNORM) /*New client connection*/
    {
        clilen = sizeof(client_address);
        connfd = accept(listenfd, (struct sockaddr *)&client_address, &clilen);

        for (i = 1; i < POLL_SIZE; i++)
        {
            if (client_fd[i].fd < 0)
            {
                cout << "Socket have fd: " << connfd << " is connected" << endl;
                client_fd[i].fd = connfd; /*Save descriptor*/
                break;
            }
        }

        if (i == POLL_SIZE)
        {
            char buf[20];
            strcpy(buf, "too many connection"); /*Max index in client[] array*/
            write(connfd, &buf, 20);
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

    while (true)
    {

        nready = poll(client_fd, maxi + 1, -1);
        EventServer();

        for (i = 1; i <= maxi; i++) /**Check all client for data*/
        {
            //place to declare package
            //unsigned char byte_r;

            if ((sockfd = client_fd[i].fd) < 0)
                continue;

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
                    }
                    else
                    {
                        cout << "Socket have fd : " << sockfd << "is disconnected eror" << endl;
                        cerr << "read eror" << endl;
                    }
                }
                else if (n == 0)
                {
                    /**connection close by client*/
                    cout << "Socket have fd : " << sockfd << "is disconnected" << endl;
                    power = false;
                    close(sockfd);
                    client_fd[i].fd = -1;
                }
                else
                {
                    //recieved package in here
                    if (byte_r == '!')
                        analyReceivingData();
                }

                if (--nready <= 0)
                    break; /** no more readable descriptors */
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
    case str2int("JoyR"):
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
        cout << "pitch: " << pitch << " roll: ";

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
        roll = atoi(str2Char(message));
        cout << roll << endl;

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
        break;
    case str2int("JoyL"):
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
        cout << "throttle: " << throttle << " yaw: ";
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
        //cout<<yaw<<endl;
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
    return;
}

void socketCommunicate::handleMotorCut(string message)
{
    if (strcmp(str2Char(message), "on") == 0)
    {
        
        active_motor();
    }
    else if (strcmp(str2Char(message), "off") == 0)
    {
        stop_motor();
    }

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
