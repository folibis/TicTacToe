# TicTacToe
TicTacToe game implementation

## How to build and run the game ##


#### 1. Requirements. ####
* Linux (Ubuntu >= 18.04 is preferable)
* git
* cmake
* gcc
* build essential tools

#### 1. Downoading. ####
**Note**: _The game is require both Linux and Windows machines_
##### Linux #####
Enter a folder in your system when you want to build the game and clone the GitHub repository, for example:
```bash
cd ~/sources
git clone git@github.com:folibis/TicTacToe.git 
```
##### Windows #####
* clone the game repository into a source folder, for example:
```bash
cd C:/sources
git clone git@github.com:folibis/TicTacToe.git 
```


#### 2. Building. ####
**Note**: _The game is require both Linux and Windows machines_

##### Linux #####
enter the downloaded folder and buil the applications:
```bash
cd TicTacToe/
# update/download the dependencies
./git-update
# run the script to buld the game
./build.sh
```
##### Windows #####
* install python from [this link](https://www.python.org/downloads/). v. >= 3.8 is required.
* install pyzmq (Python bindings for ØMQ) by entering the following command from a command line:
```bash
pip install pyzmq
```

#### 3. Running the game. ####
**Note**: _The game is require both Linux and Windows machines_
##### Windows #####
* run the judge module:
```bash
python C:/sources/TicTacToe/judge/main.py
```
Use the following options for configuration:
```
main.py [-a address] [-p port]
where:
  -a address - the address the server binds to
  -p port - the port the server binds to
```

##### Linux #####
* run the server:
```bash
cd ~/sources/TicTacToe/bin/
# run the server
./server
```
Use the following options for configuration:
```
main.py [-a address] [-p port]
where:
  -a address - the address the server binds to
  -p port - the port the server binds to
  -az address - the address of the judge server
  -pz port - the port of the judge server
  -h - prints this help
```

 run the client
```bash
cd /path/to/source/bin/
#run the server
./client
```
Use the following options for configuration:
```
main.py [-a address] [-p port]
where:
  -a address - the address of the game server
  -p port - the port of the game server 
  -h - prints this help
```

#### 3. Playing the game. ####
Follow the game instruction to play the game


