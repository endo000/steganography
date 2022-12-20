# Steganograpy
## Računalniška Večpredstavnost 3. Vaja

### How to build
- Using docker:
  1. Different commands for docker are [here](docker.sh)
  2. Build an image using Dockerfile
  3. Run this image, use volume to see the results of program execution
  4. Exec in docker container to work inside of it

- Locally:
  1. I am using CLion with MinGW compiler on Windows, so if you use Visual Studio or Linux process will be a little different
  2. I used [this repo](https://github.com/huihut/OpenCV-MinGW-Build) to download the build
  3. Extract it somewhere (e.g. C:\opencv_mingw\)
  4. Then add ...\x64\mingw\bin and ...\x64\mingw\lib to Path (absolute path e.g. C:\opencv_mingw\x64\mingw\bin and C:\opencv_mingw\x64\mingw\bin)

### Run the code
- Run manually using `./do_steganograpy` with different flags
- Use [examples.sh](examples.sh) as `./examples.sh 1`

### libdc1394 error
If you see **libdc1394 error: Failed to initialize libdc1394**, run `ln /dev/null /dev/raw1394` inside of container

### JPG format incorrect saving
It's impossible to reveal message from **.jpg** image, because `cv::imwrite()` changes pixel values to +-2. That's why LSB method doesn't work.
You can save hidden image to another format (**.png**, **.bmp**, etc.)