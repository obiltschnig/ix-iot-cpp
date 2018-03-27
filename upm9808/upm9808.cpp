#include <upm/mcp9808.hpp>
#include <iostream>


int main(int argc, char** argv)
{
    try
    {
        upm::MCP9808 tempSensor(0);

        std::cout << "Temperature: " <<
            tempSensor.getTemp() << std::endl;
    }
    catch (std::exception& exc)
    {
        std::cerr << exc.what() << std::endl;
        return 1;
    }
    return 0;
}

