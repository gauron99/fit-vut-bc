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

#prints out given message and exits with desired code
def err_exit(string,rc):
    print ("Error: ",string,"- exiting w/",rc)
    sys.exit(rc)

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

def work_args():
    print('~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nis comaptible:\n')
    print(' name: ',opp.name)
    print(' arg1(m): ',opp.arg1)
    print(' arg2(n): ',opp.arg2)
    print(' arg3(o): ',opp.arg3)
    for m1,m2 in opp.arg1.items():
        for n1,n2 in opp.arg2.items():
            if opp.arg3 is not None: #if only 2 arguments are needed (NOT & INT2CHAR)
                for o1,o2 in opp.arg3.items():
                    break
            break
        break
    
    # if (is_var(m2,m1) or is_symb(n2,n1)) is False:
        # err_exit('''Unsupported types of arguments for arithmetic/relational/boolean 
        # or conversion instructions. Expected '<var> <symb> <symb>' (NOT & INT2CHAR excluded)''') 
    
    m = re.match(r'^(GF|LF|TF)@(.*)',m2)

    #operations take only 2args (as ooposed to all other arithmetic, relational, boolean or conversion instructions)   
    if opp.name == 'NOT': #only 2 operands
        pass
    elif opp.name == 'INT2CHAR': #only 2 operands
        pass

    
    #operations all take 3 arguments <var> <symb1> <symb2>
    if is_symb(o2,o1) is False: #first two tested few lines up
        err_exit('''Unsupported types of arguments for arithmetic/relational/boolean 
        or conversion instructions. Expected '<var> <symb> <symb>' (NOT & INT2CHAR excluded)''') 
    
    if opp.name == 'ADD':
        print('m is var ',is_var(m2,m1))
        print('n is var ',is_var(n2,n1))
        print('o is var ',is_var(o2,o1))
        #check if its var, otherwise its symbol
        if is_var(n2,n1) and is_var(o2,o1): #both vars
            
            n = re.match(r'^(GF|LF|TF)@(.*)',n2)
            o = re.match(r'^(GF|LF|TF)@(.*)',o2)
            tt1,tv1 = gib_var_val(n2) #tt = temp type; tv = temp val
            tt2,tv2 = gib_var_val(o2)
            print('eccececec',tt1,tt2)
            if tt1 != 'int' or tt2 != 'int':
                err_exit("ADD: 2nd & 3rd argument must be of type int",53)

            if m.group(1) == "GF":
                fr.GF[m.group(2)] = ['int',tv1 + tv2]
            elif m.group(1) == "LF":
                fr.LF[fr.LFcnt-1][m.group(2)] = ['int',tv1 + tv2]
            else:#TF
                fr.TF[m.group(2)] = ['int',tv1 + tv2]
        elif is_var(n2,n1): #only second argument is var
            n = re.match(r'^(GF|LF|TF)@(.*)',n2)

            tt1,tv1 = gib_var_val(n2) #tt = temp type; tv = temp val
            if tt1 != 'int' or o1 != 'int':
                err_exit("ADD: 2nd & 3rd argument must be of type int",53)

            if m.group(1) == "GF":
                fr.GF[m.group(2)] = ['int',tv1 + int(o2)]
            elif m.group(1) == "LF":
                fr.LF[fr.LFcnt-1][m.group(2)] = ['int',tv1 + int(o2)]
            else:#TF
                fr.TF[m.group(2)] = ['int',tv1 + int(o2)]
        elif is_var(o2,o1):
            n = re.match(r'^(GF|LF|TF)@(.*)',o2)

            tt1,tv1 = gib_var_val(o2) #tt = temp type; tv = temp val
            if tt1 != 'int' or n1 != 'int':
                err_exit("ADD: 2nd & 3rd argument must be of type int",53)

            if m.group(1) == "GF":
                fr.GF[m.group(2)] = ['int',tv1 + int(n2)]
            elif m.group(1) == "LF":
                fr.LF[fr.LFcnt-1][m.group(2)] = ['int',tv1 + int(n2)]
            else:#TF
                fr.TF[m.group(2)] = ['int',tv1 + int(n2)]

        elif (n1 != 'int' or o1 != 'int'):
            err_exit("ADD: both symbols must be of type int",53)


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
    elif opp.name == 'STRI2INT':
        pass
    else:
        err_exit('''Unknown error. Unsupported instruction in 'work_args()'. Function for arithmetic, relational,
         boolean and conversion instructions''',99)
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


def label_exist(label):
    for i in myLabelList:
        if label == i:
            return True
    return False

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
        return False
        # print("Note: possibly used <symb> instead of <var> or wrong frame")
        # err_exit("Variable couldnt be verified",52)
        
def is_symb(string,sType):
    if sType == 'var':#has frame, check var
        if is_var(string,sType):
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



## ~~~~~~~~~~~~~~~~~~~~~~~~ frames & labels & stack ~~~~~~~~~~~~~~~~~~~~~~~~ ##
myLabelList = {} #dictionary of labels & their instr # {"label": <number>, ...}
myCallList = [] #list of positions
myStack = [] #for stack instructions
stackCnt = 0 #how many 'things' are in stack

