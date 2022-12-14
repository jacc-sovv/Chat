###############################################
# Group Name  : xXXXXx

# Member1 Name: John McFall
# Member1 SIS ID: 832435619
# Member1 Login ID: jmcf521

# Member2 Name: Jack Sovereign
# Member2 SIS ID: 832430250
# Member2 Login ID: jacc
###############################################

#Need to accept a port from the command line
#Are we allowed to use other libraries like pickle?
import sys
import getopt
import socket
import selectors
import types
import pickle
import random
import os
import threading

#Gets the file and returns the file data
def empty_list(url):
    
    #Issue wget request
    command = "wget " + url
    print(f"Request: {url}")
    filename = os.popen('basename "' + url + '"').read()
    filename = filename.strip()

    if(filename == url):    #If there is no file name, then the name of the file should be index.html
        filename = "index.html"
    print("chainlist is empty")
    print(f"issuing wget for file {filename}")
    os.system(command)



    f = open(filename, "rb")
    file_data = f.read()
    f.close()
    os.remove(filename)
    print("File received")
    return file_data

#Returns data to be sent
def full_list(chain_list, url):
    print(f"Request: {url}")
    print("chainlist is")
    for x in chain_list:
        print(x)
    starting_point = random.choice(chain_list)

    ip = starting_point.split()[0]
    port = starting_point.split()[1]

    print(f"Next SS is {ip}, port is {port}")

    #Stripy myself from the chainlist
    chain_list.remove(ip + " " + port)
    chain_list.insert(0, url)

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((ip, int(port))) 
    #Send URL and chainlist to ss
    chain_data = pickle.dumps(chain_list)

    s.sendall(chain_data)
    #Wait until you receive the file, then pass it back
    full_data = b''
    print("waiting for file...")

    #While there is data to be received, receive it
    loop_count = 0
    while True:
        loop_count += 1
        chunk_data = s.recv(4096)
        full_data += chunk_data

        if(not chunk_data):
            break
    
    return full_data

def deal_with_connection(conn):
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
        #At this point, newdata is a list containing the url and any remaining ips [test.com, 1.1.1.1 1234, 2.2.2.2 2345, etc]

        url = new_data[0]
        chain_list = new_data[1:]
        #If I am the host to do wget
        if(len(chain_list) == 0):
            file_data = empty_list(url)
            output_data += file_data  #Append the file data to data.outb
        else:
            #Chainlist was not empty, forward to the next in the chain
            full_data = full_list(chain_list, url)
            output_data += full_data

        if output_data:
            print("Relaying file ...")
            while True:
                sent = conn.send(output_data[:4096])
                output_data = output_data[sent:]

                if(not output_data):
                    break
        print("Goodbye!")
        print("")
        conn.close()

argumentList = sys.argv[1:]
options = "p:"

port = 12359

try:
    arguments, values = getopt.getopt(argumentList, options)
    for currentArgument, currentValue in arguments:
        if currentArgument in ("-p"):
            port = int(currentValue)
except getopt.error as err:
    print (str(err))

hostname = socket.gethostname()
host = socket.gethostbyname(hostname) #IP address of the computer I'm on
# set up the listening socket and register it with the SELECT mechanism

parent_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
parent_sock.bind((host, port))
parent_sock.listen()
print("ss: listening on", (hostname, port), "ip address of ", host)
while True:
    conn, addr = parent_sock.accept()
    threading.Thread(target=deal_with_connection, args=[conn]).start()
