<?php
#navratove kody pro analyzator(parse.php): 21,22,23 

$xw = xmlwriter_open_memory();
xmlwriter_set_indent($xw, 1);
$res = xmlwriter_set_indent_string($xw, '   ');

#regexes to trim the line of whitespaces at the start of a line; comments; whitespaces at the end of a line
$reg_ws_start = '/^[[:blank:]]+/';
$reg_del_comms = '/#.*/';
$reg_ws_end = '/[[:blank:]]+$/';

#match label
$reg_match_label = '/^[_\-$\%&\*\!?a-zA-Z]\w*$/';
#match var
$reg_match_var = '/^[G|L|T]F@[_\-$\%&\*\!?a-zA-Z]\w*$/';
#match symb
$reg_match_symb = '/^[G|L|T]F@[_\-$\%&\*\!?a-zA-Z]\w*$|^int@[+-][0-9]+$|^int@[0-9]*$|^string@([^\\#\s]|\\\d{3})*$|^bool@(false|true)$|^nil@nil$/';
#match type
$reg_match_type = '/^(int|string|bool)$/';

#help message
if($argc == 2){
    
    if($argv[1] == '--help' || $argv[1] == '-h'){
        fputs(STDERR, "argument is --help(-h)\n");
        echo ("- Use '<file' to input a file containing your ippcode20 code
- use '>file' to also output the xml representation into a file
- use'--help' as first and only argument to print out help message\n");
        exit(0);
    }
    else{
        fputs(STDERR, "Wrong argument given, try '--help'\n");
        exit(10);
    }
}

else if($argc > 2){
    fputs(STDERR, "Wrong number of arguments given, try '--help'\n");
    exit(10);
}

$firstLineRead = false;
$line_cnt = 0;
$inst_cnt = 0;

#array for arguments and their types to pass to a "write xml output" function
$args = array();
$argtypes = array();

#main cycle; line counter (starts with 1)
while($line = fgets(STDIN)){
    $line_cnt = $line_cnt + 1;
    
    #ignore whitespace (tabs/spaces) at the start of a line
    $line = preg_replace($reg_ws_start,"",$line);
    #ignore comments (delete)
    $line = preg_replace($reg_del_comms,"",$line);
    #ignore whitespace at the end of line
    $line = preg_replace($reg_ws_end,"",$line);
    #skip line -- empty
    if($line == "\n"){
        continue;
    }

    #.ippcode20 must be first line (case-insensitive)
    if($firstLineRead == false){
        if(strtolower($line) !== ".ippcode20\n"){
            fputs(STDERR,"Error 21, first line should be .IPPcode20\n");
            exit(21);
        }
        else{
            $firstLineRead = true;
            #outputs beginnning of the program
            xmlwriter_start_document($xw,'1.0','UTF-8');
            xmlwriter_start_element($xw,'program'); #for ending this element look at the end of the program
            xmlwriter_start_attribute($xw, 'language');
            xmlwriter_text($xw,'IPPcode20');
            xmlwriter_end_attribute($xw);
            continue;
        }
    }


    $line = preg_replace('!\s+!', ' ', $line);
    $words = explode(" ",trim($line));
    
    switch (strtolower($words[0])) {

        #empty
        case 'createframe':
        case 'popframe':
        case 'pushframe':
        case 'return':
        case 'break':
            if(count($words) != 1){
                fputs(STDERR,"Error, too many arguments for CREATE/POP/PUSH-FRAME\n");
                prnt_err_msg();
                exit(23);
            }
            break;

        #⟨label⟩ ⟨symb1⟩ ⟨symb2⟩
        case 'jumpifeq':
        case 'jumpifneq':
            if(count($words) != 4){prnt_err_msg();exit(23);}
            if(!preg_match($reg_match_label,$words[1],$matches)){prnt_err_msg();exit(23);} #match label
            $args[0] = $matches[0];
            $argtypes[0] = 'label';
            if(!preg_match($reg_match_symb,$words[2],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[1] = $matches[0];
            $argtypes[1] = 'symb';
            if(!preg_match($reg_match_symb,$words[3],$matches)){prnt_err_msg();exit(23);} #match symb2
            $args[2] = $matches[0];
            $argtypes[2] = 'symb';
            break;

        #⟨var⟩ ⟨symb1⟩ ⟨symb2⟩
        case 'sub':
        case 'mul':
        case 'idiv':
        case 'lt':
        case 'gt':
        case 'eq':
        case 'and':
        case 'or':
        case 'stri2int':
        case 'concat':
        case 'getchar':
        case 'setchar':
            if(count($words) != 4){prnt_err_msg();exit(23);}
            if(!preg_match($reg_match_var,$words[1],$matches)){prnt_err_msg();exit(23);} #match var
            $args[0] = $matches[0];
            $argtypes[0] = 'var';
            if(!preg_match($reg_match_symb,$words[2],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[1] = $matches[0];
            $argtypes[1] = 'symb'; #(symb == int,bool,string,nil)
            if(!preg_match($reg_match_symb,$words[3],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[2] = $matches[0];
            $argtypes[2] = 'symb';
        break;
        
        #⟨var⟩
        case 'defvar':
        case 'pops':
            if(count($words) != 2){prnt_err_msg();exit(23);}
            if(!preg_match($reg_match_var,$words[1],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[0] = $matches[0];
            $argtypes[0] = 'var';
        break;

        #⟨var⟩ ⟨symb⟩
        case 'move':
        case 'int2char':
        case 'strlen':
        case 'type':
        case 'not':
            if(count($words) != 3){prnt_err_msg();exit(23);}
            if(!preg_match($reg_match_var,$words[1],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[0] = $matches[0];
            $argtypes[0] = 'var';
            if(!preg_match($reg_match_symb,$words[2],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[1] = $matches[0];
            $argtypes[1] = 'symb';
        break;

        #⟨var⟩ ⟨type⟩
        case 'read':
            if(count($words) != 3){prnt_err_msg();exit(23);}
            if(!preg_match($reg_match_var,$words[1],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[0] = $matches[0];
            $argtypes[0] = 'var';
            if(!preg_match($reg_match_type,$words[2],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[1] = $matches[0];
            $argtypes[1] = 'type';
            break;

        # ⟨label⟩
        case 'label':
        case 'jump':
        case 'call':
           
            if(count($words) != 2){prnt_err_msg();exit(23);}
            if(!preg_match($reg_match_label,$words[1],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[0] = $matches[0];
            $argtypes[0] = 'label';
            break;

        # symb
        case 'pushs':
        case 'write':
        case 'exit':
        case 'dprint':
            if(count($words) != 2){prnt_err_msg();exit(23);}
            if(!preg_match($reg_match_symb,$words[1],$matches)){prnt_err_msg();exit(23);} #match symb1
            $args[0] = $matches[0];
            $argtypes[0] = 'symb';
            break;

        default:
            fputs(STDERR,"Error 22,unknown operation\n");
            exit(22);
            break;

        }
        $inst_cnt++;
        #arguments $words, $xw, $inst_cnt, arguments, their types
        write_inst($words, $xw, $inst_cnt,$args,$argtypes);
        $args = array(); # to re-inicialize variables, aka to empty them
        $argtypes = array();

}

xmlwriter_end_element($xw);
echo xmlwriter_output_memory($xw);
// fputs (STDERR,"End of program, exit 0\n");
exit(0);


function prnt_err_msg(){ #prints out message that a lexical/syntax error has occured
    fputs(STDERR,"Lexical/syntax error exit(23) \n");
    return;
    }

    #takes arguments line devided into words, xwriter variable, instruction counter
function write_inst($words, $xw, $inst_cnt, $args, $argtypes){

    xmlwriter_start_element($xw,'instruction');
    xmlwriter_start_attribute($xw,'order');
    xmlwriter_text($xw,$inst_cnt);
    xmlwriter_start_attribute($xw,'opcode');
    xmlwriter_text($xw,$words[0]);
    $arg_cnt = 1;
    
    if($args){
        for($i=0;count($args) > $i;$i++){
            if($argtypes[$i] === 'symb'){#int,string,bool,nil,var
                if(preg_match('/^int@/',$args[$i])){
                    $arg_type_wr = 'int';
                }
                elseif(preg_match('/^string@/',$args[$i])){
                    $arg_type_wr = 'string';
                }
                elseif(preg_match('/^bool@/',$args[$i])){
                    $arg_type_wr = 'bool';
                }
                elseif(preg_match('/^nil@/',$args[$i])){
                    $arg_type_wr = 'nil';
                }
                else{#if nothing was matched, its variable starting with (GF@/TF@/LF@)
                    $arg_type_wr = 'var';
                }

            }
            else{$arg_type_wr = $argtypes[$i];}

            
                #argtype -> int,bool,string,nil,label,type,var
            xmlwriter_start_element($xw,'arg'.$arg_cnt++);
            xmlwriter_start_attribute($xw,'type');
            xmlwriter_text($xw,$arg_type_wr);
            xmlwriter_end_attribute($xw);
            if($arg_type_wr == 'bool'|| $arg_type_wr == 'int' || $arg_type_wr == 'string' || $arg_type_wr == 'nil'){
               if($arg_type_wr == 'string'){
                $arg_new = substr($args[$i],7);
                xmlwriter_text($xw,$arg_new);
                }

               else{
                $arg_new = explode("@",$args[$i]);
                xmlwriter_text($xw,$arg_new[1]);
                }
            }
            else{
                xmlwriter_text($xw,$args[$i]);
            }
            xmlwriter_end_element($xw);
        }
    }
    xmlwriter_end_attribute($xw);
    xmlwriter_end_attribute($xw);
    xmlwriter_end_element($xw);

}

?>
