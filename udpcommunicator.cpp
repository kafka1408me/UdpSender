#include "udpcommunicator.h"
#include "myapp.h"

using namespace boost;


UdpCommunicator::UdpCommunicator() noexcept:
    m_sock(m_ios)
{
}

UdpCommunicator::~UdpCommunicator()
{
    stop();
}

void UdpCommunicator::start(int binding_port)
{
    m_work = std::make_unique<boost::asio::io_service::work>(m_ios);

    thread = std::make_unique<std::thread>([this](){
        m_ios.run();
    });
    m_sock.open(asio::ip::udp::v4());

    if(binding_port > 0)
    {
        m_sock.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), binding_port));

        start_receive();
    }
}

void UdpCommunicator::stop()
{
    if(thread.get())
    {
        m_sock.cancel();
        m_ios.stop();
        thread->join();
    }
}

void UdpCommunicator::send_message(const std::string& message, const std::string& ip, const unsigned port)
{
    asio::ip::udp::endpoint remote_endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(ip), port);

    std::shared_ptr<std::string> my_message{std::make_shared<std::string>(message)};

    m_sock.async_send_to(boost::asio::buffer(*my_message), remote_endpoint,
                         boost::bind(&UdpCommunicator::handle_send, this, my_message,
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));
}

void UdpCommunicator::handle_send(std::shared_ptr<std::string> /*my_message*/,
                                  const boost::system::error_code& error,
                                  std::size_t bytes_transferred)
{
    if(error.value() != 0)
    {
        ADD_TEXT("sending error occured. error code = " + std::to_string(error.value()), MessageType::Error);
    }
    else if(error.value() == asio::error::operation_aborted)
    {
        ADD_TEXT("sending operation cancelled", MessageType::Warning);
    }
    else
    {
        ADD_TEXT(std::to_string(bytes_transferred) + " bytes sended", MessageType::MessageSended);
    }
}

void UdpCommunicator::handle_receive(const boost::system::error_code& error,
                                     std::size_t bytes_transferred)
{
    if(error.value() == 0)
    {
        recv_buffer[bytes_transferred] = '\0';
        ADD_TEXT(std::to_string(bytes_transferred) + " bytes received : \"" +
                 std::string(recv_buffer.data()) + "\" from " + remote_endpoint.address().to_string()
                 + ":" + std::to_string(remote_endpoint.port()),
                 MessageType::MessageReceived);
    }

    start_receive();
}


void UdpCommunicator::start_receive()
{
    m_sock.async_receive_from(
                boost::asio::buffer(recv_buffer), remote_endpoint,
                boost::bind(&UdpCommunicator::handle_receive, this,
                            asio::placeholders::error,
                            asio::placeholders::bytes_transferred));
}
