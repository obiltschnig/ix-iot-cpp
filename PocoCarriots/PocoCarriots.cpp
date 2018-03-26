#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/JSON/Parser.h>
#include <Poco/URI.h>
#include <sstream>
#include <iostream>

using namespace std::literals::string_literals;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
namespace JSON = Poco::JSON;

void sendTemperature(const std::string& apiKey, const std::string device, float temperature)
{
	const Poco::URI uri{"https://api.carriots.com/streams"s};

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
	std::string json{jsonStream.str()};

	Poco::Net::HTTPSClientSession session{uri.getHost(), uri.getPort()};
	HTTPRequest request{HTTPRequest::HTTP_POST, uri.getPathEtc(), HTTPMessage::HTTP_1_1};
	request.setContentType("application/json"s);
	request.setContentLength(json.length());
	request.set("carriots.apikey"s, apiKey);

	session.sendRequest(request) << json;

	HTTPResponse response;
	std::istream& istr = session.receiveResponse(response);

	JSON::Parser parser;
	const auto var = parser.parse(istr);
	const auto pObject = var.extract<JSON::Object::Ptr>();
	const auto message = pObject->getValue<std::string>("response"s);
	if (response.getStatus() != HTTPResponse::HTTP_OK)
		throw Poco::IOException("Request failed"s, message);
}

int main(int argc, char** argv)
{
    const auto apiKey{"ce3a2a76dbd871f0ef773e21bf79a49cae2f4d8a27cf463c9a0508cd92a6e5b5"s};
    const auto device{"RaspberryPi2@obiltschnig.obiltschnig"s};

	try
	{
    	sendTemperature(apiKey, device, 23);
	}
	catch (Poco::Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
	}

    return 0;
}
