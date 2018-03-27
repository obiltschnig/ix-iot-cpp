#include "MQTTClient.h"
#include <system_error>
#include <iostream>

using namespace std::literals::string_literals;

const int MQTT_TIMEOUT_MSECS = 10000;

extern "C" int onMessageArrived(void* context, char* topicName, int topicLen, MQTTClient_message* message)
{
	std::string topic;
	if (topicLen > 0)
		topic.assign(topicName, topicLen);
	else
		topic.assign(topicName);

	std::cout << "Message received on topic: " << topic << std::endl;
	const std::string payload(static_cast<char*>(message->payload), message->payloadlen);
	std::cout << "Payload: " << payload << std::endl;

	return true;
}

MQTTClient connect(const std::string& serverURL)
{
	MQTTClient client;
	int rc = MQTTClient_create(&client, serverURL.c_str(), "SampleClient", MQTTCLIENT_PERSISTENCE_NONE, 0);
	if (rc != MQTTCLIENT_SUCCESS)
		throw std::system_error(rc, std::generic_category(), "MQTTClient_create failed");

	MQTTClient_setCallbacks(client, nullptr, nullptr, onMessageArrived, nullptr);

	MQTTClient_connectOptions options = MQTTClient_connectOptions_initializer;
	rc = MQTTClient_connect(client, &options);
	if (rc != MQTTCLIENT_SUCCESS)
	{
		MQTTClient_destroy(&client);
		throw std::system_error(rc, std::generic_category(), "MQTTClient_connect failed");
	}

	return client;
}

void disconnect(MQTTClient& client)
{
	MQTTClient_disconnect(client, MQTT_TIMEOUT_MSECS);
	MQTTClient_destroy(&client);
}

int main(int argc, char** argv)
{
	const auto topic = "temp/random"s;

	try
	{
		MQTTClient client = connect("tcp://iot.eclipse.org"s);

		int rc = MQTTClient_subscribe(client, topic.c_str(), 0);
		if (rc == MQTTCLIENT_SUCCESS)
		{
			std::cout << "Enter 'q' + return to quit..." << std::endl;
			char c;
			do { std::cin >> c; } while (c != 'q');
		}
		else
		{
			std::cout << "Failed to subscribe to: " << topic << std::endl;
		}
		disconnect(client);
	}
	catch (std::system_error& exc)
	{
		std::cerr << exc.what() << " (" << exc.code() << ")" << std::endl;
	}
	return 0;
}
