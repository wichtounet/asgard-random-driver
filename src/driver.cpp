//=======================================================================
// Copyright (c) 2015-2016 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include "asgard/driver.hpp"

#include <random>

namespace {

// Configuration (this should be in a configuration file)
const char* server_socket_path = "/tmp/asgard_socket";
const char* client_socket_path = "/tmp/asgard_random_socket";
const std::size_t delay_ms = 5000;

asgard::driver_connector driver;

// The remote IDs
int source_id = -1;
int sensor_id = -1;

void stop(){
    std::cout << "asgard:random: stop the driver" << std::endl;

    asgard::unregister_sensor(driver, source_id, sensor_id);
    asgard::unregister_source(driver, source_id);

    // Unlink the client socket
    unlink(client_socket_path);

    // Close the socket
    close(driver.socket_fd);
}

void terminate(int){
    stop();

    std::exit(0);
}

} //End of anonymous namespace

int main(){
    // Open the connection
    if(!asgard::open_driver_connection(driver, client_socket_path, server_socket_path)){
        return 1;
    }

    //Register signals for "proper" shutdown
    signal(SIGTERM, terminate);
    signal(SIGINT, terminate);

    // Register the source and sensors
    source_id = asgard::register_source(driver, "random");
    sensor_id = asgard::register_sensor(driver, source_id, "RANDOM", "rand_100");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(1, 100);

    while(true){
        int value = distribution(gen);

        asgard::send_data(driver, source_id, sensor_id, value);

        // Wait some time before messages
        usleep(delay_ms * 1000);
    }

    stop();

    return 0;
}
