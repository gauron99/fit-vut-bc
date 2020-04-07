#########################################
#          Login : xfridr08             #
#       Jmeno : David Fridrich          #
#      Tema  : IPP2020 interpret        #
#     Posledni zmena : 07.04.2020       #
#########################################

import xml.etree.ElementTree as ET #import library to work with xml
import sys, os
import re


## ~~~~~~~~~~ functions ~~~~~~~~~~ ##

#prints out given message and exits with desired code
def err_exit(string,rc):
    print ("Error: ",string,"- exiting w/",rc)
    sys.exit(rc)

#validates arguments 
#arguments:
#       opp = what operation is checked
#       num = number of arguments the operation takes
#       
def workArgs(opp, num):
    pass

### ~~~~~~~~~~~~~~~~~~~~~ start of the program ~~~~~~~~~~~~~~~~~~~~~ ###


## ~~~~~~~~~~ parsing the arguments ~~~~~~~~~~ ##
if len(sys.argv) < 2: #no arguments are provided
    err_exit("No arguments given, try '--help'",10)

#definition of 'source' & 'input' so they exist outside the for loop
sourceArg = inputArg = None

for arg in sys.argv[1:]: #[1:] to skip the first iteration
    if (arg == "--help"):
        if(len(sys.argv) > 2):
            err_exit("--help argument has to be given by itself",10)
        else:#output help message
            print("""Help message for Interpet.py
            input/output:
                input: takes in xml representations of ippcode20 code(produced by parser)
                output: outputs the result of the code
            arguments:
                --source=<file>; where <file> is file with XML representation
                --input=<file>; where <file> is a file with input for interpret itself
                    (note: atleast one of above arguments must be given, if one
                    is missing, read from stdin)
                --help; prints out help message
                """)
            sys.exit(0)
        #else end
        #temp is used because whenever the if statement is checked and not matched
        #the value asigned by := is then 'None' therefore only as last argument
        #this would be valid w/ (sourceArg := ...)
    elif (temp := re.match(r'^--source=(.*)',arg)):
        sourceArg = temp.group(1)
        # print ('--source= arg given: ',sourceArg)#debug

        #looks like os.path.isfile() checks absolute as well as relative path
        # (if absolute path is not found, search cwd)
        # print ("path? ",os.path.isfile(sourceArg))#debug
        if not os.path.isfile(sourceArg):
            #error
            err_exit('Could not open file in --source',11)
    elif(temp := re.match(r'^--input=(.*)',arg)):
        inputArg = temp.group(1)
        # print ('--input= arg given: ',inputArg)#debug
        print ("path? ",os.path.isfile(inputArg))
        if not os.path.isfile(inputArg):
            err_exit('Could not open file in --input',11)
    # else:
        # print("Argument given: ",arg)
        # err_exit(("this argument is invalid"),10)
    #if-else end1
#for end

#atleast one of --input | --source must exist
if inputArg == sourceArg == None:
    print ('its a no go bro nemas ani jeden sause')
    # err_exit("""Atleast one of the following arguments must be provided:
    # '--source', '--input'. Use --help for help""",10)

# if inputArg == None:
#     #read from stdin
#     inputArg = sys.stdin.readlines()
#     print ("OUTPUT:\n",inputArg)

elif sourceArg == None:
    sourceArg == sys.stdin.readlines()
    print ("SAUCE: ", sourceArg)


## ~~~~~~~~~~ working with xml input ~~~~~~~~~~ ##


#no need to open a file manually?
# try:
#     f = open(sourceArg,"r")
# except:
#     err_exit('File could not be opened for reading',11)

try:
    tree = ET.parse(sourceArg)
    root = tree.getroot()
except:
    err_exit("XML structure is invalid",31)

if root.tag != "program":
    err_exit("root element expected: 'program'",32)

# print (root.attrib)#debug
#check root element
if 'language' in root.attrib:
    if root.attrib["language"] == "IPPcode20":
        # print("Hlavicka je cajk")#debug
        pass
    else:
        err_exit("value of attribute 'language' expected: IPPcode20",32)
else:
    err_exit("program attribute expected: language",32)

inst_num = 0
#main for cycle to check validity of xml and
for child in root:
    inst_num += 1 #increment instruction number (starting from 1)
    print (child.tag,child.attrib)#debug
    #check element == 'instruction'
    if child.tag != 'instruction':
        err_exit("element 'instruction' expected",32)
    
    #check directories ('order': <number> , 'opcode': <operation name>)
    if 'order' in child.attrib and 'opcode' in child.attrib:
        pass
    else:
        err_exit("attribute 'order' & 'opcode' expected in instruction",32)

    #instruction order must follow      
    if int(child.attrib['order']) != inst_num:
        err_exit("Instruction's 'order' not in order (must be 1,2,3..)",32)

    

    break



#end for (main cycle)

