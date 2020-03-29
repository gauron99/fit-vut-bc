<?php
                ###~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~###
                ###     autor: Fridrich David               ###             
                ###     zadani: IPP20 tester                ### 
                ###     datum posledni upravy: 29.03.2020   ###
                ###~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~###



$files_raw = array();


#variables signaling what parameters were given (bool)
#note: int-only cant be combined with parse-only or parse-script!

$_directory = false;
$_recursive = false;
$_parse_script = false;
// $_int_script = false;   #--interpret only
$_parse_only = false;
// $_int_only = false;     #--interpret only
// $_jexamxml = false;

#complete path to file
$parse_file;
// $int_file;

#writes message provided specificaly by a calling function
#about the error and exits with the code
function err_exit($err_code,$err_reason){
    fputs(STDERR, "Error occured: ".$err_reason."; exiting w/ code(".$err_code.")\n");
    exit($err_code);
}

// testing /home/donnovan/skola/druhy/letni/IPP/


#print help message // parse the arguments; set variables beggining with '$_'
echo ("\n##~~~~~~~~~~~~ arg list ~~~~~~~~~~~~##\n\n"); #debug
foreach(array_slice($argv,1) as $prgArg) { //array_slice >> dont include name of the script
    echo ("given: ".$prgArg."\n"); #debug
    
    # --help
    if($prgArg == '--help'){
        if ($argc == 2) {
        #echo print help message for test
        }
        else{
            err_exit(10,"Wrong arguments given, try '--help'");
        }
    }
    # --directory=file
    elseif (preg_match('/^--directory=/',$prgArg)) {
        $_directory = true;
        echo("dir tho\n"); #debug
        $tmp = explode("=",$prgArg);
        $directory = $tmp[1];
        if (!is_dir($directory)) {
            err_exit(11,"Path given to '--directory' is invalid");
        }
        else{
            echo ("hey baby the dir iz goood\n"); #debug
        }
    }


    #--recursive
    elseif(preg_match('/^--recursive$/',$prgArg)){
        $_recursive = true;
    }
    #--parse_script=file
    elseif(preg_match('/^--parse_script=/',$prgArg)){

        echo("EY BOI parse_script active\n");#debug

        $_parse_script = true;
        $tmp = explode("=",$prgArg);
        $parse_file = $tmp[1];
        // echo ($parse_file);
        if(preg_match('/parse.php$/',$parse_file)){

            if(file_exists($parse_file)); #absolute path found
            elseif(file_exists(getcwd() . $parse_file)){ #relative path found
                
                // possible TODO maybe get rid of ../ lines to make it cleaner
                $parse_file = getcwd() . $parse_file; #transform to absolute
            }
            else{ #file was not found using either absolute or relative path
                err_exit(11, "File 'parse.php' in --parse_script not found");
            }
        }
        else{
            err_exit(11, "File provided by --parse_script is invalid");
        }
    }

    #int_script=file
    // elseif(pregmatch('/^--int_script=/', $prgArg)){

    // }

    #--parse-only
    elseif(preg_match('/^--parse-only$/',$prgArg)){
        $_parse_only = true;
    }
    
    #--int_only
    // elseif(preg_match('/^--int_only$/',$prgArg)){

    // }
    
    else{
        echo ("Not yet implemented or you fucked up big time(parsing)\n");#debug
        exit(0);
    }

} #end of parsing arguments
echo ("\n##~~~~~~~~~~~~ arg list end ~~~~~~~~~~~~##\n\n"); #debug

##~~~~~~~~~~~~~~~ additional checking ~~~~~~~~~~~~~~~##

# checking for script files if needed
if($_parse_script == false){ #cwd
    if(!file_exists(getcwd()."/parse.php")){
        err_exit(11); #path was not provided;file wasnt found in cwd
    }
    $parse_file = getcwd()."/parse.php"; #MUST BE CHANGED TO PHP7.4 TO RUN ON MERLIN
}

// $parse_script ="php " . getcwd() . "/" . "external_file.php "; # MUST BE CHANGED TO PHP7.4 TO RUN ON MERLIN 
// $blabla = shell_exec(($parse_script));
// echo($blabla);


##~~~~~~~~~~~~~~~ working with test dirs ~~~~~~~~~~~~~~~##

if(!$_directory){
    $directory = getcwd(); #if directory is not given, set to cwd
}

// $file_dirs = array();
function getFiles($dir){ # $dir = path given by '--directory=' or cwd
    $home_dir = $dir; #parent dir saved
    global $files_raw;
    foreach (scandir($dir) as $file) {
        if($_recursive == true){
            if(is_dir($file)){
                #if file scanned is directory, call function recursively
                #and search for .src files in all subdirs
                getFiles($home_dir."/".$file);
            }
        }
        if ($file !== '.' && $file !== '..') {
            $files_raw[] = $file; #fill up $files_raw[] with all files found 
        }
    }   //CONTINUE --FIND .src files, save to $files (add global)
        //TEST IF WORKS PROPERLY
}















// $directory = '/home/donnovan/skola/druhy/letni/IPP/test_parse_only_for_now';
// echo($directory);

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