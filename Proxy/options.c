//
// Created by juangod on 08/10/18.
//

#include "../Shared/include/executionValidator.h"
#include "include/options.h"

void initialize_options()
{
    option_register("Error Specification",'e',error_specification,"Specifies the file where stderr is rerouted to. By defect /dev/null");
    option_register("Help",'h',help,"Prints out the help text.");
    option_register("Management Direction",'L',management_direction,"Specifies the route where the Management service will act.");
    option_register("Replacement Message",'m',replacement_message,"Specifies the message to replace filtered text.");
    option_register("Censored Mediatype",'M',censored_mediatype,"Censored media type list");
    option_register("Management port",'o',management_port,"Specifies the SCTP port the management server listens to. By defect 9090");
    option_register("Local Port",'p',local_port,"Specifies the TCP port for incoming POP3 connections. By defect 1110");
    option_register("Origin Port",'P',origin_port,"Specifies the TCP port where the POP3 server is found in the origin server. By defect 110");
    option_register("Command Specification",'t',command_specification,"Command utilized for external transformations. Compatible with system(3). By defect applies no transformations.");
    option_register("Version",'v',version,"Prints version information.");
}

void error_specification()
{
    //TODO: has to receive a file direction and use it to specify the file to print out the stderr.
    printf("This is error specification!\n");fflush(stdout);

}

void management_direction()
{
    //TODO: has to receive a direction and use it to specify the route of the management service.
    printf("This is management direction!\n");fflush(stdout);
}
void replacement_message()
{
    //TODO: has to receive a message and use it as a replacement for something.
    printf("This is replacement message!\n");fflush(stdout);

}
void censored_mediatype()
{
    //TODO: has to receive a list of media types and apply a condition for the server to censor them <<USE PROTOS TP 2>>
    printf("This is censored mediatype!\n");fflush(stdout);

}
void management_port()
{
    //TODO: has to receive a port and specify it as the SCTP port the management server will listen to
    printf("This is management port!\n");fflush(stdout);
}
void local_port()
{
    //TODO: has to receive a port and specify
    printf("This is local port!\n");fflush(stdout);
}
void origin_port()
{
    //TODO: has to receive a port and specify it as the origin server POP3 port
    printf("This is origin port!\n");fflush(stdout);
}
void command_specification()
{
    //TODO: has to receive a command from stdin and apply it to the content that passes through proxy. <<USE PROTOS TP 1>>
    printf("This is command_specification!\n");fflush(stdout);

}
void version()
{
    //TODO: has to print out version
    printf("This is version!\n");fflush(stdout);

}