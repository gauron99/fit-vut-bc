#########################################
#          Login : xfridr08             #
#       Jmeno : David Fridrich          #
#      Tema  : IPP2020 interpret        #
#     Posledni zmena : 07.04.2020       #
#########################################

import xml.etree.ElementTree as ET #import library to work with xml
import sys, os
import re

import hashlib


class Opperation():
    def __init__(self,name,arg1,arg2,arg3):
        self.name  = name
        self.arg1 = arg1
        self.arg2 = arg2
        self.arg3 = arg3

class Frames():
    def __init__(self,GF,LF,TF,LFcnt):
        self.GF = GF
        self.LF = LF
        self.TF = TF
        self.LFcnt = LFcnt
        
## ~~~~~~~~~~ functions ~~~~~~~~~~ ##

#prints out given message and exits with desired code
def err_exit(string,rc):
    print ("Error: ",string,"- exiting w/",rc)
    sys.exit(rc)

#work with arguments one by one
# opp = opperation
def workArgs(opp,child):
    print ('in workArgs(',opp,'):~~~~~~~~~~~~~')
    global GF, LF, TF
    print('YOYOYOY ',child)
    try:
        arg = child.attrib['type']
        text = child.text
        pass
    except:
        print(arg,text)
        err_exit("Expected attribute 'type' in argument",32)
    
    text = re.match(r'(GF|LF|TF)@(.*)',text)#cut text into groups

    if opp == 'defvar':
        if arg == 'var':        
            print(text.group(2))#debug
            if text.group(1) == 'GF':
                GF[text.group(2)] = tuple() #is defined, not inicialized, therefore None
    elif opp == 'read':
        print ('Hey read')
        if arg == 'var':
                if text.group(1) == 'GF':
                    for i in GF:
                        if text.group(2) in i:
                            print ('JE TO TAM')

        elif arg == 'type':
            pass
        else:
            print ('asi chybka')

    print ('~~~~~~~~~~~~~~~~~~~~~~~~~~~')

## ~~~~~~~~~~ frames & lists of labels and support call/return func ~~~~~~~~~~ ##
myLabelList = {} #dictionary of labels & their instr # {"label": <number>, ...}
myCallList = [] #list of positions
GF = {} #global frame
LF = [] #local frame (stack of dictionaries)
TF = None #temporary frame
LFcounter = 5 #keeps count how many frames there are(also points which the most top one is)

fr = Frames(GF,LF,TF,LFcounter)




### ~~~~~~~~~~~~~~~~~~~~~ __MAIN__ start of the program ~~~~~~~~~~~~~~~~~~~~~ ###


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
    err_exit("""Atleast one of the following arguments must be provided:
    '--source', '--input'. Use --help for help""",10)

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

#check root element
if 'language' in root.attrib:
    if root.attrib["language"] == "IPPcode20":
        # print("Hlavicka je cajk")#debug
        pass
    else:
        err_exit("value of attribute 'language' expected: IPPcode20",32)
else:
    err_exit("program attribute expected: language",32)


#cycle to save everything to a variable so its more excesible for call & return
instr = []
for child in root:
    instr.append(child)

order = []

instCnt = 0
print ('instr length: ',len(instr))
#label cycle ( + xml basic structure validity check)
while instCnt < len(instr):
    instCnt += 1
    child = instr[instCnt-1]
    #check element == 'instruction'
    if child.tag != 'instruction':
        if 'arg' in child.tag:
            print ("Note: argument might be outside instruction")
        err_exit("element 'instruction' expected",32)
    
    #check dictionary ('order': <number> , 'opcode': <operation name>)
    if 'order' in child.attrib and 'opcode' in child.attrib:
        pass #all good
    else:
        err_exit("attributes 'order' & 'opcode' expected in elem instruction",32)

    if int(child.attrib['order']) > 0:
        for i in order:
            if i == int(child.attrib['order']):
                err_exit("Duplicity detected in instructions",32)
        order.append(int(child.attrib['order']))
        pass 
    else:
        err_exit("Negative order of instructions is not allowed",32)
    lab = child.attrib['opcode'].upper()
    if lab == "LABEL":
        if child.find('arg1').text in myLabelList:
            err_exit("Redefinition of label not allowed",52)
        else:
            #myLabelList.append(child.find('arg1').text)
            myLabelList[child.find('arg1').text] = instCnt
        #TODO this should work, not tested tho

