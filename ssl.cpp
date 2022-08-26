
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/array.hpp>


using boost::asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;


int main(int argc, char* argv[])
{
  using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;

try{
    if (argc != 2){
      std::cerr << "Usage: client <host>" << std::endl;
      return 1;
    }

// Create a context that uses the default paths for
// finding CA certificates.
ssl::context ctx(ssl::context::sslv23);
ctx.set_default_verify_paths();
ctx.add_verify_path("/etc/ssl");

// Open a socket and connect it to the remote host.
boost::asio::io_context io_context;
ssl_socket sock(io_context, ctx);
tcp::resolver resolver(io_context);
tcp::resolver::query query(argv[1], "https");
boost::asio::connect(sock.lowest_layer(), resolver.resolve(query));
sock.lowest_layer().set_option(tcp::no_delay(true));

// Perform SSL handshake and verify the remote host's
// certificate.
sock.set_verify_mode(ssl::verify_peer);
sock.set_verify_callback(ssl::host_name_verification(argv[1]));
sock.handshake(ssl_socket::client);

for (;;){
      boost::array<char, 128> buf;
      boost::system::error_code error;
      size_t len = sock.read_some(boost::asio::buffer(buf), error);
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