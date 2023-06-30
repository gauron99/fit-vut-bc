import socket, sys, re, os

HOST = '127.0.0.1'

port = sys.argv[1]
port = int(port)

if port > 1023 and port < 6535: #non-privileged ports are > than 1023
    PORT = port;
else:
    print ("error, port value should be between 1023 and 6535") #500 Internal Server Error ?
    sys.exit(1)

first_line = False
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) #override waiting after closing the server
    s.bind((HOST,PORT))
    s.listen(1)
    try:      # try/except; if CTRL+C command is pressed, server is closed casually with exit 0
        while True:
            conn, addr = s.accept() #conn = connection
            with conn:
                data = conn.recv(1024).decode('utf-8')
                
                if data is None:
                    conn.sendall('500 Internal Server Error\n'.encode('utf-8'))#if no data are req. #500 Internal Server Error ?
                #GET and POST commands split
                if  re.search(r"^GET ",data): 
                    if re.search(r"^GET /resolve\?name=(.+)&type=(A|PTR)",data):
                        name = (re.search(r"name=(.+)&type=(A|PTR)",data)) # save the name part; devided into 2 groups 
                        #group one = name; group 2 = type (A or PTR)
                        if name.group(2) == 'A' and name.group(1) != None: #re.search(r'^www\.',name.group(1)) and
                            try:
                                conn.sendall(("HTTP/1.1 200 OK\r\n\r\n"+name.group(1)+":A="+
                                socket.gethostbyname(name.group(1))+"\r\n").encode('utf-8'))
                            except socket.error: #when en error occurs trying to answer
                                conn.send(("HTTP/1.1 404 Not Found").encode('utf-8'))
                        elif name.group(2) == 'PTR':
                            try:
                                conn.sendall(("HTTP/1.1 200 OK\r\n\r\n"+name.group(1)+":PTR="+ 
                                socket.gethostbyaddr(name.group(1))[0]+"\r\n").encode('utf-8'))# gethostbyaddr returns field of 3 items
                            except socket.error:
                                conn.sendall(("HTTP/1.1 404 Not Found").encode('utf-8'))
                        else:
                            conn.sendall(("HTTP/1.1 Error 400 Bad Request)").encode('utf-8'))
                    else:
                        conn.sendall(("HTTP/1.1 Error 400 Bad Request)").encode('utf-8'))
                elif re.search(r'^POST.*',data): #POST command
                    if re.search(r'^POST /dns-query HTTP/1.1',data):#POST /dns-query HTTP/1.1
                        try:
                            file_data = open("queries.txt","r")#open a file and decode info to utf-8
                            post_lines = file_data.readlines() 
                        except:
                            conn.sendall('500 Internal Server Error\n'.encode('utf-8'))
                            continue
                        if os.stat('queries.txt').st_size > 0:#checks if the size of a file is 0(empty) or greater
                            if first_line == False:
                                first_line = True
                                conn.sendall("HTTP/1.1 200 OK\r\n\r\n".encode('utf-8'))
                            for line in post_lines:
                                if re.search(r'(.+):(A|PTR)',line): #devides a line into groups 1 and 2
                                    line = re.search(r'(.+):(A|PTR)',line)
                                    if line.group(2) == 'A' and line.group:
                                        try:
                                            conn.sendall((line.group(1)+":A="+
                                            socket.gethostbyname(line.group(1))+"\r\n").encode('utf-8'))
                                        except socket.error: #when en error occurs trying to answer
                                            conn.sendall(("HTTP/1.1 404 Not Found").encode('utf-8'))
                                    elif line.group(2) == 'PTR':
                                        try:
                                            conn.sendall((line.group(1)+":PTR="+ 
                                            socket.gethostbyaddr(line.group(1))[0]+"\r\n").encode('utf-8'))# gethostbyaddr returns field of 3 items
                                        except socket.error:
                                            conn.sendall("HTTP/1.1 404 Not Found".encode('utf-8'))
                                else:
                                    conn.sendall(("HTTP/1.1 Error 400 Bad Request)").encode('utf-8'))
                            
                            
                                        
                        else:#file is empty
                            conn.sendall('500 Internal Server Error\n'.encode('utf-8'))
                            continue
                    else:
                        conn.sendall(("HTTP/1.1 Error 400 Bad Request)").encode('utf-8'))



                else:    #Method not allowed, error
                    conn.sendall("405 Method Not Allowed\n".encode('utf-8'))
        conn.close() #close the connection after the while cycle
    except KeyboardInterrupt:
        s.close()
        print('\nPressed CTRL+C, exiting casually with exit(0)\n')
        sys.exit(0)