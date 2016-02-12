//=======================================================================
// Copyright (c) 2015 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

namespace {

const std::size_t UNIX_PATH_MAX = 108;

const char* server_socket_path = "/tmp/asgard_socket";
const char* client_socket_path = "/tmp/asgard_random_socket";

const std::size_t buffer_size = 4096;

//Buffer
char write_buffer[buffer_size];
char receive_buffer[buffer_size];

} //End of anonymous namespace

int main(){
    // Open the socket
    auto socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(socket_fd < 0){
        std::cerr << "asgard:random: socket() failed" << std::endl;
        return 1;
    }

    // Init the client address
    struct sockaddr_un client_address;
    memset(&client_address, 0, sizeof(struct sockaddr_un));
    client_address.sun_family = AF_UNIX;
    snprintf(client_address.sun_path, UNIX_PATH_MAX, client_socket_path);

    // Unlink the client socket
    unlink(client_socket_path);

    // Bind to client socket
    if(bind(socket_fd, (const struct sockaddr *) &client_address, sizeof(struct sockaddr_un)) < 0){
        std::cerr << "asgard:random: bind() failed" << std::endl;
        return 1;
    }

    // Init the server address
    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(struct sockaddr_un));
    server_address.sun_family = AF_UNIX;
    snprintf(server_address.sun_path, UNIX_PATH_MAX, server_socket_path);

    // Send a message
    auto nbytes = snprintf(write_buffer, 4096, "REG_SENSOR random");
    sendto(socket_fd, write_buffer, nbytes, 0, (struct sockaddr *) &server_address, sizeof(struct sockaddr_un));

    // Close the socket
    close(socket_fd);

    return 0;
}
