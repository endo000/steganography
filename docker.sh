docker build -t steg:latest .
# https://stackoverflow.com/questions/41485217/mount-current-directory-as-a-volume-in-docker-on-windows-10
docker run --rm -it -v ${PWD}/files:/source/files steg:latest