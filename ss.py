###############################################
# Group Name  : XXXXXX

# Member1 Name: XXXXXX
# Member1 SIS ID: XXXXXX
# Member1 Login ID: XXXXXX

# Member2 Name: XXXXXX
# Member2 SIS ID: XXXXXX
# Member2 Login ID: XXXXXX
###############################################

#Need to accept a port from the command line
#Are we allowed to use other libraries like pickle?
import sys
import socket
import selectors
import types
import pickle
import random
import os

sel = selectors.DefaultSelector()

# this routine is called when the LISTENING SOCKET gets a 
# connection request from a new client

def accept_wrapper(sock):
    conn, addr = sock.accept()  # Should be ready to read
    print("accepted connection from", addr)
    conn.setblocking(False)
    data = types.SimpleNamespace(addr=addr, inb=b"", outb=b"")
    print(f"Data is {data.inb}")
    events = selectors.EVENT_READ | selectors.EVENT_WRITE
    sel.register(conn, events, data=data)


# this routine is called when a client is ready to read or write data  

def service_connection(key, mask):
    sock = key.fileobj
    data = key.data
    if mask & selectors.EVENT_READ:     #If socket is ready for reading
        recv_data = b''   # Should be ready to read
        buf_size = 4096
        while True:          #Ensures that ALL bits are read, important for a really big chain file
            temp_data = sock.recv(buf_size) 
            recv_data += temp_data
            if(len(temp_data) < buf_size):
                break

        if recv_data:
            new_data = pickle.loads(recv_data)
            print(new_data)
            url = new_data[0]
            chain_list = new_data[1:]


            #At this point, newdata is a list containing the url and any remaining URLs [test.com, 1.1.1.1 1234, 2.2.2.2 2345, etc]
            
            if(len(chain_list) == 0):
                #Issue wget request
                command = "wget " + url
                print("command is ", command)
                os.system(command)
                print('yo')
            else:
                #Chainlist was not empty, forward to the next in the chain
                starting_point = random.choice(chain_list)

                ip = starting_point.split()[0]
                port = starting_point.split()[1]

                #Stripy myself from the chainlist
                chain_list = chain_list[1:]
                chain_list.insert(0, url)

                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect((ip, int(port))) 
                #Send URL and chainlist to ss
                data = pickle.dumps(chain_list)
                s.send(data)
                #Wait until you receive the file, then pass it back
            
            
            data.outb += recv_data
        else:
            print("closing connection to", data.addr)
            sel.unregister(sock)
            sock.close()
    if mask & selectors.EVENT_WRITE:
        if data.outb:
            print("echoing", repr(data.outb), "to", data.addr)
            sent = sock.send(data.outb)  # Should be ready to write
            data.outb = data.outb[sent:]


# main program: set up the host address and port; change them if you need to

hostname = socket.gethostname()
host = socket.gethostbyname(hostname) #IP address of the computer I'm on
port = 12358    #Need to implement the command line port to go here

# set up the listening socket and register it with the SELECT mechanism

lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
lsock.bind((host, port))
lsock.listen()

print("listening on", (hostname, port), "ip address of ", host)
lsock.setblocking(False)
sel.register(lsock, selectors.EVENT_READ, data=None)

# the main event loop
try:
    while True:
        events = sel.select(timeout=None)
        for key, mask in events:
            if key.data is None:
                accept_wrapper(key.fileobj)
            else:
                service_connection(key, mask)
except KeyboardInterrupt:
    print("caught keyboard interrupt, exiting")
finally:
    sel.close()