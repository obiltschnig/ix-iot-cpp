#include "ip_connection.h"
#include "bricklet_temperature.h"
#include <cstdint>
#include <iostream>

const char* HOST = "localhost";
const int16_t PORT = 4223;
const char* BRICKLET_UID = "abc";

int main(int argc, char** argv)
{
    IPConnection ipcon;
    ipcon_create(&ipcon);

    if (ipcon_connect(&ipcon, HOST, PORT) < 0) 
    {
        std::cerr << "Cannot connect to daemon." << std::endl;
        return 1;
    }

    Temperature tempSensor;
    temperature_create(&tempSensor, BRICKLET_UID, &ipcon);

    int16_t temp;
    int rc = 0;
    if (temperature_get_temperature(&tempSensor, &temp) == 0) 
    {
		std::cout << "Temperature: " << 
                temp/100.0 << std::endl;
    }
    else
    {
        std::cerr << "Cannot get temperature." << std::endl;
        rc = 2;
    }

    temperature_destroy(&tempSensor);
    ipcon_destroy(&ipcon); 

    return rc;
}

