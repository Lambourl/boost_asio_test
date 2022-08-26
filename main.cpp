#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/array.hpp>
#include <iostream>

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;


int main(int argc, char* argv[]){
  try{
    if (argc != 2){
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "https");
    ssl::context ctx(ssl::context::tlsv12_client);
    ctx.set_verify_mode(ssl::verify_peer);
    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);
    for (;;){
      boost::array<char, 128> buf;
      boost::system::error_code error;
      size_t len = socket.read_some(boost::asio::buffer(buf), error);
      if (error == boost::asio::error::eof)
        break; // Connection closed cleanly by peer.
      else if (error)
        throw boost::system::system_error(error); // Some other error.
      std::cout.write(buf.data(), len);
    }
  }catch (std::exception& e){
    std::cerr << e.what() << std::endl;
  }
     
   return 0;
}