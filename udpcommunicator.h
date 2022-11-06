#pragma once

//#include <boost/asio/io_service.hpp>
//#include <boost/asio/ip/udp.hpp>

#include <string>
#include <memory>

namespace std
{
    class thread;
}

class UdpCommunicator
{
public:
    UdpCommunicator() noexcept;

    ~UdpCommunicator();

    void start(int binding_port = -1);

    void stop();

    void send_message(const std::string& message, const std::string& ip, const unsigned port);

private:
    void handle_send(std::shared_ptr<std::string> /*my_message*/,
                     const boost::system::error_code& error,
                     std::size_t bytes_transferred);

    void handle_receive(const boost::system::error_code& error,
                        std::size_t bytes_transferred);

    void start_receive();

private:
    boost::asio::io_service m_ios;
    boost::asio::ip::udp::socket m_sock;
    std::unique_ptr<std::thread> thread;
    std::unique_ptr<boost::asio::io_service::work> m_work;
    boost::asio::ip::udp::endpoint remote_endpoint;
    std::array<char, 65508> recv_buffer;
};