print ('~~~~~~~~~~Debug time~~~~~~~~~~\n')

print('Labels: ',myLabelList)
i = 0
print ('All instructions:')
while i < len(instr):
    i += 1
    print('Inst',i,': ',instr[i-1].attrib)

print('\n~~~~~~~~~~Debug time over~~~~~~~~~~\n')
exit(0)
## ~~~~~~~~~~~~~~~ main cycle ~~~~~~~~~~~~~~~ ##
# def progr(instr):
#     global myLabelList 
#     global myCallList 
#     global GF 
#     global LF
#     global LFcounter 
#     global TF
#     global instCnt
actLine = 0

while actLine < len(instr):
    actLine += 1
    child = instr[actLine - 1]
    # print (child.tag,child.attrib)#debug
    #inicialization of operation
    opp = Opperation(child.attrib['opcode'].upper(),{},{},{})
    # opp.name = current opperation name (ex: LABEL)

    argcnt = 0
    for sub in child:
        argcnt +=1
        if argcnt == 1:
            opp.arg1[sub.attrib['type']] = sub.text 
        elif argcnt == 2:
            opp.arg2[sub.attrib['type']] = sub.text 
        else:
            opp.arg3[sub.attrib['type']] = sub.text 

    
    # upper() #case-insensitive
    if opp.name == 'CREATEFRAME':
            fr.TF = {} #easy enough
    elif opp.name == 'PUSHFRAME':
        if fr.TF == {}:
            err_exit("Trying to push TF to LF buffer, but TF is undefined",55)
        if fr.LFcnt == 0: #first one
            fr.LF = [dict()] #inicialize list
            fr.LF[LFcnt] = fr.TF
        fr.LF.append(fr.TF)
        fr.TF = {}
        fr.LFcnt += 1
    elif opp.name == 'POPFRAME':
        if fr.LF == [] or fr.LF == [{}]:
            err_exit("Trying to pop LF, but none is available",55)
        fr.TF = fr.LF[fr.LFcnt-1]
        fr.LF.pop(fr.LFcnt-1)
        fr.LFcnt -= 1
    elif opp.name == 'RETURN':
        if myCallList == None:
            err_exit("Trying to use return, but no 'calls' were made",56)
        num = len(myCallList)
        actLine = myCallList[num-1]
        myCallList.pop()
    elif opp.name == 'BREAK': # < & ^ these take no arguments
        pass
    elif opp.name == 'MOVE':
        for subChild in child:
            pass
        pass
    elif opp.name == 'DEFVAR':
        for subChild in child:
            workArgs("defvar",subChild)
            pass
    elif opp.name == 'CALL':
        pass
    elif opp.name == 'PUSHS':
        pass
    elif opp.name == 'POPS':
        pass
    elif opp.name == 'ADD':
        pass
    elif opp.name == 'SUB':
        pass
    elif opp.name == 'MUL':
        pass
    elif opp.name == 'IDIV':
        pass
    elif opp.name == 'LT': 
        pass
    elif opp.name == 'GT':
        pass
    elif opp.name == 'EQ':
        pass
    elif opp.name == 'AND':
        pass
    elif opp.name == 'OR':
        pass
    elif opp.name == 'NOT':
        pass
    elif opp.name == 'INT2CHAR':
        pass
    elif opp.name == 'STRI2INT':
        pass
    elif opp.name == 'READ':
        for subChild in child:
            workArgs("read",subChild)
            pass
    elif opp.name == 'WRITE':
        pass
    elif opp.name == 'CONCAT':
        pass
    elif opp.name == 'STRLEN':
        pass
    elif opp.name == 'SETCHAR':
        pass
    elif opp.name == 'TYPE':
        pass
    elif opp.name == 'LABEL':  ## this is done before the cycle, so jump on later
        pass              ## defined label in code is possible
    elif opp.name == 'JUMP':
        pass
    elif opp.name == 'JUMPIFEQ':
        pass
    elif opp.name == 'JUMPIFNEQ':
        pass
    elif opp.name == 'EXIT':
        pass
    elif opp.name == 'DPRINT':
        pass
    else:
        print ('well fuck takovou operaci neznam')
        sys.exit(0) 
    # break
#end while (main cycle)
#end of function progr


##debug print of all frames
print ('All frames:')
print ('GF: ',fr.GF)
print ('LF: ',fr.LF)
print ('TF: ',fr.TF)