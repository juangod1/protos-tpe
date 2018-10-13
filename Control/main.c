#include "include/main.h"
#include "include/options.h"
#include "../Shared/include/executionValidator.h"

int main(int argc, char ** argv)
{
    response_p response =malloc(sizeof(response_t));
    initialize_options();
    parse_command(argc,argv,response);
    free(response);

    //Saludo al usuario y le informo que se va a intentar hacer una conexion al proxy
    //Waiting for conection
    waitForConection();

    char status = 0;//0 desconectado, 1 conectado, 2 quitting.
    while(status!=2)
    {
        //Tengo que loggearme
        requestForLogin(&status);
        if(status == 1)
        {
            //Me conecte eitosamente entonces entro en otro modo
            interaction();
        }
    }
    //CierreDeConexion
    closeConection();
    //Saludo de despedida
    return 0;
}

void waitForConection()
{
    //Establezco una conexion SCTP en el puerto 9090
    //Y espero que el servidor me salude.
    //Esto tiene que tener un timeout porque el servidor puede estar caido y nunca contestar
    //Tambien puede ser hecho en otro thread e ir encolando las peticiones del usuario
}

void requestForLogin(char* status)
{
    //Le solicito un usuario
    //Le solicito una contraseña
    //Hago la solicitud al servidor
    if (requestLoginToProxy())
    {
        //Respuesta positiva
        loginSuccess(status);
    } else
    {
        //Respuesta negativa
        loginError(status);
    }
}

void loginError(char* status)
{
    //Aviso que no se puedo autenticar
    //Pregunto si quiere volver a intentar o si quiere quitear
    //Si quitea hago *status = 2;
}

void loginSuccess(char* status)
{
    //Aviso que se conecto
    //Seteo variable para que salga del while
    *status = 1;
}

char requestLoginToProxy(){
    //En la conexion 9090 le envia con USER name el parametro obtenido del usuario
    //Luego le envia la contraseña con PASS string
    //Parsea la respuesta del proxy
    //Devuelve 1 si fue exitoso
    //Devuelve 0 si falla
}

void interaction()
{
    //Aca se que estoy logeado y quiero procesar acciones del usuario con una maquina de estados
    //Leer que es lo que quiere hacer
    char quit = 0;
    while(!quit) {
        function command;
        char* parameter = "";
        //Llamar a la funcion de pablo para leer de input
        //Llamar a la funcion de alguien para parsear lo que me dio pablo
        switch (command) {
            case EXIT:
                quit = 1;
                break;
            case STATS:
                //Hacer request al proxy por los stats
                //Reportar la respuesta
                break;
            case COMMAND:
                //Hacer request para cambio de transformacion.
                //Como esta es una funcion critica hay que agregar cosas en el header como etag para diferenciar entre
                //pedidos de distintos usuarios.
                //Reportar respuesta
                break;
            default:
                //Input incorrecto
                break;
        }
    }
}

void closeConection()
{
    //Cerrar la conexion SCTP
}