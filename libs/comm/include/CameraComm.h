#ifndef CAMERA_COMM_H
#define CAMERA_COMM_H

#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>

struct CameraValues {
  int id;
  float x;
  float y;
  float orientation;
};

class CameraComm {
 private:
  int sock_fd;
  const int LISTEN_PORT = 5000;
  const int BUFFER_SIZE = 65536;

  std::vector<CameraValues> latest_values;
  std::mutex values_mutex;
  std::thread recv_thread;
  std::atomic<bool> running;

  void ReceiveLoop();

  // Private constructor/destructor for Singleton
  CameraComm();
  ~CameraComm();

  // Prevent copying
  CameraComm(const CameraComm&) = delete;
  CameraComm& operator=(const CameraComm&) = delete;

  // Singleton instance accessor
  static CameraComm& GetInstance();

 public:
  static std::vector<CameraValues> GetCameraValues();
};

#endif  // CAMERA_COMM_H