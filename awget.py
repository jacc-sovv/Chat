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

#Stripy myself from the chainlist
chain_list = chain_list[1:]
chain_list.insert(0, url)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((ip, int(port))) 
#Send URL and chainlist to ss
data = pickle.dumps(chain_list)
s.send(data)
#Wait until you receive the file

