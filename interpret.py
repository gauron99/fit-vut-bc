#########################################
#          Login : xfridr08             #
#       Jmeno : David Fridrich          #
#      Tema  : IPP2020 interpret        #
#     Posledni zmena : 04.04.2020       #
#########################################

import xml.etree.ElementTree as ET #import library to work with xml
import sys, re


## ~~~~~~~~~~ functions ~~~~~~~~~~ ##

#function prints out given message and exits with desired code
def err_exit(string,rc):
    print ("Error: ",string,"- exiting w/",rc)
    sys.exit(rc)





### ~~~~~~~~~~~~~~~~~~~~~ start of the program ~~~~~~~~~~~~~~~~~~~~~ ###


## ~~~~~~~~~~ parsing the arguments ~~~~~~~~~~ ##

if len(sys.argv) < 2: #no arguments are provided
    err_exit("No arguments given, try '--help'",10)
print("~~~~~ arguments given ~~~~~")
print (str(sys.argv))
print("~~~~~ arguments end ~~~~~~~")

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
        #temp must be used because whenever the if statement is checked and not matched
        #the value asigned by := is then 'None' therefore only as last argument
        #this would be valid w/ (sourceArg := ...)
    elif (temp := re.match(r'^--source=(.*)',arg)):
        sourceArg = temp.group(1)
        print ('--source= arg given: ',sourceArg)
    elif(temp := re.match(r'^--input=(.*)',arg)):
        inputArg = temp.group(1)
        print ('--input= arg given: ',inputArg)

    # else:
        # print("Argument given: ",arg)
        # err_exit(("this argument is invalid"),10)
    #if-else end
#for end

#atleast one of --input | --source must exist
if inputArg == sourceArg == None:
    print ('its a no go bro nemas ani jeden sause')
    # err_exit("""Atleast one of the following arguments must be provided:
    # '--source', '--input'. Use --help for help""",10)

if inputArg == None:
    #read from stdin
    inputArg = sys.stdin.readlines()
    print ("OUTPUT:\n",inputArg)

elif sourceArg == None:
    sourceArg == sys.stdin.readlines()
    print ("SAUCE: ", sourceArg)


