# Steganography OpenMPI
## PARALELNO IN PORAZDELJENO RAČUNANJE

### For this project I'm using OpenMPI, Boost.MPI and OpenCV

### How to build
> I'll explain how to build application only for Linux systems, for more convenient use on different hosts

#### Install OpenMPI
*TODO*
[Source](https://webpages.charlotte.edu/abw/coit-grid01.uncc.edu/ParallelProgSoftware/Software/OpenMPIInstall.pdf)

#### Install OpenCV
1. You can use [this source](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html) or [this](https://stackoverflow.com/questions/65738296/how-to-run-a-simple-opencv-code-in-c-on-linux), worked for me
```bash
# Install minimal prerequisites (Ubuntu 18.04 as reference)
sudo apt update && sudo apt install -y cmake g++ wget unzip
# Download and unpack sources
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.x.zip
unzip opencv.zip
# Create build directory
mkdir -p build && cd build
# Configure
cmake  ../opencv-4.x
# Build
cmake --build .
```
> If you use Anaconda for python you probably have `undefined reference to 'TIFFStripSize@LIBTIFF_4.0'` error, fix [*here*](https://github.com/colmap/colmap/issues/188)
#### Install Boost.MPI
> [Source](https://www.boost.org/doc/libs/1_81_0/doc/html/mpi/getting_started.html#mpi.getting_started.config.bootstrap) and [source](https://kratos-wiki.cimne.upc.edu/index.php/How_to_compile_the_Boost_if_you_want_to_use_MPI)
1. Download **.tar.bz2** archive from [here](https://www.boost.org/users/history/version_1_81_0.html)
2. Extract it and go to this folder
> Better extract it to the user folder e.g. /home/user/boost_1_81_0
3. Run `./bootstrap.sh`
4. Add `using mpi ;` to **project-config.jam**
5. Run `./b2` and `./b2 install`
6. Add library folder to PATH with `export PATH="/path/to/boost_1_81_0:$PATH"`

### How to run
* Compile program via `cmake . && make`
* Run `mpirun -np **proc_n** Steganography **flags**`

### Flags
* **-v** for _verbose_
* **-h** or **-u** for _hide_ or _unhide(reveal)_
* **-i** for _input image directory_
* **-b** for _book file_
__If hide, then read this file to hide in images, otherwise write revealed text to this file__

Only for hiding
* **-o** for _output directory_
* **-r** **-g** **-b** for only one channel hiding
* **--bits** to hide _n_ bits in every pixel
* **--random** to hide in random pixels

### Current bugs
* Don't check if the text will fit into the image in OpenMPI assign text to images phase

### How to run program on several machines
> [Original guide](https://mpitutorial.com/tutorials/running-an-mpi-cluster-within-a-lan/).
> We won't setup NFS, to make things easier

1. Create `mpiuser` user
2. One manager generates ssh key and sends public key to all machines
    > Generate **RSA** key, because **DSA** keys don't work by default
3. Ensure, that you installed OpenMPI, OpenCV and Boost
4. Clone this repo again as `mpiuser`
5. Go to repo and compile the program via `cmake . && make`
6. Manager creates hostfile with all workers
7. Run a program with `mpirun -np 5 -hostfile steg_hostfile steganography/Steganography **flags**` in home directory on manager
> If facing `Authorization required, but no authorization protocol specified` warning, set bash variable `export HWLOC_COMPONENTS=-gl`