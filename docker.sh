docker build -t steg:latest .
docker run --rm -it -v ./files:/source/files steg