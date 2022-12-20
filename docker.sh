docker build -t steg:latest .
docker run --rm -it -v $(pwd)/files:/source/files steg