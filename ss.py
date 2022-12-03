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

# sel = selectors.DefaultSelector()

# # this routine is called when the LISTENING SOCKET gets a 
# # connection request from a new client

# def accept_wrapper(sock):
#     conn, addr = sock.accept()  # Should be ready to read
#     print("accepted connection from", addr)
#     conn.setblocking(False)
#     data = types.SimpleNamespace(addr=addr, inb=b"", outb=b"")
#     print(f"Data is {data.inb}")
#     events = selectors.EVENT_READ | selectors.EVENT_WRITE
#     registered = True
#     sel.register(conn, events, data=data)



# # this routine is called when a client is ready to read or write data  

# def service_connection(key, mask):
#     sock = key.fileobj
#     data = key.data
#     if mask & selectors.EVENT_READ:     #If socket is ready for reading
#         recv_data = b''   # Should be ready to read
#         buf_size = 4096
#         while True:          #Ensures that ALL bits are read, important for a really big chain file
#             temp_data = sock.recv(buf_size) 
#             recv_data += temp_data
#             if(len(temp_data) < buf_size):
#                 break

#         if recv_data:
#             new_data = pickle.loads(recv_data)
#             print(new_data)
#             url = new_data[0]
#             chain_list = new_data[1:]


#             #At this point, newdata is a list containing the url and any remaining URLs [test.com, 1.1.1.1 1234, 2.2.2.2 2345, etc]
            
#             if(len(chain_list) == 0):
#                 #Issue wget request
#                 command = "wget " + url
#                 print("command is ", command)
#                 filename = os.popen('basename "' + url + '"').read()
#                 filename = filename.strip()

#                 if(filename == url):    #If there is no file name, then the name of the file should be index.html
#                     filename = "index.html"
#                 os.system(command)


#                 #Send the file back, reading it 4096 bits at a time
#                 f = open(filename, "rb")
#                 file_data = f.read()
#                 f.close()
#                 os.remove(filename)

#                 print(f"How big is the original file? {len(file_data)}")


#                 data.outb += file_data  #Append the file data to data.outb
#                 print("Data.outb contains all of file_data")

#             else:
#                 #Chainlist was not empty, forward to the next in the chain
#                 starting_point = random.choice(chain_list)

#                 ip = starting_point.split()[0]
#                 port = starting_point.split()[1]

#                 #Stripy myself from the chainlist
#                 chain_list.remove(ip + " " + port)
#                 chain_list.insert(0, url)

#                 s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#                 s.connect((ip, int(port))) 
#                 #Send URL and chainlist to ss
#                 chain_data = pickle.dumps(chain_list)
#                 print(f"Sending data to {ip}")
#                 s.send(chain_data)
#                 #Wait until you receive the file, then pass it back
#                 full_data = b''

#                 #While there is data to be received, receive it
#                 loop_count = 0
#                 while True:
#                     loop_count += 1
#                     chunk_data = s.recv(4096)
#                     full_data += chunk_data
#                     print(f"Chunk data is of size {len(chunk_data)}")
#                     if(not chunk_data):
#                         break
                
#                 print(f"All data received! Received {len(full_data)} bytes, looped through {loop_count} times")
#                 data.outb += full_data
#                 print(f"Length of data outb is {len(data.outb)}")
            
            

#         else:
#             print("closing connection to", data.addr)
#             sel.unregister(sock)
#             sock.close()
#     if mask & selectors.EVENT_WRITE:
#         
#         # print("Data.outb has fully sent the data")
#     sel.unregister(sock)


# # main program: set up the host address and port; change them if you need to

# hostname = socket.gethostname()
# host = socket.gethostbyname(hostname) #IP address of the computer I'm on
# port = 12350    #Need to implement the command line port to go here

# # set up the listening socket and register it with the SELECT mechanism

# lsock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# lsock.bind((host, port))
# lsock.listen()

# print("listening on", (hostname, port), "ip address of ", host)
# lsock.setblocking(False)
# sel.register(lsock, selectors.EVENT_READ, data=None)

# # the main event loop

# try:
#     while True:
#         events = sel.select(timeout=None)
#         for key, mask in events:
#             if key.data is None:
#                 accept_wrapper(key.fileobj)
#             else:
#                 service_connection(key, mask)
# except KeyboardInterrupt:
#     print("caught keyboard interrupt, exiting")
# finally:
#     sel.close()




hostname = socket.gethostname()
host = socket.gethostbyname(hostname) #IP address of the computer I'm on
port = 12351    #Need to implement the command line port to go here

# set up the listening socket and register it with the SELECT mechanism

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((host, port))
sock.listen()
conn, addr = sock.accept()

recv_data = b''   # Should be ready to read
buf_size = 4096
output_data = b''

while True:          #Ensures that ALL bits are read, important for a really big chain file
    temp_data = conn.recv(buf_size) 
    recv_data += temp_data
    if(len(temp_data) < buf_size):
        break

if recv_data:
    new_data = pickle.loads(recv_data)
    #At this point, newdata is a list containing the url and any remaining URLs [test.com, 1.1.1.1 1234, 2.2.2.2 2345, etc]

    print(new_data)
    url = new_data[0]
    chain_list = new_data[1:]
    #If I am the host to do wget
    if(len(chain_list) == 0):
        #Issue wget request
        command = "wget " + url
        print("command is ", command)
        filename = os.popen('basename "' + url + '"').read()
        filename = filename.strip()

        if(filename == url):    #If there is no file name, then the name of the file should be index.html
            filename = "index.html"
        os.system(command)


        #Send the file back, reading it 4096 bits at a time
        f = open(filename, "rb")
        file_data = f.read()
        f.close()
        os.remove(filename)

        print(f"How big is the original file? {len(file_data)}")


        output_data += file_data  #Append the file data to data.outb
    else:
        #Chainlist was not empty, forward to the next in the chain
        starting_point = random.choice(chain_list)

        ip = starting_point.split()[0]
        port = starting_point.split()[1]

        #Stripy myself from the chainlist
        chain_list.remove(ip + " " + port)
        chain_list.insert(0, url)

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((ip, int(port))) 
        #Send URL and chainlist to ss
        chain_data = pickle.dumps(chain_list)
        print(f"Sending data to {ip}")
        s.sendall(chain_data)
        #Wait until you receive the file, then pass it back
        full_data = b''

        #While there is data to be received, receive it
        loop_count = 0
        while True:
            loop_count += 1
            chunk_data = s.recv(4096)
            full_data += chunk_data
            print(f"Chunk data is of size {len(chunk_data)}")
            if(not chunk_data):
                break
        
        print(f"All data received! Received {len(full_data)} bytes, looped through {loop_count} times")
        output_data += full_data
        print(f"Length of data outb is {len(output_data)}")

    if output_data:
        print(f"Length of data.outb is now {len(output_data)}")
        # print("echoing", repr(data.outb), "to", data.addr)
        #print("Sending datat to ", data.addr)
        print(f"The type of data sending is {type(output_data)}")
        #Sends out the file in chunks
        # sent = sock.sendall(data.outb)  # Should be ready to write
        # print(f"Chunk data should have length of {sent}")

        # sent = conn.sendall(output_data)
        while True:
            sent = conn.send(output_data[:4096])
            output_data = output_data[sent:]
            print(f"Chunk data sent : {sent}")
            if(not output_data):
                break
            
        # output_data = output_data[sent:]


    conn.close()


            
