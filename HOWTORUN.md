# How to Run

## Using the Two Linux Virtual Machines

### Prerequisite
1. Make sure you have [VirtualBox](https://www.virtualbox.org/wiki/Downloads) Installed.
2. Make sure you have [Vagrant](https://www.vagrantup.com/downloads.html) Installed.
3. Clone the [*os-challenge-common*](https://github.com/dtu-ese/os-challenge-common) repository.

### Getting Started
1. Navigate to to the *os-challenge-common* repository.
2. Add the Ubuntu Server box with `vagrant box add ubuntu/trusty64` the first time.
3. "Turn on" the virtual machines with `vagrant up`. 
4. Log into the server with `vagrant ssh server` in one terminal. 
5. Clone the [*os-challenge-included-eagles*](https://github.com/Niels-wm/os-challenge-included-eagles) repository with `git clone https://github.com/Niels-wm/os-challenge-included-eagles` the first time.
6. Navigate to to the *os-challenge-included-eagles* repository.
7. Compile the *server.c* file with `gcc -o server server.c -lcrypto`
8. Run the server with `./server`
9. Log into the server with `vagrant ssh client` in another terminal. 
10. Run the client with `run-client.sh` (You can modify this script see Further help section).


## Using your own PC

### Getting Started
1. Make sure you have a C compiler.
2. Make sure you have [OpenSSL](https://www.openssl.org/source/) installed.
3. Clone the [*os-challenge-common*](https://github.com/dtu-ese/os-challenge-common) repository.
5. Clone this repository ([*os-challenge-included-eagles*](https://github.com/Niels-wm/os-challenge-included-eagles)).


### Run and compile
1. In the terminal, navigate to this directory.
2. Compile the server.c with `gcc -o server server.c -lcrypto`.
3. Run the server with `./server`.
4. Modify the *run-client.sh* script accordingly (You have to figure out this yourself).
5. Run the client with `run-client.sh` (You can modify this script see Further help section).

## Further help
See the documents in 02159 Operating Systems on [Learn DTU Inside](https://learn.inside.dtu.dk/).