#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <wordexp.h>
#include <ctype.h>
#include "include/main.h"
#include "../Shared/include/executionValidator.h"
#include "../Shared/include/lib.h"
#include "include/options.h"

#define MAX_STREAMS 64
#define MAX_BUFFER 1024
void prepareForSending(char **username, char **password);

//config socket_config;
struct sockaddr_in addr;
static admin_context_p admin_context;


int main(int argc, char ** argv)
{
    initialize_app_context();
    int valid = parse_arguments(argc, argv);
    if(!valid)
    {
        printf("Error creating connection");
        free_struct();
        exit(1);
    }
    admin_context = get_admin_context();
    socket_config();

    //Saludo al usuario y le informo que se va a intentar hacer una conexion al proxy
    printf("Hello! Starting connection...\n");
    //Waiting for conection
    int fd = createConnection();

    char status = 0;//0 desconectado, 1 conectado, 2 quitting.
    while(status!=2)
    {
        //Tengo que loggearme

        requestForLogin(fd,&status);
        if(status == 1)
        {
            //Me conecte exitosamente entonces entro en otro modo
            interaction(fd);
        }
    }
    //CierreDeConexion
    closeConnection(fd);
    return 0;
}


void socket_config()
{
    memset((void*)&addr,0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(admin_context->port);
    addr.sin_addr.s_addr = inet_addr(admin_context->listenaddress);
}
int createConnection()
{
    int fd, ret;
    struct sctp_initmsg initmsg;
    struct sctp_event_subscribe events;


    //Creo una conexion SCTP con los valores (default 9090, 127.0.0.1)
    if((fd = socket(AF_INET,SOCK_STREAM,IPPROTO_SCTP)) == -1 )
    {
        printf("An error has ocurred while creating SCTP socket\n");
        perror("socket");
        closeConnection(fd);
        exit(1);
    }
    //Configuro la cantidad de streams disponible para el socket
//    memset(&initmsg,0, sizeof(struct sctp_initmsg));
//    initmsg.sinit_num_ostreams  = MAX_STREAMS;
//    initmsg.sinit_max_instreams = MAX_STREAMS;
//    initmsg.sinit_max_attempts  = MAX_STREAMS;
//    ret = setsockopt(fd,IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(struct sctp_initmsg));
//    if(ret<0)
//    {
//        perror("setsockopt SCTP_INITMSG");
//        closeConnection(fd);
//        exit(1);
//    }

    //Configuro los eventos
//    events.sctp_association_event = 1;
//    events.sctp_data_io_event = 1;
//    ret = setsockopt(fd,IPPROTO_SCTP,SCTP_EVENTS, &events, sizeof(events));
//    if(ret<0)
//    {
//        perror("setsockopt SCTP_EVENTS");
//        closeConnection(fd);
//        exit(1);
//    }
    //Realizo la conexion
    if((ret = connect(fd,(struct sockaddr*)&addr, sizeof(addr))) == -1)
    {
    printf(("An error has ocurred while connecting the SCTP socket\n"));
    perror("connect");
    closeConnection(fd);
    exit(1);
    }
    return fd;
}
void requestForLogin(int fd, char* status)
{
    //Hago la solicitud al servidor
    if (requestLoginToProxy(fd))
    {
        //Respuesta positiva
        loginSuccess(status);
    } else
    {
        //Respuesta negativa
        loginError(fd,status);
    }
}

void loginError(int fd, char* status)
{
    //Aviso que no se puedo autenticar
    printf("Login failed. Do you wish to retry? [Y/n]: ");
    //Pregunto si quiere volver a intentar o si quiere quitear
    char * input = calloc(1,INITIAL_INPUT_SIZE);
    size_t count = fetchLineFromStdin(&input,INITIAL_INPUT_SIZE);
    if(count!=1)
    {
        printf("Please input only Y/n");
    } else
    {
        if(*input == 'Y' || *input == 'y')
        {
            requestForLogin(fd,status);
        }
        else if(*input == 'N' || *input == 'n')
        {
            //Si quitea hago *status = 2;
            *status = 2;
        }
    }
    free(input);
}

void loginSuccess(char* status)
{
    //Aviso que se conecto
    printf("Login succesful\n");
    //Seteo variable para que salga del while
    *status = 1;
}

char requestLoginToProxy(int fd){
    char * usernameInput = calloc(1,INITIAL_INPUT_SIZE);
    char * passwordInput = calloc(1,INITIAL_INPUT_SIZE);
    //Le solicito un usuario
    printf("Please enter your username: ");
    fetchLineFromStdin(&usernameInput,INITIAL_INPUT_SIZE);
    printf("+OK\n");
    //Le solicito una contraseña
    printf("Please enter your password: ");
    fetchLineFromStdin(&passwordInput,INITIAL_INPUT_SIZE);
    prepareForSending(&usernameInput,&passwordInput);


    //En la conexion 9090 le envia con USER name el parametro obtenido del usuario
    int ret = sctp_sendmsg(fd,usernameInput,strlen(usernameInput), NULL, 0, 0, 0, 0, 0, 0);
    if(ret == -1)
    {
        printf("An error has ocurred sending USER info\n");
        perror("sctp_sendmsg");
        closeConnection(fd);
        exit(1);
    }

    //Luego le envia la contraseña con PASS string
    ret = sctp_sendmsg(fd,passwordInput,strlen(passwordInput), NULL, 0, 0, 0, 0, 0, 0);
    if(ret == -1) {
        printf("An error has ocurred sending PASS info\n");
        perror("sctp_sendmsg");
        closeConnection(fd);
        exit(1);
    }
    //Parsea la respuesta del proxy
    char buffer[MAX_BUFFER] = {0};

    ret = sctp_recvmsg(fd, buffer, sizeof(buffer), NULL, 0, 0, 0);

    free(usernameInput);
    free(passwordInput);

//    if( ret == '1')
//    {
//        return 1;
//    } else
//    {
//        return 0;
//    }
    return 1;
    //Devuelve 1 si fue exitoso
    //Devuelve 0 si falla
    //Libera memoria
}

void prepareForSending(char **username, char **password) {
    char * user = calloc(1,INITIAL_INPUT_SIZE + 6);
    strcpy(user,"USER: ");
    strcat(user, *username);
    strcpy(*username,user);
    char * pass = calloc(1,INITIAL_INPUT_SIZE + 5);
    strcpy(pass,"PASS: ");
    strcat(pass, *password);
    strcpy(*password,pass);
    free(user);
    free(pass);
}

void interaction(int fd)
{

    char buffer[MAX_BUFFER];
    int ret;
    wordexp_t p;
    char **w;
    int count;
    while(1)
    {
        if(fgets(buffer,MAX_BUFFER,stdin) == NULL)
        {
            closeConnection(fd);
            exit(-1);
        }
        size_t length = strlen(buffer);
        char * pos;
        if((pos = strchr(buffer,'\n')) != NULL)
        {
            *pos = '\0';
        }

        if(strcmp(buffer,"LISTS")==0)
        {
            ret = sctp_sendmsg(fd,(void*)buffer,length,NULL, 0, 0, 0, 0, 0, 0);
        }
        else if(strncmp(buffer,"STATS",5)==0)
        {
            int response = wordexp(buffer,&p,0);
            if(response != 0)
            {
                printf("There seems to be an internal error\n");
            }
            else
            {
                count = (int)p.we_wordc;
                w = p.we_wordv;
                if(count!=2 || !isdigit(w[1]))
                {
                    printf("Usage: STATS number, number must be a digit\n");
                }
                else
                {
                    ret = sctp_sendmsg(fd,(void*)buffer,length,NULL, 0, 0, 0, 0, 0, 0);
                }
            }
            wordfree(&p);
        }
        else if(strncmp(buffer,"ACTIVE",6)==0)
        {
            int response = wordexp(buffer,&p,0);
            if(response != 0)
            {
                printf("There seems to be an internal error\n");
            }
            else
            {
                count = (int)p.we_wordc;
                w = p.we_wordv;
                if(count==1)
                {
                    ret = sctp_sendmsg(fd,(void*)buffer,length,NULL, 0, 0, 0, 0, 0, 0);
                }
                else if(count==2)
                {
                    if(strcmp(w[1],"1")==0 || strcmp(w[1],"0")==0)
                    {
                        ret = sctp_sendmsg(fd,(void*)buffer,length,NULL, 0, 0, 0, 0, 0, 0);
                    } else
                    {
                        printf("Usage: ACTIVE [BOOL], bool must be 0 or 1");
                    }
                }
            }

            wordfree(&p);
        }
        else if(strncmp(buffer,"FILTER",6)==0)
        {
            int response = wordexp(buffer,&p,0);
            if(response != 0)
            {
                printf("There seems to be an internal error\n");
            }
            else
            {
                count = (int)p.we_wordc;
                w = p.we_wordv;
                if(count==1 || count==2)
                {
                    ret = sctp_sendmsg(fd,(void*)buffer,length,NULL, 0, 0, 0, 0, 0, 0);
                }
            }
            wordfree(&p);
        }
        else if(strcmp(buffer,"HELP")==0)
        {
            printf("LISTS---------------- Lists the possible monitoring options along with its corresponding number\n");
            printf("STATS number--------- Returns the monitoring information corresponding to number\n");
            printf("ACTIVE [BOOL]-------- With no BOOL, returns the current transformation filter's status\n"
                   "                      With BOOL, activates (1) or deactivates (0) the current filter\n");
            printf("FILTER [CMD]--------- With no CMD, returns current transformation filter\n"
                   "                      With CMD (a command name compatible with system(3)) changes the current tranformation filter into CMD\n");
            printf("QUIT----------------- Closes the connection\n");
            printf("HELP----------------- Lists the possible input options\n");


        }
        else if(strcmp(buffer,"QUIT")==0)
        {
            ret = sctp_sendmsg(fd,(void*)buffer,length,NULL, 0, 0, 0, 0, 0, 0);

        }
        else
        {
            printf("Incorrect input, try HELP for options\n");
            ret = -2;
        }

        if(ret == -1)
        {
            printf("An error has ocurred sending the message\n");
            closeConnection(fd);
            exit(1);
        }
    }
}

void parseToArgv()
{

}
void closeConnection(int fd)
{
    printf("Goodbye, hope to see you soon!\n");
    free_struct();
    close(fd);
}
