<?php
                ###~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~###
                ###     autor: Fridrich David               ###             
                ###     zadani: IPP20 tester                ### 
                ###     datum posledni upravy: 11.03.2020   ###
                ###~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~###


#variables signaling what parameters were given (if empty, none given)
#   "##" sign means that these parameters take additional arguments (like path)
#note: int-only cant be combined with parse-only or parse-script!
// $$directory;
// $$recursive;##
// $$parse_script;
// $$int_script;   #--interpret only
// $$parse_only;##
// $$int_only;##   #--interpret only
// $$jexamxml;

#writes message about the error and exits with the code given as a parameter
function err_exit($err_code){
    fputs(STDERR, "Error occured while processing given arguments, try '--help'\n");
    
    
    
    exit($err_code);
}



#print help message
echo ("\n##~~~~~~~~~~~~ arg list ~~~~~~~~~~~~##\n\n");
foreach ($argv as $prgArg) {
    echo ("given: ".$prgArg."\n");
    if($prgArg == '--help'){
        if ($argc == 2) {
        #echo print help message for test
        }
        else{
            err_exit(10);
        }
    }

    elseif (preg_match('/^--directory=/',$prgArg)) {
        echo("dir tho\n");
    }



}



















// $directory = '/home/donnovan/skola/druhy/letni/IPP/test_parse_only_for_now';
// // echo($directory);

// if (!is_dir($directory)) {
//     exit('Invalid diretory path');
// }

// $file_dirs = array();
// $files = array();
// foreach (scandir($directory) as $file) {
//     if ($file !== '.' && $file !== '..') {
//         $file_dirs[] = $file;
//     }
// }  ##file_dirs je array vsech slozek kterej sou uvnitr slozky "/test_parse_only_for_now"

// // var_dump($file_dirs);

// for($i=0;$i<15;$i++){
//     foreach (scandir($directory."/".$file_dirs[$i]) as $file) {
//         if(strpos($directory."/".$file_dirs[$i],"GENERATE")){
//             continue;
//         }
//         // echo($file."\n");
//         if(preg_match("/\.src$/",$file)){
//             if ($file !== '.' && $file !== '..') {
//                 $files[] = $file;
                
//             }
//         }
//     }
// }


// foreach($files as $output){
//     echo($output."\n");
// }


##~~~~~~~~~~~##
// $i = 0;
// foreach ($files as $one_file) {
//     $i++;
//     echo($i."__".$file);
// }


?>