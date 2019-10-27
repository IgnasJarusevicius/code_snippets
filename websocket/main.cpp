#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

// Handles Websocket connection
void websocket_session(tcp::socket& socket, http::request<http::dynamic_body> request)
{
    try
    {
        // Construct websocket stream by moving in the socket
        websocket::stream<tcp::socket> ws(std::move(socket));
        // Set suggested timeout settings for the websocket
        ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));
        //Accept the websocket handshake
        //by responding to a WebSocket HTTP Upgrade request
        //will produce an error if request is not WebSocket HTTP Upgrade
        ws.accept(request);

        for(;;)
        {
            std::string str;
            //encapsulate string into buffer so it can be used with boost websocket read/write
            auto buffer = net::dynamic_buffer(str);
            // Read a message
            ws.read(buffer);

            if (!ws.got_text()) //close connection on non-text message
            {
                std::cout << "Not a text message, disconnect" << std::endl;
                //manually close the socket, not strictly needed as destructor would take care of it
                ws.close(websocket::close_code::none);
                return;
            }
            //replace '?' with '!';
            std::replace(str.begin(), str.end(), '?', '!');

            //respond with modified text message
            ws.text(true);
            ws.write(buffer.data());
         }
    }
    catch(beast::system_error const& se) //handle boost exceptions
    {
        //do not consider closed connection as an error
        if(se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

// Handles HTTP connection
void http_session(tcp::socket&& socket)
{
    try
    {
        beast::flat_buffer buffer;
        http::request<http::dynamic_body> request;
        http::response<http::dynamic_body> response;
        http::read(socket, buffer, request);;

        if (request.method() == http::verb::get) //handle GET
        {
            //handle switch to websocket on "/ws" path
            if(request.target() == "/ws")
            {
                return websocket_session(socket, request);
            }
            else //respond with file not found otherwise
            {
                response.result(http::status::not_found);
                response.set(http::field::content_type, "text/plain");
                beast::ostream(response.body()) << "File not found\r\n";
            }
        }
        else //repond with 'bad request' if request is not GET
        {
            response.result(http::status::bad_request);
            response.set(http::field::content_type, "text/plain");
            beast::ostream(response.body()) << "Bad request: " << std::string(request.method_string());
        }

        response.version(request.version());    //HTTP version
        response.keep_alive(false);             //do not keep connection indication
        response.set(http::field::server, "HTTP Server"); //server name
        response.set(http::field::content_length, response.body().size());

        //send response
        http::write(socket, response);

        //terminate connection by calling shutdown() + close()
        socket.shutdown(tcp::socket::shutdown_both);
        socket.close(); //manually close the socket, not strictly needed as destructor would take care of it
    }
    catch(beast::system_error const& se) //handle boost exceptions
    {
        //do not consider closed connection as an error
        if(se.code() != http::error::end_of_stream)
            std::cerr << "Error: " << se.code().message() << std::endl;
    }
}


int main(int argc, char* argv[])
{
    try
    {
        const auto address = net::ip::make_address("127.0.0.1");
        const unsigned short port = (argc > 1) ? std::atoi(argv[1]) : 8080; //read port from command line, default to 8080
        std::cout << "Info: HTTP server port " << port << std::endl;

        net::io_context ioc(1);      // The io_context is required for all I/O
        tcp::acceptor acceptor(ioc, {address, port});
        for(;;)
        {
            tcp::socket socket(ioc);
            // Blocks until we get a connection
            acceptor.accept(socket);
            //create and detach thread
            //socket is non-copyable, but movable so move it
            std::thread(http_session, std::move(socket)).detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}