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

def bubblesort(nlist): #to order instruction ascending (vzezstupne = vzrustajici hodnoty)
    #n items = n -1 pairs to be compared
    #by first iteration largest number is last(where it should be)
    for i in range(len(nlist)-1,0,-1):
        for j in range(i):
            if nlist[j] > nlist[j+1]:
                temp = nlist[j]
                nlist[j] = nlist[j+1]
                nlist[j+1] = temp

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#prints out given message and exits with desired code
def err_exit(string,rc):
    print ("Error: ",string,"- exiting w/",rc)
    sys.exit(rc)

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#return tuple (pair) like this -> (type,value) (aka same as symbol)
def gib_var_val(string): #returns the correct var in frame
    x = re.match(r'^(GF|LF|TF)@(.*)',string)
    
    if x.group(1) == "GF":
        for key in fr.GF:
            if x.group(2) == key:
                return fr.GF[key]
    elif x.group(1) == "LF":
        for key in fr.LF[fr.LFcnt-1]:
            if x.group(2) == key:
                return fr.LF[fr.LFcnt-1][key]
    else:#TF
        for key in fr.TF:
            if x.group(2) == key:
                return fr.TF[key]

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

def is_good_str(string):
    try:
        string = str(string)
    except:
        return False
    if re.match(r'#',string):
        return False
    if re.match(r'\\',string):
        if re.match(r'\\\d{3}',string):
            return True
        else:
            return False
    return True

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

def str_escape_seq(string):
    if is_good_str(string) is False:
        err_exit("String contains ilegal characters",57)
    try:
        string = str(string)
    except:
        err_exit("String contains ilegal characters",57)
    final = ''
    tss = '' # totaly super string
    done = True

    for c in string:
        if done is False: #loading sequence
            tss += c
            if len(tss) == 3:
                final += chr(int(tss))
                done = True

        elif c == '\\': #begin to escape
            done = False
        else:
            final += c
    return final

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

