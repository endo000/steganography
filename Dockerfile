FROM schickling/opencv:latest
COPY . /source
RUN g++ $(pkg-config --cflags --libs opencv) -std=c++14 main.cpp steganography/steganography.cpp utils/utils.cpp utils/config_utils.cpp steganalysis/steganalysis.cpp -o do_steganography
RUN ln /dev/null /dev/raw1394
CMD /bin/bash