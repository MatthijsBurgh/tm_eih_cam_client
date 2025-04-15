#include <grpcpp/grpcpp.h>

#include <iostream>
#include <vector>

#include "EIHCameraApiClient.hpp"
using namespace TmEIHCamera;

int main() {
  std::string server_address = "172.25.181.19:15567";

  EIHCameraApiClient client(server_address);

  //   client.isCameraConnected();

  while (true) {
    client.getImageData();
  }

  return 0;
}