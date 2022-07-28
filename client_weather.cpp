#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <string>
#include <iostream>

std::string host = "api.openweathermap.org";

/*strings to create a full path*/
std::string path = "data/2.5/weather";
std::string city_name = "Kyiv";

std::string key_append = "&appid=";
std::string key = "53ee40e2d4ea3a13aab2839412d80295";

std::string units = "&units=metric";

/*Connects to host and returns host_server_socker_descriptor*/
int connect_to(const char* hostname, const char* port)
{
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;

    addrinfo *peer_address;

    if (getaddrinfo(hostname, port, &hints, &peer_address))
    {
        printf("getaddrinfo() error!\n");
        exit(1);
    }

    int server= socket(peer_address->ai_family, peer_address->ai_socktype, peer_address->ai_protocol);

    if (server == -1)
    {
        printf("socket() error!\n");
        exit(1);
    }

    if (connect(server, peer_address->ai_addr, peer_address->ai_addrlen))
    {
        printf("connect() error!\n");
        exit(1);
    }

    freeaddrinfo(peer_address);

    printf("Connection completed!\n");

    return server;
}

/*sends GET request to HOST using PATH*/
void send_get_tos(int server, const char* host, const char* path, const char* port)
{
    char req[2048];

    sprintf(req, "GET /%s HTTP/1.1\r\n", path);
    sprintf(req + strlen(req), "Host: %s\r\n", host, port);
    sprintf(req + strlen(req), "\r\n");

    send(server, req, strlen(req), 0);

    printf("Request:\n%s\n", req);
}

/*a small json parsing*/
enum type {STR, NUM};
void get_field(const char* field_name, char* json, type t)
{
    char* p = strstr(json, field_name) + strlen(field_name) + 1 + 1;

    if (t == STR)
    {
        p++;

        while(*p != '"')
        {
            printf("%c", *p);
            p++;
        }
    }
    else
    {
        while(*p != ',' && *p != '}')
        {
            printf("%c", *p);
            p++;
        }
    }
}

void parse_weather(char* json)
{
    
    printf("Name: ");
        get_field("name", json, STR);
    printf("\n");

    printf("Country: ");
        get_field("country", json, STR);
    printf("\n\n");


    printf("Description: ");
        get_field("description", json, STR);
    printf("\n\n");

    printf("Temperature: ");
        get_field("temp", json, NUM);
    printf("\n");

    printf("Min: ");
        get_field("temp_min", json, NUM);

     printf(" Max: ");
        get_field("temp_max", json, NUM);
    printf("\n");

}

int main(int argc, char* argv[])
{

    if (argc < 2)
    {
        printf("usage: ./client cityName\n");
        return 1;
    }

    city_name = argv[1];
    std::string full_path = path + "?q=" + city_name + key_append + key + units;

    int server = connect_to(host.c_str(), "http");
    send_get_tos(server, host.c_str(), full_path.c_str(), "http");

    char resp[4096];
    int br = recv(server, resp, 4096, 0);

    close(server);

    /*get body of response*/
    char* body = strstr(resp, "\r\n\r\n") + 4;
    parse_weather(body);

    return 0;
}