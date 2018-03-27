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

std::string toJSON(const JSON::Object& obj)
{
	std::ostringstream jsonStream;
	obj.stringify(jsonStream);
	return jsonStream.str();
}

void sendTemperature(const std::string& apiKey, const std::string device, float temperature)
{
	const Poco::URI uri{"https://api.carriots.com/streams"s};

	JSON::Object obj;
	obj.set("protocol"s, "v2"s);
	obj.set("device"s, device);
	obj.set("at"s, "now"s);
	JSON::Object data;
	data.set("temperature"s, temperature);
	obj.set("data"s, data);

	const auto json{toJSON(obj)};

	Poco::Net::HTTPSClientSession session{uri.getHost(), uri.getPort()};
	HTTPRequest request{HTTPRequest::HTTP_POST, uri.getPathEtc(), HTTPMessage::HTTP_1_1};
	request.setContentType("application/json"s);
	request.setContentLength(json.length());
	request.set("carriots.apikey"s, apiKey);

	session.sendRequest(request) << json;

	HTTPResponse response;
	std::istream& istr = session.receiveResponse(response);

	if (response.getStatus() != HTTPResponse::HTTP_OK)
	{
		const auto pObject = JSON::Parser().parse(istr).extract<JSON::Object::Ptr>();
		const auto message = pObject->getValue<std::string>("response"s);
		throw Poco::IOException("Request failed"s, message);
	}
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
