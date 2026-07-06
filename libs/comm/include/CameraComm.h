#pragma once
#include <vector>
#include <string>

struct CameraValues{
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

public:
  CameraComm();
  ~CameraComm();

  std::vector<CameraValues> GetCameraValues();
};