def work_args():
    for m1,m2 in opp.arg1.items():
        for n1,n2 in opp.arg2.items():
            if opp.arg3 is not None: #if only 2 arguments are needed (NOT & INT2CHAR)
                for o1,o2 in opp.arg3.items():
                    break
            break
        break
    if (is_var(m2,m1) is False) or (is_symb(n2,n1) is False):
            err_exit("ADD/SUB/MUL/IDIV: arguments don't match type. Expected: <var> <symb1> <symb2>",53)
    
    m = re.match(r'^(GF|LF|TF)@(.*)',m2)

    #operations take only 2args (as ooposed to all other arithmetic, relational, boolean or conversion instructions)   
    if opp.name == 'NOT': #only 2 operands
        if is_var(n2,n1):
            n1,n2 = gib_var_val(n2)

        if n1 == 'bool':
            if n2 == 'true':
                temp = [n1, 'false']
            elif n1 == 'false':
                temp = [n1, 'true']
            else:
                err_exit("NOT: value of type <bool> can only be true / false",57)
        else:
            err_exit("NOT: second argument type expected: <bool>",53)
        
    elif opp.name == 'INT2CHAR': #only 2 operands
        if is_var(n2,n1):
            n1,n2 = gib_var_val(n2)
        if n1 == 'int':
            try:
                n2 = int(n2)
            except:
                err_exit("INT2CHAR: value is not an integer",57)
            try:
                temp = ['string',chr(n2)]
            except:
                err_exit("INT2CHAR: Value is not valid value in Unicode",58)
        else:
            err_exit('INT2CHAR: second argument expected: <int>',53)

    elif opp.name == 'ADD' or opp.name == 'SUB' or opp.name == 'MUL' or opp.name == 'IDIV':

        #operations all take 3 arguments <var> <symb1> <symb2>
        if (is_symb(o2,o1) is False):
            err_exit("ADD/SUB/MUL/IDIV: arguments don't match type. Expected: <var> <symb1> <symb2>",53)

        try:
            if is_var(n2,n1):
                n1,n2 = gib_var_val(n2)
            else:
                n2 = int(n2)
            if is_var(o2,o1):
                o1,o2 = gib_var_val(o2)
            else:
                o2 = int(o2)
        except:
            pass
        #check if 2nd & 3rd args are int
        if n1 != 'int' or o1 != 'int':
            err_exit("ADD/SUB/MUL/IDIV: 2nd & 3rd argument must be of type int",53)
        

        #check if not dividing by 0
        if opp.name == 'IDIV':
            if int(o2) == 0:
                err_exit("IDIV: Dividing by 0 is not my cup of tea",57) 

        if opp.name == 'ADD':
            temp = ['int',n2 + o2]
        elif opp.name == 'SUB':
            temp = ['int',n2 - o2]
        elif opp.name == 'IDIV':
            temp = ['int',n2 // o2]
        else: #MUL
            temp = ['int',n2 * o2]

    elif opp.name == 'LT' or opp.name == 'GT' or opp.name == 'EQ':
        if is_var(n2,n1):
                n1,n2 = gib_var_val(n2)
        else:
            if n1 == 'int':
                n2 = int(n2)
        if is_var(o2,o1):
            o1,o2 = gib_var_val(o2)
        else:
            if o1 == 'int':
                o2 = int(o2)

        if (n1 == 'int' and o1 == 'int') or (n1 == 'string' and o1 == 'string') or (n1 == 'bool' and o1 == 'bool'): #both int
            if opp.name == 'LT' and n1 < o1: #LT true
                temp = ['bool', 'true']
            elif opp.name == 'GT' and n1 > o1: #GT tru
                temp = ['bool', 'true']
            elif opp.name == 'EQ' and n1 == o1: #EQ tru
                temp = ['bool', 'true']
            else:
                temp = ['bool', 'false']
        elif opp.name == 'EQ': #its possible to compare something to nil@nil
            if n1 == 'nil' and o1 == 'nil':
                temp = ['bool', 'true']
            else:
                temp = ['bool', 'false']
                

        else: #err
            err_exit("LT/GT/EQ: expected 2 same argument types(int / string / bool)",53)

    
    elif opp.name == 'AND' or opp.name == 'OR':
        if n1 == 'bool' and o1 == 'bool':
            if opp.name == 'AND' and n2 == 'true' and o2 == 'true':
                temp = ['bool','true']
            elif opp.name == 'OR' and (n2 == 'true' or o2 == 'true'):
                temp = ['bool','true']
            else:
                temp = ['bool','false']

    elif opp.name == 'STRI2INT': #symb1 retezec; symb2 pozice v retezci
        if n1 != 'string' and o1 != 'int':
            err_exit("STRI2INT: expected types of args: (arg2 -> string)( arg3 -> int)",53)
        try:
            tem = ord(n2[o2])
            temp = ['int',tem]#indexing from 0
        except:
            err_exit("STRI2INT: char cant be converted to integer or index is out of bound",58)
    else:
        err_exit('''Unknown error. Unsupported instruction in 'work_args()'. Function for arithmetic, relational,
         boolean and conversion instructions''',99)

    #assign values
    if m.group(1) == 'GF':
        fr.GF[m.group(2)] = temp
    elif m.group(1) == 'LF':    
        fr.LF[fr.LFcnt-1][m.group(2)] = temp
    else: #TF
        fr.TF[m.group(2)] = temp

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#return if frame is exists
def fr_exist(frame):
    #GF exists from the start of the program (& always)
    if frame == "TF":
        if fr.TF == None:
            return False
        return True
    elif frame == "LF": 
        if fr.LF == None:
            return False
        return True

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

def label_exist(label):
    for i in myLabelList:
        if label == i:
            return True
    return False

##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#return true if var exists
def is_var(string,typ):
    if typ != 'var':
        return False
    try:
        x = re.match(r'^(GF|LF|TF)@(.*)',string)
        if x.group(1) == "GF":
            for key in fr.GF:
                if x.group(2) == key:
                    return True
            return False
        elif x.group(1) == "LF":
            for key in fr.LF[fr.LFcnt-1]:
                if x.group(2) == key:
                    return True
            return False
        else:
            for key in fr.TF:
                if x.group(2) == key:
                    return True
            return False
    except:
        # print("Note: possibly used <symb> instead of <var> or wrong frame")
        err_exit("Variable couldnt be verified",55)
        
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~        

def is_symb(string,sType):
    if sType == 'var':#has frame, check var
        if is_var(string,sType):
            return True
        return False

    #is symb & not var
        # sType = int, string, bool, nil
        # {in xml: <arg1 type="sType">string</arg1>}

    #int
    if sType == 'int':
        try:
            int(string)

            # if y == y.__int__(): #no need, if int() fails it raises an error
            return True
        except:
            err_exit('Value of type int is not an integer',32)
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


##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
##~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

## ~~~~~~~~~~~~~~~~~~~~~~~~ frames & labels & stack ~~~~~~~~~~~~~~~~~~~~~~~~ ##
myLabelList = {} #dictionary of labels & their instr # {"label": <number>, ...}
myCallList = [] #list of positions
myStack = [] #for stack instructions
stackCnt = 0 #how many 'things' are in stack
firstReading = True
lines = []
#inicialized GF,LF,TF,LFcnt; fr = Frames class
fr = Frames({},None,None,0)

#note: to access lists(LF,labels,calls,stack, index of last item is 'counter - 1')


##### ~~~~~~~~~~~~~~~~~~~~~ __MAIN__ start of the program ~~~~~~~~~~~~~~~~~~~~~ #####


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
                --input=<file>; where <file> is a file with input 

                (note: atleast one of above arguments must be given, if one is missing, read from stdin)
                --help; prints out help message

                """)
            sys.exit(0)
        #else end
        #temp is used because whenever the if statement is checked and not matched
        #the value asigned by := is then 'None' therefore only as last argument
        #this would be valid w/ (sourceArg := ...)
    elif (temp := re.match(r'^--source=(.*)',arg)):
        sourceArg = temp.group(1)

        #looks like os.path.isfile() checks absolute as well as relative path
        # (if absolute path is not found, search cwd)
        if not os.path.isfile(sourceArg):
            err_exit('Could not open file in --source',11)

    elif(temp := re.match(r'^--input=(.*)',arg)):
        inputArg = temp.group(1)
        if not os.path.isfile(inputArg):
            err_exit('Could not open file in --input',11)
        f=open(inputArg,"r")
        inputArg =''
        try:
            for line in f:
                inputArg += line
            else:
                pass #nothing is done if nothing is read (although inputArg is now NOT None)
        except EOFError:
            pass
    else:
        print("Note: Argument given: ",arg)
        err_exit(("this argument is invalid"),10)
    #if-else end1
#for end

#atleast one of --input | --source must exist
if inputArg == sourceArg == None:
    err_exit("""Atleast one of the following arguments must be provided:
    '--source', '--input'. Use --help for help""",10)

if inputArg == None: #read from stdin
    inputArg=''
    try:
        while True:
            inputArg += input()+"\n"
    except EOFError:
        pass

elif sourceArg == None:
    sourceArg=''
    try:
        while True:
            sourceArg += input()+"\n"
    except EOFError:
        pass

## ~~~~~~~~~~ working with xml input ~~~~~~~~~~ ##

try:
    tree = ET.parse(sourceArg) #sourceArg is a file
    root = tree.getroot()
except:
    try:
        root = ET.fromstring(sourceArg) #its a string
    except:
        err_exit("XML structure is invalid",31)



if root.tag != "program":
    err_exit("root element expected: 'program'",32)

#check root element
if 'language' in root.attrib:
    if root.attrib["language"] == "IPPcode20":
        pass
    else:
        err_exit("value of attribute 'language' expected: IPPcode20",32)
else:
    err_exit("program attribute expected: language",32)

instrlist = []
#bubblesort
for child in root:
    for arg in child: #check arguments
        if((arg.tag != 'arg1') and (arg.tag != 'arg2') and (arg.tag != 'arg3')):
            err_exit("Arguments have set names (arg1,arg2,arg3)",32)

    try:
        if int(child.attrib['order']) <= 0: #check for negative values
            err_exit("Negative instruction number in 'order' is not allowed",32)
        instrlist.append(int(child.attrib['order']))
    except:
        err_exit("Attribute of instruction has to be 'order'",32)
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

## ~~~~~~~~~~~~~~~ Main cycle ~~~~~~~~~~~~~~~ ##

actLine = 0

while actLine < len(instr):
    actLine += 1
    child = instr[actLine - 1]
    
    argcnt = 0

    opp = Opperation(child.attrib['opcode'].upper(),None,None,None)
    
    for sub in child: #load arguments into their correct possitions (ascending)
        argcnt += 1
        if sub.tag == 'arg1':
            opp.arg1 = {}
            opp.arg1[sub.attrib['type']] = sub.text 
        elif sub.tag == 'arg2':
            opp.arg2 = {}
            opp.arg2[sub.attrib['type']] = sub.text 
        else:
            opp.arg3 = {}
            opp.arg3[sub.attrib['type']] = sub.text
        if argcnt > 3:
            err_exit("Wrong number of arguments for instruction",32)
            

    if opp.arg3 is not None: #check if no arguments are missing acordingly (if 3rd is loaded, 1st & 2nd must be as well)
        if opp.arg2 is not None:
            if opp.arg1 is not None:
                pass
            else:
                #err arg1 is not loaded but 3 is (therefore missing argument)
                err_exit("Missing argument 1",32)
        else:
            #err arg2 is not loaded but 3 is
            err_exit("Missing argument 2 or 1",32)
    elif opp.arg2 is not None:
        if opp.arg1 is not None:
            pass
        else:
            #err arg2 is not loaded but
            err_exit("Missing argument 1",32)
    else:
        pass # first arg has no restrictions (can be by itself)


    if opp.arg1 is not None:
        for a1,a2 in opp.arg1.items():
            if opp.arg2 is not None:
                for b1,b2 in opp.arg2.items():
                    if opp.arg3 is not None:
                        for c1,c2 in opp.arg3.items():
                            break
                    break
            break
    

#main SWITCH-like part        
    if opp.name == 'CREATEFRAME':
        if argcnt != 0:
            err_exit("Wrong number of arguments for instruction",32)
        fr.TF = {} #easy enough

    elif opp.name == 'PUSHFRAME':
        if argcnt != 0:
            err_exit("Wrong number of arguments for instruction",32)

        if fr.TF == None:
            err_exit("Trying to push TF to LF buffer, but TF is undefined",55)
        if fr.LFcnt == 0: #first one
            fr.LF = [{}] #inicialize list
            fr.LF[fr.LFcnt] = fr.TF
        else:
            fr.LF.append(fr.TF)
        fr.TF = None
        fr.LFcnt += 1

    elif opp.name == 'POPFRAME':
        if argcnt != 0:
            err_exit("Wrong number of arguments for instruction",32)
            
        if fr.LF == [] or fr.LF == None:
            err_exit("Trying to pop LF, but none is available",55)
        fr.TF = fr.LF[fr.LFcnt-1]
        fr.LF.pop(fr.LFcnt-1)
        fr.LFcnt -= 1

    elif opp.name == 'RETURN':
        if argcnt != 0:
            err_exit("Wrong number of arguments for instruction",32)

        if myCallList == None:
            err_exit("RETURN: Trying to ret, but no 'calls' were made",56)
        num = len(myCallList)
        actLine = myCallList[num-1]
        myCallList.pop()

    elif opp.name == 'BREAK': # < & ^ these take no arguments
        if argcnt != 0:
            err_exit("Wrong number of arguments for instruction",32)

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
        if argcnt != 2:
            err_exit("Wrong number of arguments for instruction",32)
        
        if is_var(a2,a1) is False:
            err_exit("MOVE: 1st argument type expected <var>",53)
        if is_symb(b2,b1) is False: #b1 = type (for symbols like int,string,etc)
            err_exit("2nd arg in MOVE is not symb",53)
        if b1 == 'int': #convert to int so its truly int in frames
            b2 = int(b2)

        a = re.match(r'^(.*)@(.*)',a2)
        if a.group(1) == "GF":
            for arg in fr.GF:
                if arg == a.group(2):
                    #symb = int,string,bool,nil #var = steal
                    if (b1 == 'var') and (b := re.match(r'^(.*)@(.*)',b2)):
                        if b.group(1) == "GF":
                            fr.GF[arg] = fr.GF[b.group(2)]
                        elif b.group(1) == "LF":
                            fr.GF[arg] = fr.LF[fr.LFcnt-1][b.group(2)]
                        else:#TF
                            fr.GF[arg] = fr.TF[b.group(2)]
                    else:
                        fr.GF[arg] = [b1,b2]
                    break
        elif a.group(1) == "LF":
            for arg in fr.LF[fr.LFcnt-1]:
                if arg == a.group(2):
                    if (b1 == 'var') and (b := re.match(r'^(.*)@(.*)',b2)):
                        if b.group(1) == "GF":
                            fr.LF[fr.LFcnt-1][arg] = fr.GF[b.group(2)]
                        elif b.group(1) == "LF":
                            fr.LF[fr.LFcnt-1][arg] = fr.LF[fr.LFcnt-1][b.group(2)]
                        else:#TF
                            fr.LF[fr.LFcnt-1][arg] = fr.TF[b.group(2)]
                    else:
                        fr.LF[fr.LFcnt-1][arg] = [b1,b2]
                    break
        else: #a.group(1) == "TF" 
            for arg in fr.TF:
                if arg == a.group(2):
                    if (b1 == 'var') and (b := re.match(r'^(.*)@(.*)',b2)):
                        if b.group(1) == "GF":
                            fr.TF[arg] = fr.GF[b.group(2)]
                        elif b.group(1) == "LF":
                            fr.TF[arg] = fr.LF[fr.LFcnt-1][b.group(2)]
                        else:#TF
                            fr.TF[arg] = fr.TF[b.group(2)]
                    else:
                        fr.TF[arg] = [b1,b2]
                    break

    elif opp.name == 'DEFVAR': # var
        if argcnt != 1:
            err_exit("Wrong number of arguments for instruction",32)
        if a1 != 'var':
            err_exit("DEFVAR: arg should be of type <var>",53)
        
        a = re.match(r'^(GF|LF|TF)@(.*)',a2)
        if a.group(1) == 'GF':
            for key in fr.GF:
                if key == a.group(2):
                    err_exit("DEFVAR: defining a variable that already exists",52)
            fr.GF[a.group(2)] = [None,None]
        elif a.group(1) == "LF": 
            if fr_exist('LF') is False:
                err_exit("DEFVAR: LF doesn't exist",55)
            for key in fr.LF[fr.LFcnt-1]:
                if key == a.group(2):
                    err_exit("DEFVAR: defining a variable that already exists in LF",52)
            fr.LF[fr.LFcnt-1][a.group(2)] = [None,None]
        else:#TF
            if fr_exist('TF') is False:
                err_exit("DEFVAR: TF doesn't exist",55)
            for key in fr.TF:
                if key == a.group(2):
                    err_exit("DEFVAR: defining a variable that already exists in TF",52)
            fr.TF[a.group(2)] = [None,None]
            
    elif opp.name == 'CALL': #label
        if argcnt != 1:
            err_exit("Wrong number of arguments for instruction",32)
        if label_exist(a2) is False:
            err_exit("CALL: Label refered to by CALL doesn't exist",56)
        myCallList.append(actLine)
        actLine = myLabelList[a2]
    # else: #if no iteration of for is made, do else (smart snake)
        # err_exit("CALL: requires 1 argument (label)",32) ?

    elif opp.name == 'PUSHS': #symb
        if argcnt != 1:
            err_exit("Wrong number of arguments for instruction",32)
        if is_symb(a2,a1) is False:
            err_exit("PUSHS: argument is not <symb>",53)
        myStack.append([a1,a2])
        stackCnt += 1
            
    elif opp.name == 'POPS': #var
        if argcnt != 1:
            err_exit("Wrong number of arguments for instruction",32)
        if myStack == []: #is empty
            err_exit("POPS: stack is empty, nothing to pop",56)

        if is_var(a2,a1) is False:
            err_exit("POPS: Argument should be of type <var>",53)
        a = re.match(r'^(GF|TF|LF)@(.*)',a2)
        if a.group(1) == "GF":
            fr.GF[a.group(2)] = myStack[stackCnt-1]
        elif a.group(1) == "LF":
            fr.LF[fr.LFcnt-1][a.group(2)] = myStack[stackCnt-1]
        else: #TF
            fr.TF[a.group(2)] = myStack[stackCnt-1]
        myStack.pop()
        stackCnt -= 1

    #separated it to multiple lines so theres not 1 line with 300 characters
    elif opp.name == 'ADD' or opp.name == 'SUB' or opp.name == 'MUL' or opp.name == 'IDIV':
        if argcnt != 3:
            err_exit("Wrong number of arguments for instruction",32)        
        work_args()
    elif opp.name == 'LT' or opp.name == 'GT' or opp.name == 'EQ' or opp.name == 'AND':
        if argcnt != 3:
            err_exit("Wrong number of arguments for instruction",32) 
        work_args()
    elif opp.name == 'OR' or opp.name == 'NOT' or opp.name == 'INT2CHAR' or opp.name == 'STRI2CHAR':
        if opp.name == 'OR' or opp.name == 'STRI2INT':
            if argcnt != 3:
                err_exit("Wrong number of arguments for instruction",32)
        else: 
            if argcnt != 2:
                err_exit("Wrong number of arguments for instruction",32)
        work_args()
        
    elif opp.name == 'READ':
        if argcnt != 2:
            err_exit("Wrong number of arguments for instruction",32)
        if b1 != 'type':
            err_exit("READ: second arguments must be of type <type>",53)
        if (b2 != 'int') and (b2 != 'string') and (b2 != 'bool'):
            err_exit("READ: 2nd argument should be: (int,string,bool)",53)
        if is_var(a2,a1) is False:
            err_exit("READ: first argument is not of type var",53)

        if firstReading == True:
            if inputArg == '': #aka empty file
                temp = ['nil','nil']
            else:
                lines = inputArg.splitlines()
            firstReading = False
        if lines == []:
            temp = ['nil','nil']
        else:#read list is not empty, can be read from
            tmp = lines[0]
            lines.pop(0)
            if b2 == 'int':
                try:
                    tmp = int(tmp)
                    temp = ['int', tmp]
                except:
                    err_exit("READ: line read cant be converted to 'int'",57)
            elif b2 == 'string':
                is_symb(tmp,'string')
                tmp = str_escape_seq(tmp)
                temp = ['string',tmp]
            elif b2 == 'bool':
                # "true" -> true otherwise false
                if tmp == 'true':
                    temp = ['bool','true']
                else:
                    temp = ['bool','false']
            #else je to nil@nil?

            a = re.match(r'^(GF|TF|LF)@(.*)',a2)
            if a.group(1) == "GF":
                fr.GF[a.group(2)] = temp
            elif a.group(1) == "LF":
                fr.LF[fr.LFcnt-1][a.group(2)] = temp
            else: #TF
                fr.TF[a.group(2)] = temp

    elif opp.name == 'WRITE':
        if argcnt != 1:
            err_exit("Wrong number of arguments for instruction",32)
        if is_symb(a2,a1) is False:
            err_exit("WRITE: arg must be a symbol",32)
        if a1 == 'var':
            if is_var(a2,a1) is False:
                err_exit("WRITE: trying to reach uninicialized var",55)
            a1,a2 = gib_var_val(a2)
        if a1 == 'string':
            temp = str_escape_seq(a2)
        else:
            temp = a2
        print(temp,end='')

    elif opp.name == 'CONCAT':
        if argcnt != 3:
            err_exit("Wrong number of arguments for instruction",32)
        if (is_var(a2,a1) is False) or (is_symb(b2,b1) is False) or (is_symb(c2,c1) is False):
            err_exit("CONCAT: wrong argument types. Expected: <var> <symb> <symb>",53)
        if is_var(b2,b1):
               b1,b2 = gib_var_val(b2)
        if is_var(c2,c1):
               c1,c2 = gib_var_val(b2)
            
        if b1 == 'string' and c1 == 'string':
            a = re.match(r'^(GF|LF|TF)@(.*)',a2)
            if a.group(1) == 'GF':
                fr.GF[a.group(2)] = ['string',b2 + c2] #concatenate 2 strings
            elif a.group(1) == 'LF':
                fr.LF[fr.LFcnt-1][a.group(2)] = ['string',b2 + c2]
            else:#TF
                fr.TF[a.group(2)] = ['string',b2 + c2]
        else:
            err_exit("CONCAT: only strings can be concatenated",53)

    elif opp.name == 'STRLEN':
        if argcnt != 2:
            err_exit("Wrong number of arguments for instruction",32)
        if (is_var(a2,a1) is False) or (is_symb(b2,b1) is False):
            err_exit("STRLEN: arguments should be <var> <symb>",53)
        if is_var(b2,b1):
            b1,b2 = gib_var_val(b2)
        a = re.match(r'^(GF|LF|TF)@(.*)',a2)
        if a.group(1) == 'GF':
            fr.GF[a.group(2)] = ['int',len(b2)]
        elif a.group(1) == 'LF':
            fr.LF[fr.LFcnt-1][a.group(2)] = ['int',len(b2)]
        else:#TF
            fr.TF[a.group(2)] = ['int',len(b2)]

    elif opp.name == 'GETCHAR':
        if argcnt != 3:
            err_exit("Wrong number of arguments for instruction",32)
        if (is_var(a2,a1) is False) or (is_symb(b2,b1) is False) or (is_symb(c2,c1) is False):
            err_exit("GETCHAR: wrong argument types. Expected: <var> <symb> <symb>",53)
        if is_var(b2,b1):
            b1,b2 = gib_var_val(b2)
        if is_var(c2,c1):
            c1,c2 = gib_var_val(c2)
        if c2 < 0 or c2 > len(b2):
            err_exit("GETCHAR: index out of bounds",58)
        for c in b2:
            if c == c2:
                a = re.match(r'^(GF|LF|TF)@(.*)',a2)
                if a.group(1) == 'GF':
                    fr.GF[a.group(2)] = ['string',c]
                elif a.group(1) == 'LF':
                    fr.LF[fr.LFcnt-1][a.group(2)] = ['string',c]
                else:#TF
                    fr.TF[a.group(2)] = ['string',c]
                break


    elif opp.name == 'SETCHAR':
        if argcnt != 3:
            err_exit("Wrong number of arguments for instruction",32)
        if (is_var(a2,a1) is False) or (is_symb(b2,b1) is False) or (is_symb(c2,c1) is False):
            err_exit("SETCHAR: wrong argument types. Expected: <var> <symb> <symb>",53)
        if is_var(b2,b1):
            b1,b2 = gib_var_val(b2)
        if is_var(c2,c1):
            c1,c2 = gib_var_val(c2)
        
        #save value to be changed
        a = re.match(r'^(GF|LF|TF)@(.*)',a2)
        if a.group(1) == 'GF':
            a1,a2 = fr.GF[a.group(2)]
        elif a.group(1) == 'LF':
            a1,a2 = fr.LF[fr.LFcnt-1][a.group(2)]
        else:#TF
            a1,a2 = fr.TF[a.group(2)]

        if b1 == 'int' and c1 == 'string' and a1 == 'string':
        
            if b2 < 0 or b2 > len(temp):
                err_exit("SETCHAR: index out of bounds",58)
                i = 0
                for ah in a2:
                    if b2 == i:
                        temp += c2[0]
                else:
                    temp += ah 
                    i += 1
            else:
                err_exit("SETCHAR: arg 1 & arg 3 must be a string, arg 2 an int",53)
        if a.group(1) == 'GF':
            fr.GF[a.group(2)] = [a1,temp]
        elif a.group(1) == 'LF':
            fr.LF[fr.LFcnt-1][a.group(2)] = [a1,temp]
        else:#TF
            fr.TF[a.group(2)] = [a1,temp]


    elif opp.name == 'TYPE':
        if argcnt != 2:
            err_exit("Wrong number of arguments for instruction",32)
        if (is_var(a2,a1) is False) or (is_symb(b2,b1) is False):
            err_exit("TYPE: first arg expected <var>, second >symb>",53)
        if is_var(b2,b1):
            b1,b2 = gib_var_val(b2)
        if b1 == 'int':
            temp = ['string','int']
        elif b1 == 'string':
            temp = ['string','string']
        elif b1 == 'bool':
            temp = ['string','bool']
        elif b1 == 'nil': #nil
            temp = ['string', 'nil']
        else:
            temp = ['string', '']
        a = re.match(r'^(GF|LF|TF)@(.*)',a2)
        if a.group(1) == 'GF':
                fr.GF[a.group(2)] = temp
        elif a.group(1) == 'LF':
            fr.LF[fr.LFcnt-1][a.group(2)] = temp
        else:#TF
            fr.TF[a.group(2)] = temp


    elif opp.name == 'LABEL':  ## this is done before the cycle, so jump on
                                ## later-defined label in code is possible
        if argcnt != 1:
            err_exit("Wrong number of arguments for instruction",32) 
    elif opp.name == 'JUMP':
        if argcnt != 1:
            err_exit("Wrong number of arguments for instruction",32)
    
        if label_exist(a2) is False:
            err_exit("JUMP: trying to jump to non existing label",52)
        actLine = myLabelList[a2]

    elif opp.name == 'JUMPIFEQ':
        if argcnt != 3:
            err_exit("Wrong number of arguments for instruction",32)
        if label_exist(a2) is False:
            err_exit("JUMPIFEQ: trying to jump to non existing label",52)
        if is_symb(b2,b1) is False or is_symb(c2,c1) is False:
            err_exit("JUMPIFEQ: 2nd & 3rd argument should be both <symb>")

        if is_var(b2,b1):
            b1, b2 = gib_var_val(b2)    
        if is_var(c2,c1):
            c1, c2 = gib_var_val(c2)

        if b1 == c1: #types do match
            if b1 == 'int':
                if (int(b2) == int(c2)): #values match -> jump
                    actLine = myLabelList[a2]
            elif b1 == 'string': #both strings
                if (str(b2) == str(c2)):
                    actLine = myLabelList[a2]
            elif b1 == 'nil':
                actLine = myLabelList[a2]

        elif b1 == 'nil' or c1 == 'nil':
            pass #dont jump
        else:
            err_exit("JUMPIFEQ: non matching types of operands(nor one of them nil)",53)

    elif opp.name == 'JUMPIFNEQ':
        if argcnt != 3:
            err_exit("Wrong number of arguments for instruction",32)
        if label_exist(a2) is False:
            err_exit("JUMPIFNEQ: trying to jump to non existing label",52)
        if is_symb(b2,b1) is False or is_symb(c2,c1) is False:
            err_exit("JUMPIFNEQ: 2nd & 3rd argument should be both <symb>")
        
        if is_var(b2,b1):
            b1, b2 = gib_var_val(b2)    
        if is_var(c2,c1):
            c1, c2 = gib_var_val(c2)
        if b1 == c1: #stejne typy
            if b1 == 'int':
                if int(b1) == int(c1): #jump if they are not equal
                    pass
                else:
                    actLine = myLabelList[a2]
                    
        elif b1 == 'nil' or c1 == 'nil':
            actLine = myLabelList[a2]
        else:
            err_exit("JUMPIFNEQ: non matching types of operands(nor one of them nil)",53)

    elif opp.name == 'EXIT':
        if argcnt != 1:
            err_exit("Wrong number of arguments for instruction",32)
        if a1 == 'int':
            try:
                temp = int(a2)
            except:
                err_exit("EXIT: argument value is not integer",57)

            if temp >= 0 and temp <= 49:
                sys.exit(temp)
            else:
                err_exit("EXIT: int has to be in interval <0,49>",57)
        else:
            err_exit("EXIT: argument should be of type <int>",53)
    elif opp.name == 'DPRINT':
        if argcnt != 1:
            err_exit("Wrong number of arguments for instruction",32)
        if is_symb(a2,a1) is False:
            err_exit("DPRINT: argument expected type is <symb>",53)
        if is_var(a2,a1):
            a1,a2 = gib_var_val(a2)
        sys.stderr.write(a2)
    else:
        err_exit("Unknown instruction",32)
