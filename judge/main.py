import zmq
import sys
import utils

address = "*"
port = 1235
action = ""

for i, arg in enumerate(sys.argv):
    if i > 0:
        match arg:
            case "-a":
                action = "address"
            case "-p":
                action = "port"
            case _:
                match action:
                    case "address":
                        address = arg
                    case "port":
                        port = int(arg)

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://" + address + ":" + str(port))

while True:
    message = socket.recv()
    response = utils.process_message(message)
    socket.send(response.encode())

