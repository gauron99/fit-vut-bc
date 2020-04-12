#########################################
#          Login : xfridr08             #
#       Jmeno : David Fridrich          #
#      Tema  : IPP2020 interpret        #
#     Posledni zmena : 07.04.2020       #
#########################################

import xml.etree.ElementTree as ET #import library to work with xml
import sys, os
import re

## ~~~~~~~~~~ classes ~~~~~~~~~~ ##
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

def bubblesort(nlist):
    #n items = n -1 pairs to be compared
    #by first iteration largest number is last(where it should be)
    for i in range(len(nlist)-1,0,-1):
        for j in range(i):
            if nlist[j] > nlist[j+1]:
                temp = nlist[j]
                nlist[j] = nlist[j+1]
                nlist[j+1] = temp

#prints out given message and exits with desired code
def err_exit(string,rc):
    print ("Error: ",string,"- exiting w/",rc)
    sys.exit(rc)

#return true if var exists
def is_var(string,fr):
    try:
        x = re.match(r'^(GF|LF|TF)@(.*)',string)
        if x.group(1) == "GF":
            print ('\n',x.group(2),': Yo whaddup yall Im in GF')
            for key in fr.GF:
                if x.group(2) == key:
                    return True
            return False
        elif x.group(1) == "LF":
            print ('\n',x.group(2),': Hey fellas me in LF')
            for key in fr.LF:
                if x.group(2) == key:
                    return True
            return False
        else:
            print ('\n',x.group(2),': Ello im joinin TF')
            for key in fr.TF:
                if x.group(2) == key:
                    return True
            return False
    except:
        print('Note: (Var might contain unsupported frame(should be - GF/LF/TF))')
        err_exit("Variable couldnt be verified",52)

def is_symb(string,sType,fr):
    if sType == 'var':#has frame, check var
        if is_var(string,fr):
            return True
        return False

    #is symb & not var
    try:
        # sType = int, string, bool, nil
        # {in xml: <arg1 type="sType">string</arg1>}

        #int
        if sType == 'int':
            try:
                int(string)
                # if y == y.__int__(): #no need, if int() fails it raises an error
                return True
            except:
                print ('Note: Value of type int is not an integer')
                return False
        #bool    
        elif sType == 'bool':
            if string == 'true' or string == 'false':
                return True
            print ('Note: Type bool should be "bool@<true/false>"')
            return False
        #str
        elif sType == 'string':
            return True
        #nil
        elif sType == 'nil':
            if string == 'nil':
                return True
            else:
                print('Note: Type nil supported for "nil@nil" only')
                return False #error return + message after each check

    except:
        err_exit("Unsupported symbol type",52)



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

instrlist = []
#bubblesort
for child in root:
    if int(child.attrib['order']) <= 0: #check for negative values
        err_exit("Negative instruction number in 'order' is not allowed",32)
    instrlist.append(int(child.attrib['order']))
bubblesort(instrlist)


#cycle to put instructions in order (from lowest (1))
instr = []
i=0
for i in range(len(instrlist)):
    i+=1
    for child in root:
        if instrlist[i-1] == int(child.attrib['order']):     
            instr.append(child)
            break

order = []
instCnt = 0
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

    #check for duplicity
    for i in order:
        if i == int(child.attrib['order']):
            err_exit("Duplicity detected in instructions",32)
    order.append(int(child.attrib['order']))
        
    lab = child.attrib['opcode'].upper()
    if lab == "LABEL":
        if child.find('arg1').text in myLabelList:
            err_exit("Redefinition of label not allowed",52)
        else:
            #myLabelList.append(child.find('arg1').text)
            myLabelList[child.find('arg1').text] = instCnt
        #TODO this should work, not tested tho

print ('~~~~~~~~~~Debug time~~~~~~~~~~\n')#####################################################################

print('Labels: ',myLabelList)
i = 0

print ('All instructions:')
while i < len(instr):
    i += 1
    
    print('Inst',i,': ',instr[i-1].attrib, ", ",end='')
    for c in instr[i-1]:
        print(c.text," ",end='')
    print('')
# fr.GF['maggie'] = 4
fr.GF['bitch'] = 4
# fr.GF['fucker'] = 4
fr.GF['sabina'] = 4
g1 = "GF@maggie"
g2 = "GF@bitch"
g4 = "GF@fucker"
g3 = "GF@sabina"
fr.LF = {}
fr.LF['bob']= 4
# fr.LF['ivstypek']= 2
fr.LF['whodis']= 1
l1 = "LF@bob"
l2 = "LF@ivstypek"
l3 = "LF@whodis"

fr.TF = {}
fr.TF['Myman']= 4
# fr.TF['badboi']= 5
# fr.TF['jerry']= 5
t1 ="TF@Myman"
t2 ="TF@badboi"
t3 ="TF@jerry"

d = []
d.append(g1)
d.append(g2)
d.append(g3)
d.append(g4)

d.append(l1)
d.append(l2)
d.append(l3)

d.append(t1)
d.append(t2)
d.append(t3)

# for da in d:
#     y = is_var(da,fr)
#     if y:
#         print ('God: ye its true\n')
#     else:
#         print ('God: aint gonn happen\n')
# s = 'int@+5'
# boo = is_symb(s,fr)
# if boo:
#     print ('NICE its cislo')
# else:
#     print ('damn what a shame no cislo')#

