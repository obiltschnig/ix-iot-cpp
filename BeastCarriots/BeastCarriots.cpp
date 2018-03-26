#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

using namespace std::literals::string_literals;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;
namespace beast = boost::beast;
namespace http = beast::http;
namespace pt = boost::property_tree;

void sendTemperature(const std::string& apiKey, const std::string device, float temperature)
{
	const auto host = "api.carriots.com"s;
	const auto port = 80;
	const auto path = "/streams"s;

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

	boost::asio::io_context ioc;
	tcp::resolver resolver{ioc};
	tcp::socket socket{ioc};

	const auto resolved = resolver.resolve(host, std::to_string(port));
	boost::asio::connect(socket, resolved.begin(), resolved.end());

	http::request<http::string_body> req{http::verb::post, path, 11};
	req.set(http::field::host, host);
	req.set("carriots.apikey"s, apiKey);
	req.set(http::field::content_type, "application/json"s);
	req.body() = json;
	req.prepare_payload();
	http::write(socket, req);

	beast::flat_buffer buffer;
	http::response<http::string_body> res;
	http::read(socket, buffer, res);

	boost::system::error_code ec;
	socket.shutdown(tcp::socket::shutdown_both, ec);

	if (ec && ec != boost::system::errc::not_connected)
		throw boost::system::system_error{ec};

	if (res.result_int() != 200)
	{
		pt::ptree pt;
		std::istringstream bstr(res.body());
		pt::read_json(bstr, pt);
		const auto& respPt = pt.get_child("response");
  		const auto resp = respPt.get_value<std::string>();
		throw std::runtime_error(resp);
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
	catch (std::exception& exc)
	{
		std::cerr << exc.what() << std::endl;
	}

    return 0;
}
