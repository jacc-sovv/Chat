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
import getopt
import socket
import pickle
import os

#URL of a pic of cam lookin cute :) https://alumni.colostate.edu/wp-content/uploads/sites/26/2022/01/cam_banner2.jpg

url = None
chain_file = "chaingang.txt"
chain_list = []

if(len(sys.argv) == 2):
    url = sys.argv[1]
else:
    url = sys.argv[1]
    argumentList = sys.argv[2:]
    options = "c:"

    try:
        arguments, values = getopt.getopt(argumentList, options)
        for currentArgument, currentValue in arguments:
            if currentArgument in ("-c"):
                chain_file = currentValue
    except getopt.error as err:
        print (str(err))

with open(chain_file) as file:
    file.readline()
    chain_list = [line.rstrip() for line in file]

#At this point, chain_list contains a list of strings, each one being a line in chain file
print(f"{sys.argv[0]}:")
print(f"Request: {url}")
print("Chainlist is:")
for x in chain_list:
    print(f"{x}")

if(not(url and len(chain_list) > 0)):
    print("Error reading in URL or chain file! Exitting...")
    sys.exit(2)

starting_point = random.choice(chain_list)

ip = starting_point.split()[0]
port = starting_point.split()[1]

print(f"Next IP is {ip}, port is {port}")

#Stripy first connection from chainlist
chain_list.remove(ip + " " + port)
chain_list.insert(0, url)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((ip, int(port))) 
#Send URL and chainlist to ss
data = pickle.dumps(chain_list)
s.sendall(data)
#Wait until you receive the file

full_data = b''

print("Waiting for file...")
#While there is data to be received, receive it
while True:
    chunk_data = s.recv(4096)
    full_data += chunk_data
    if(not chunk_data):
        break

filename = os.popen('basename "' + url + '"').read()
filename = filename.strip()

if(filename == url):    #If there is no file name, then the name of the file should be index.html
    filename = "index.html"

print(f"Recieved file {filename} of length {len(full_data)}")

f = open(filename, "wb")
f.write(full_data)
f.close()

print("Goodbye")
s.close()