#inicialized GF,LF,TF,LFcnt; fr = Frames class
fr = Frames({},None,None,0)

#note: to access lists(LF,labels,calls,stack, index of last item is 'counter - 1')


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
                --input=<file>; where <file> is a file with input fo~~~~~~~~~~~~~~~~~ ##
myLabelList = {} #dictionary of labels & their instr # {"label": <number>, ...}
myCallList = [] #list of positions
myStack = [] #for stack instructions
stackCnt = 0 #how many 'things' are in stack

#inicialized GF,LF,TF,LFcnt; fr = Frames class
fr = Frames({},None,None,0)

r interpret itself
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

print ('~~~~~~~~~~Debug time~~~~~~~~~~\n')##################################################################################

# print('Labels: ',myLabelList)
# i = 0

# print ('All instructions:')
# while i < len(instr):
#     i += 1
    
#     print('Inst',i,': ',instr[i-1].attrib, ", ",end='')
#     for c in instr[i-1]:
#         print(c.text," ",end='')
#     print('')

# # fr.GF['maggie'] = 4
# fr.GF['bitch'] = ['int',5]
# # fr.GF['fucker'] = 4
# fr.GF['sabina'] = ['int',4]
# g1 = "GF@maggie"
# g2 = "GF@bitch"
# g4 = "GF@fucker"
# g3 = "GF@sabina"


#working with LF as a list of directories
# fr.LFcnt = 0
# fr.LF = [{}] #inicialize list
# fr.LFcnt += 1

# print('\nLF: ',fr.LF)
# fr.LF[fr.LFcnt-1]['boi'] = ['int',5]
# print(fr.LF)
# fr.LF.append({})#add new dictionary
# fr.LFcnt += 1
# print(fr.LF)
# fr.LF[fr.LFcnt-1]['a'] = ['str','ahoj']
# print(fr.LF)
# fr.LF.pop(fr.LFcnt-1)
# fr.LFcnt -= 1
# print(fr.LF)
# fr.LF[fr.LFcnt-1]['b'] = ['bool','True']

# print(fr.LF)
# exit(0)

# fr.LFcnt = 0
# fr.LF = [{}]
# fr.LFcnt += 1
# fr.LF[fr.LFcnt-1]['bob']=['int',4]
# # fr.LF['ivstypek']= 2
# # fr.LF['whodis']= ['int',1]
# l1 = "LF@bob"
# l2 = "LF@ivstypek"
# l3 = "LF@whodis"

# fr.TF = {}
# fr.TF['Myman']= ['string','text']
# # fr.TF['badboi']= 5
# # fr.TF['jerry']= 5
# t1 ="TF@Myman"
# t2 ="TF@badboi"
# t3 ="TF@jerry"

# d = []
# d.append(g1)
# d.append(g2)
# d.append(g3)
# d.append(g4)

# d.append(l1)
# d.append(l2)
# d.append(l3)

# d.append(t1)
# d.append(t2)
# d.append(t3)

# for da in d:
#     y = is_var(da)
#     if y:
#         print ('God: ye its true\n')
#     else:
#         print ('God: aint gonn happen\n')
# s = 'int@+5'
# boo = is_symb(s,typeyho)
# if boo:
#     print ('NICE its cislo')
# else:
#     print ('damn what a shame no cislo')#

#int
# b2 = True
# a1 = 'GF@bitch'

# #string
# b2 = 'DOPICEUZ'
# # a1 = 'LF@bob'

# a = re.match(r'(.*)@(.*)',a1)
# if a.group(1) == "GF":
#     for arg in fr.GF:
#         if arg == a.group(2):
#             fr.GF[arg] = b2
#             break
# elif a.group(1) == "LF":
#     for arg in fr.LF:
#         if arg == a.group(2):
#             fr.LF[arg] = b2
#             break
# else: #a.group(1) == "TF" 
#     for arg in fr.TF:
#         if arg == a.group(2):
#             fr.TF[arg] = b2
#             break

# print ('\nGF <arg1 type="var">GF@a</arg1>: ',fr.GF)
# fr.GF['sabina'][1] = 10
# fr.GF['bitch'][1] = 10

# isinstance(fr.GF['bitch'],str)
# print('bitch_is_bool: ', isinstance(fr.GF['bitch'],int))
# print('sabina_is_int: ', isinstance(fr.GF['sabina'],int))
# print('Myman_is_str: ', isinstance(fr.TF['Myman'],str))

##debug print of all frames
# print ('\nAll frames:')
# print ('GF: ',fr.GF)
# print ('\nLF: ',fr.LF)
# print ('\nTF: ',fr.TF)


# for a1, a2 in opp.arg1.items():
#     break

# value = gib_var(a2)
# print('value: ',value)
# print(value)

