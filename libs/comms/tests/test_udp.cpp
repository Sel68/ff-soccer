#include <gtest/gtest.h>
#include <asio.hpp>
#include <thread>
#include <chrono>
#include "AsyncUdpServer.hpp"
#include "AsyncUdpClient.hpp"

class UdpEchoTest : public ::testing::Test {
 protected:
  void SetUp() override {
    server_ioc_ = std::make_unique<asio::io_context>();
    server_ = std::make_shared<comms::AsyncUdpServer>(*server_ioc_, asio::ip::udp::endpoint(asio::ip::udp::v4(), 9001));
    server_->start();
    server_thread_ = std::thread([this]() { server_ioc_->run(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  void TearDown() override {
    if (server_ioc_) {
      server_ioc_->stop();
      server_thread_.join();
    }
  }

  std::unique_ptr<asio::io_context> server_ioc_;
  std::shared_ptr<comms::AsyncUdpServer> server_;
  std::thread server_thread_;
};

TEST_F(UdpEchoTest, RoundTripEcho) {
  comms::AsyncUdpClient client;
  std::vector<uint8_t> req({'T','e','s','t'});
  std::vector<uint8_t> resp;
  asio::ip::udp::endpoint server_ep(asio::ip::make_address("127.0.0.1"), 9001);

  bool ok = client.request_response(server_ep, req, resp, 1000);
  ASSERT_TRUE(ok) << "Failed to receive reply from server";
  EXPECT_EQ(resp, req) << "Reply does not match request";
}

TEST_F(UdpEchoTest, MultipleMessages) {
  comms::AsyncUdpClient client;
  asio::ip::udp::endpoint server_ep(asio::ip::make_address("127.0.0.1"), 9001);

  std::vector<std::vector<uint8_t>> requests = {
    {'H','e','l','l','o'},
    {'W','o','r','l','d'},
    {'T','e','s','t','1','2','3'}
  };

  for (const auto& req : requests) {
    std::vector<uint8_t> resp;
    bool ok = client.request_response(server_ep, req, resp, 1000);
    EXPECT_TRUE(ok) << "Failed on request: " << std::string(req.begin(), req.end());
    EXPECT_EQ(resp, req) << "Reply mismatch";
    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // allow socket cleanup
  }
}

TEST_F(UdpEchoTest, Timeout) {
  comms::AsyncUdpClient client;
  std::vector<uint8_t> req({'T','e','s','t'});
  std::vector<uint8_t> resp;
  // Use a different port (no server listening)
  asio::ip::udp::endpoint bad_ep(asio::ip::make_address("127.0.0.1"), 9999);

  bool ok = client.request_response(bad_ep, req, resp, 100);
  EXPECT_FALSE(ok) << "Should timeout on unreachable endpoint";
}
