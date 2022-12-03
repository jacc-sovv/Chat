###############################################
# Group Name  : XXXXXX

# Member1 Name: XXXXXX
# Member1 SIS ID: XXXXXX
# Member1 Login ID: XXXXXX

# Member2 Name: XXXXXX
# Member2 SIS ID: XXXXXX
# Member2 Login ID: XXXXXX
###############################################


import random
import sys
import socket
import pickle

url = None
chain_file = "chaingang.txt"
chain_list = []
if(len(sys.argv) == 3):
    url = sys.argv[1]
    chain_file = sys.argv[2]

if(len(sys.argv) == 2):
    url = sys.argv[1]


with open(chain_file) as file:
    file.readline()
    chain_list = [line.rstrip() for line in file]

#At this point, chain_list contains a list of strings, each one being a line in chain file

if(not(url and len(chain_list) > 0)):
    print("Error reading in URL or chain file! Exitting...")
    sys.exit(2)

starting_point = random.choice(chain_list)

ip = starting_point.split()[0]
port = starting_point.split()[1]

print(f"IP is {ip}, port is {port}")

#Stripy first connection from chainlist
chain_list.remove(ip + " " + port)
chain_list.insert(0, url)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((ip, int(port))) 
s.setblocking(False)
#Send URL and chainlist to ss
list_data = pickle.dumps(chain_list)
s.sendall(list_data)
#Wait until you receive the file

full_data = b''

#While there is data to be received, receive it

while True:
    chunk_data = s.recv(4096)
    print(f"Received {len(chunk_data)}bytes")
    full_data += chunk_data
    if(not chunk_data):
        break

f = open("cutecam.jpg", "wb")
f.write(full_data)
f.close()




# multiconn-client.py: creates multiple ECHO clients that communicate simultaneously with the ECHO server

# import sys
# import socket
# import selectors
# import types

# sel = selectors.DefaultSelector()
# #messages = [b"Message 1 from client.", b"Message 2 from client."]

# # this routine is called to create each of the many ECHO CLIENTs we want to create

# def start_connections(host, port, num_conns):
#     server_addr = (host, port)
#     for i in range(0, num_conns):
#         connid = i + 1
#         print("starting connection", connid, "to", server_addr)
#         sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#         sock.setblocking(False)
#         sock.connect_ex(server_addr)
#         events = selectors.EVENT_READ | selectors.EVENT_WRITE
#         data = types.SimpleNamespace(
#             connid=connid,
#             msg_total=len(list_data),
#             recv_total=0,
#             messages=list_data,
#             outb=b"",
#         )
#         sel.register(sock, events, data=data)

# # this routine is called when a client triggers a read or write event

# def service_connection(key, mask):
#     sock = key.fileobj
#     data = key.data
#     if mask & selectors.EVENT_READ:
#         recv_data = sock.recv(4096)  # Should be ready to read
#         if recv_data:
#             print("received", repr(recv_data), "from connection", data.connid)
#             data.recv_total += len(recv_data)
#         if not recv_data or data.recv_total == data.msg_total:
#             print("closing connection", data.connid)
#             sel.unregister(sock)
#             sock.close()
#     if mask & selectors.EVENT_WRITE:
#         if not data.outb and data.messages:
#             data.outb = data.messages.pop(0)
#         if data.outb:
#             print("sending", repr(data.outb), "to connection", data.connid)
#             sent = sock.send(data.outb)  # Should be ready to write
#             data.outb = data.outb[sent:]



# # main program
   
# host = ip   # localhost; use 0.0.0.0 if you want to communicate across machines in a real network
# port = port         # I just love fibonacci numbers
# num_conns = 1       # you can change this to however many clients you want to create


# start_connections(host, port, num_conns)

# # the event loop

# try:
#     while True:
#         events = sel.select(timeout=1)
#         if events:
#             for key, mask in events:
#                 service_connection(key, mask)
#         # Check for a socket being monitored to continue.
#         if not sel.get_map():
#             break
# except KeyboardInterrupt:
#     print("caught keyboard interrupt, exiting")
# finally:
#     sel.close()