#int
b2 = 666
a1 = 'GF@bitch'

#string
b2 = 'DOPICEUZ'
a1 = 'LF@bob'

a = re.match(r'(.*)@(.*)',a1)
if a.group(1) == "GF":
    for arg in fr.GF:
        if arg == a.group(2):
            fr.GF[arg] = b2
            break
elif a.group(1) == "LF":
    for arg in fr.LF:
        if arg == a.group(2):
            fr.LF[arg] = b2
            break
else: #a.group(1) == "TF" 
    for arg in fr.TF:
        if arg == a.group(2):
            fr.TF[arg] = b2
            break

print('\n~~~~~~~~~~Debug time over~~~~~~~~~~\n') #################################################################################
##debug print of all frames
print ('\nAll frames:')
print ('GF: ',fr.GF)
print ('LF: ',fr.LF)
print ('TF: ',fr.TF)

exit(0)
## ~~~~~~~~~~~~~~~ main cycle ~~~~~~~~~~~~~~~ ##

actLine = 0

while actLine < len(instr):
    actLine += 1
    child = instr[actLine - 1]
    
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

    print ('arg1: ',opp.arg1) 
    print ('arg2: ',opp.arg2) 
    print ('arg3: ',opp.arg3) 
    
        
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
        sys.stderr.write("--------------- BREAK debug ---------------\n\n")
        sys.stderr.write("Current state\n")
        sys.stderr.write("Global frame: ", fr.GF,"\n\n","Local frame: ", fr.LF,"\n\n","Temporary frame: ",fr.TF,"\n\n")
        sys.stderr.write("Currently on instruction #",actLine,"\n")
        try:
            sys.stderr.write("Last instruction processed: ",instr[actLine -2].attrib['oppcode'],"\n")
        except:
            sys.stderr.write("Last instruction processed: None(This is the first instruction)\n")
        try:
            sys.stderr.write("Next instruction to process: ",instr[actLine].attrib['oppcode'],"\n")
        except:
            sys.stderr.write("Next instruction to process: None(This is the last instruction)\n")
        sys.stderr.write("--------------- BREAK debug end ---------------\n\n")

    elif opp.name == 'MOVE': # var, symb
        a1,a2 = opp.arg1
        b1,b2 = opp.arg2
        if a1 != 'var':
            err_exit("MOVE: first arg should be <var>",53)
        if not is_var(a2,fr):
            err_exit("1st arg in MOVE is not var",53)
        if not is_symb(b2,b1,fr): #b1 = type (for symbols like int,string,etc)
            err_exit("2nd arg in MOVE is not symb",53)
        #is_compatible(opp)

        a = re.match(r'^(.*)@(.*)',a2)
        if a.group(1) == "GF":
            for arg in fr.GF:
                if arg == a.group(2):
                    GF.fr[arg] = b2
                    break
        elif a.group(1) == "LF":
            for arg in fr.GF:
                if arg == a.group(2):
                    LF.fr[arg] = b2
                    break
        else: #a.group(1) == "TF" 
            for arg in fr.GF:
                if arg == a.group(2):
                    LF.fr[arg] = b2
                    break
    # elif opp.name == 'DEFVAR':
       
    # elif opp.name == 'CALL':
    #     pass
    # elif opp.name == 'PUSHS':
    #     pass
    # elif opp.name == 'POPS':
    #     pass
    # elif opp.name == 'ADD':
    #     pass
    # elif opp.name == 'SUB':
    #     pass
    # elif opp.name == 'MUL':
    #     pass
    # elif opp.name == 'IDIV':
    #     pass
    # elif opp.name == 'LT': 
    #     pass
    # elif opp.name == 'GT':
    #     pass
    # elif opp.name == 'EQ':
    #     pass
    # elif opp.name == 'AND':
    #     pass
    # elif opp.name == 'OR':
    #     pass
    # elif opp.name == 'NOT':
    #     pass
    # elif opp.name == 'INT2CHAR':
    #     pass
    # elif opp.name == 'STRI2INT':
    #     pass
    # elif opp.name == 'READ':
    #     for subChild in child:
    #         workArgs("read",subChild)
    #         pass
    # elif opp.name == 'WRITE':
    #     pass
    # elif opp.name == 'CONCAT':
    #     pass
    # elif opp.name == 'STRLEN':
    #     pass
    # elif opp.name == 'SETCHAR':
    #     pass
    # elif opp.name == 'TYPE':
    #     pass
    # elif opp.name == 'LABEL':  ## this is done before the cycle, so jump on later
    #     pass              ## defined label in code is possible
    # elif opp.name == 'JUMP':
    #     pass
    # elif opp.name == 'JUMPIFEQ':
    #     pass
    # elif opp.name == 'JUMPIFNEQ':
    #     pass
    # elif opp.name == 'EXIT':
    #     pass
    # elif opp.name == 'DPRINT':
    #     pass
    else:
        print ('well fuck takovou operaci neznam')
        sys.exit(0) 
    # break
#end while (main cycle)
#end of function progr


##debug print of all frames
print ('\nAll frames:')
print ('GF: ',fr.GF)
print ('LF: ',fr.LF)
print ('TF: ',fr.TF)