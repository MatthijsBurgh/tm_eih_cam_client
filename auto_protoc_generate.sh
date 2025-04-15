protoc -I ./proto --grpc_out=./proto_gen/ --plugin=protoc-gen-grpc=/home/steven/.local/bin/grpc_cpp_plugin ./proto/*
protoc -I ./proto --cpp_out=./proto_gen/ ./proto/*
