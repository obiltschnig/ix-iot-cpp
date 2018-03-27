#include "MQTTClient.h"
#include <system_error>
#include <sstream>
#include <iostream>

using namespace std::literals::string_literals;

const int MQTT_TIMEOUT_MSECS = 10000;

void sendTemperature(const std::string& apiKey, const std::string device, float temperature)
{
    std::ostringstream jsonStream;
    jsonStream <<
        "{"
            "\"protocol\": \"v2\","
            "\"device\": \"" << device << "\","
            "\"at\": \"now\","
            "\"data\": {"
                "\"temperature\":" << temperature <<
            "}"
        "}";
    const auto json{jsonStream.str()};
    std::string topic{apiKey + "/streams"s};

    MQTTClient client;
    int rc = MQTTClient_create(&client, "ssl://mqtt.carriots.com:8883", "SampleClient", MQTTCLIENT_PERSISTENCE_NONE, 0);
    if (rc != MQTTCLIENT_SUCCESS)
        throw std::system_error(rc, std::generic_category(), "MQTTClient_create failed");

    MQTTClient_SSLOptions sslOptions = MQTTClient_SSLOptions_initializer;
    sslOptions.enableServerCertAuth = false;

    MQTTClient_connectOptions options = MQTTClient_connectOptions_initializer;
    options.username = apiKey.c_str();
    options.ssl = &sslOptions;

    rc = MQTTClient_connect(client, &options);
    if (rc != MQTTCLIENT_SUCCESS)
    {
        MQTTClient_destroy(&client);
        throw std::system_error(rc, std::generic_category(), "MQTTClient_connect failed");
    }

    MQTTClient_message message = MQTTClient_message_initializer;
    message.payload = const_cast<char*>(json.data());
    message.payloadlen = json.length();

    MQTTClient_deliveryToken token;
    rc = MQTTClient_publishMessage(client, topic.c_str(), &message, &token);
    if (rc == MQTTCLIENT_SUCCESS)
        rc = MQTTClient_waitForCompletion(client, token, MQTT_TIMEOUT_MSECS);

    MQTTClient_disconnect(client, MQTT_TIMEOUT_MSECS);
    MQTTClient_destroy(&client);

    if (rc != MQTTCLIENT_SUCCESS)
        throw std::system_error(rc, std::generic_category(), "Failed to send message");
}

int main(int argc, char** argv)
{
    const auto apiKey{"ce3a2a76dbd871f0ef773e21bf79a49cae2f4d8a27cf463c9a0508cd92a6e5b5"s};
    const auto device{"RaspberryPi2@obiltschnig.obiltschnig"s};

    try
    {
        sendTemperature(apiKey, device, 23);
    }
    catch (std::system_error& exc)
    {
        std::cerr << exc.what() << " (" << exc.code() << ")" << std::endl;
    }

    return 0;
}
