#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>

#include "generated/inference.grpc.pb.h"
#include "generated/inference.pb.h"

using namespace server::grpc;

int main(int argc, char** argv) {
    printf("Hello World!\n");
    return 0;
}