# x = re.match(r'^(GF|LF|TF)@(.*)',string)
# if x.group(1) == "GF":
#     for key in fr.GF:
#         if x.group(2) == key:
#             return True
#     return False
# elif x.group(1) == "LF":
#     for key in fr.LF[fr.LFcnt-1]:
#         if x.group(2) == key:
#             return True
#     return False
# else:
#     for key in fr.TF:
#         if x.group(2) == key:
#             return True
#     return False

################################################################################

################################################################################
print('\n~~~~~~~~~~Debug time over~~~~~~~~~~\n') ##############################################################################
##debug print of all frames
# print ('\nAll frames:')
# print ('GF: ',fr.GF)
# print ('LF: ',fr.LF)
# print ('TF: ',fr.TF)
# print ('\nStack:')
# print (myStack)
# exit(0)

## ~~~~~~~~~~~~~~~ main cycle ~~~~~~~~~~~~~~~ ##

actLine = 0

while actLine < len(instr):
    actLine += 1
    print(actLine)
    child = instr[actLine - 1]
    
    opp = Opperation(child.attrib['opcode'].upper(),None,None,None)
    # opp.name = current opperation name (ex: LABEL)

    argcnt = 0
    for sub in child:
        argcnt +=1
        if argcnt == 1:
            opp.arg1 = {}
            opp.arg1[sub.attrib['type']] = sub.text 
        elif argcnt == 2:
            opp.arg2 = {}
            opp.arg2[sub.attrib['type']] = sub.text 
        else:
            opp.arg3 = {}
            opp.arg3[sub.attrib['type']] = sub.text

    print('opp:',opp.name,'  ', end='')
    print ('arg1:',opp.arg1,'  ',end='') 
    print ('arg2:',opp.arg2,'  ',end='') 
    print ('arg3:',opp.arg3,'  \n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n') 

#main SWITCH-like part        
    if opp.name == 'CREATEFRAME':
            fr.TF = {} #easy enough

    elif opp.name == 'PUSHFRAME': #TODO
        if fr.TF == None:
            err_exit("Trying to push TF to LF buffer, but TF is undefined",55)
        if fr.LFcnt == 0: #first one
            fr.LF = [{}] #inicialize list
            fr.LF[fr.LFcnt] = fr.TF
        else:
            fr.LF.append(fr.TF)
        fr.TF = None
        fr.LFcnt += 1

    elif opp.name == 'POPFRAME':#TODO
        if fr.LF == []:
            err_exit("Trying to pop LF, but none is available",55)
        fr.TF = fr.LF[fr.LFcnt-1]
        fr.LF.pop(fr.LFcnt-1)
        fr.LFcnt -= 1

    elif opp.name == 'RETURN':
        if myCallList == None:
            err_exit("RETURN: Trying to ret, but no 'calls' were made",56)
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
        for a1, a2 in opp.arg1.items():
            break
        for b1, b2 in opp.arg2.items():
            break
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
                    print(arg,b2)#debug
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
        for a1, a2 in opp.arg1.items():
            break
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
        for a1,a2 in opp.arg1.items():
            if label_exist(a2) is False:
                err_exit("CALL: Label refered to by CALL doesn't exist",56)
            myCallList.append(actLine)
            actLine = myLabelList[a2]
        # else: #if no iteration of for is made, do else (smart snake)
            # err_exit("CALL: requires 1 argument (label)",32) ?

    elif opp.name == 'PUSHS': #symb
        for a1,a2 in opp.arg1.items():
            if is_symb(a2,a1) is False:
                err_exit("PUSHS: argument is not <symb>",53)
            myStack.append([a1,a2])
            stackCnt += 1
            
    elif opp.name == 'POPS': #var
        if myStack == []: #is empty
            err_exit("POPS: stack is empty, nothing to pop",56)
        for a1,a2 in opp.arg1.items():
            break
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

    elif opp.name == 'ADD':
        for a1,a2 in opp.arg1.items():
            for b1,b2 in opp.arg2.items():
                for c1,c2 in opp.arg3.items():
                    break
                break
            break
        print(is_var(a2,a1),is_symb(b2,b1),is_symb(c2,c1))
        if (is_var(a2,a1) or is_symb(b2,b1) or is_symb(c2,c1)) is False:
            err_exit("ADD: arguments don't match type. Expected: <var> <symb1> <symb2>",53)
        work_args()

    elif opp.name == 'SUB':
        pass
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
    #         work_args("read",subChild)
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
        #sys.exit(0) 
    # break
#end while (main cycle)
#end of function progr


##debug print of all frames
print ('\nAll frames:')
print ('GF: ',fr.GF)
print ('LF: ',fr.LF)
print ('TF: ',fr.TF)
print ('\nStack:')
print (myStack)

# CREATEFRAME done
# PUSHFRAME done
# POPFRAME done
# RETURN ?
# BREAK ?
# MOVE done
# DEFVAR done
# CALL done
# PUSHS done
# POPS done
# ADD
# SUB
# MUL
# IDIV
# LT
# GT
# EQ


