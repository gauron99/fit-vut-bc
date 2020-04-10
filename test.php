<?php
                ###~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~###
                ###     autor: Fridrich David               ###             
                ###     zadani: IPP20 tester                ### 
                ###     datum posledni upravy: 29.03.2020   ###
                ###~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~###


#globals
$files = array();
$rc_files = array();
$out_files = array();



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

foreach(array_slice($argv,1) as $prgArg) { //array_slice >> dont include name of the script
    // echo ("given: ".$prgArg."\n"); #debug
    
    # --help
    if($prgArg == '--help'){
        if ($argc == 2) {
        #echo print help message for test
        }
        else{
            err_exit(10,"Try '--help' as the only argument");
        }
    }
    # --directory=file
    elseif (preg_match('/^--directory=/',$prgArg)) {
        $_directory = true;
        // echo("dir tho\n"); #debug
        $tmp = explode("=",$prgArg);
        $directory = $tmp[1];
        if (!is_dir($directory)) {
            err_exit(11,"Path given to '--directory' is invalid");
        }
        else{
            // echo ("hey baby the dir iz goood\n"); #debug
        }
    }


    #--recursive
    elseif(preg_match('/^--recursive$/',$prgArg)){
        $_recursive = true;
    }
    #--parse_script=file
    elseif(preg_match('/^--parse_script=/',$prgArg)){

        // echo("EY BOI parse_script active\n");#debug

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
        // echo ("Not yet implemented or you fucked up big time(parsing)\n");#debug
        exit(0);
    }

} #end of parsing arguments
// echo ("\n##~~~~~~~~~~~~ arg list end ~~~~~~~~~~~~##\n\n"); #debug

##~~~~~~~~~~~~~~~ additional checking ~~~~~~~~~~~~~~~##
# checking for script files if needed

if($_parse_script == false){ #cwd
    if(!file_exists(getcwd()."/parse.php")){
        err_exit(11, "File 'parse.php not found"); #path was not provided;file wasnt found in cwd
    }
    $parse_file = getcwd()."/parse.php";
}

// $parse_script ="php " . getcwd() . "/" . "external_file.php "; # MUST BE CHANGED TO PHP7.4 TO RUN ON MERLIN 
// $blabla = shell_exec(($parse_script));
// echo($blabla);

##~~~~~~~~~~~~~~~ working with test dirs ~~~~~~~~~~~~~~~##

if(!$_directory){
    $directory = getcwd(); #if directory is not given, set to cwd
}

$base_home_dir = $directory;
getFiles($directory);

// loads $files[] with .src files from dir/&subdirs
function getFiles($dir){ # $dir = path given by '--directory=' or cwd
    
    $home_dir = $dir; #parent dir saved
    
    global $files;
    global $_recursive;
    global $_parse_only;
    global $rc_files;
    global $out_files;
    
    // (preg_grep('/^([^.])/',$file)) #~~ ignore hidden files
    foreach (preg_grep('/^([^.])/', scandir($home_dir)) as $file) {
        
        if($_recursive){
            $dir = $home_dir."/".$file; #has to be set so subdirs are recognized as dirs
            if(is_dir($dir)){ #if file is directory, call function recursively
                if ($file !== '.' && $file !== '..'){
                getFiles($home_dir."/".$file);
                }
            continue; #~~ dont save dirs to $files
            }
        }
        if(is_dir($file)){ //if recursive is not set, dont include dirs
            continue;
        }

        if ($file !== '.' && $file !== '..'){
            if(preg_match('/(.*).src$/',$file,$matches)){
                // echo("FILEFILE ".$home_dir."/".$file."\n");
                $files[] = $home_dir."/".$file; #save files with absolute path
                
                #if set, look for .rc & .out files
                if($_parse_only){ #TODO omezeni jen pro parse-only oddelat, protoze vzdycky budou potreba?
                    // preg_match('/(.*).src$/',$file,$matches);

                    #cycle to find .out & .rc files if they exist

                  
                    $rc_exists = false;
                    $out_exists = false;
                    // echo("matches: ".$matches[1]."\n");
                    foreach(scandir($home_dir) as $adj_file){
                        
                        if(strpos($adj_file,$matches[1]) !== false){                            
                            if($adj_file === $matches[1].".rc"){
                                // echo("rc exists\n");
                                $rc_exists = true;
                                $rc_files[] = $home_dir."/".$adj_file;
                            }
                            elseif($adj_file === $matches[1].".out"){
                                // echo("out exists\n");
                                $out_exists = true;
                                $out_files[] = $home_dir."/".$adj_file;
                            }
                        }
                    }#end of foreach
                    
                    if($rc_exists === false){
                        // echo("rc created\n");
                        // echo("i wanna add ".$home_dir."/".$matches[1].".rc\n");
                        file_put_contents($home_dir."/".$matches[1].".rc","0");
                        $rc_files[] = $home_dir."/".$matches[1].".rc";
                    }
                    if($out_exists === false){
                        // echo("out created\n");
                        // echo("i wanna add ".$home_dir."/".$matches[1].".out\n");
                        file_put_contents($home_dir."/".$matches[1].".out",NULL);
                        $out_files[] = $home_dir."/".$matches[1].".out";
                    }
                }
            }
        }
    }
    // var_dump($files); #debug
}

// echo "~~~~~files~~~~~\n";
// var_dump($files);
// echo "~~~~~out_files~~~~~\n";
// var_dump($out_files);
// echo "~~~~~rc_files~~~~~\n";
// var_dump($rc_files);
##~~~~~~~~~~~~~~~ running parse.php ~~~~~~~~~~~~~~~##

// variables for number of test passed/failed for htmp
$tests_all = 0;
$tests_fail = 0;
$tests_success = 0;


$to_delete = array();
// cycle to "prohnat skrz" (get input through) parse.php
$parse_script ="php " .$parse_file; # TODO MUST BE CHANGED TO PHP7.4 TO RUN ON MERLIN
foreach ($files as $file) {
    
    // echo "file is:" .$file. "\n"; #debug
    preg_match('/(.*).src$/',$file,$matches);
    $xml_file = $matches[1].".xml";
    $to_delete[] = $xml_file;
    // echo "exec this: ".$parse_script . " <" . $file. " >" . $matches[1].".xml\n";
    shell_exec($parse_script . " <" . $file. " >" . $xml_file);
//EXEC() POUZIT?
    $rc = shell_exec("echo $?"); //$rc will have a return code saved for cmp
    
    // var_dump($rc_files);#debug
    
    foreach( $rc_files as $rc_file){
        // echo "RCfile: ",$rc_file,"\n"; #debug
        // echo "file: ", $matches[1],"\n"; #debug
        if($rc_file === $matches[1].".rc"){
// TESTOVAT POMOCI DIFF MUZE BYT
            $true_rc = file_get_contents($rc_file); #rc from a .rc file
            $tests_all++;

            // echo ("RC z .rc: ".$true_rc."\n"); #debug
            // echo ("RC soubor: ".$rc); #debug
            #trim because "$?" comes with '\n'
            $rc = trim($rc);
            // $true_rc = trim($true_rc);

            if($rc !== $true_rc){ //codes must match
                echo("fail1\n");
                $tests_fail ++;
            }
            else{
                // echo("success1\n");
                #cmp .xml & .out
                $tests_success ++;
                // $jarfile = "-jar /pub/courses/ipp/jexamxml/jexamxml.jar";
                // $retval;
                // exec("java ".$jarfile." test.out test.your_out diffs.xml  /D /pub/courses/ipp/jexamxml/options",$output,$retval);
            }
        }
    }           
}
// echo("~~~~~~~~~\n");
// echo "bad: ",$tests_fail, "\n";#debug
// echo "good: ",$tests_success, "\n";#debug

// var_dump($to_delete);
foreach($to_delete as $del){ // delete .xml files hence they are just to compare
    unlink($del);
}
##~~~~~~~~~~~~ HTML output ~~~~~~~~~~~~##
?>
<!DOCTYPE html>
<html>
<head>
<style>
table, th{
  border: 1px solid black;
  border-collapse: collapse;
}
th{
  padding: 15px;
  font-size: 25px;
}
</style>
</head>
<body>

<h1 style="color:DodgerBlue;"><font size= '7'>IPP 2020 tests evaluation</font size></h1>

<p><b><font size= '5'>home dir: <?php echo($base_home_dir); ?></font size></b></p>
<table style="width:100%">
    <tr>
        <th >Tests found</th>
        <th style="color:#01b401">Tests accepted</th>
        <th style="color:#e6002d">Tests failed</th>
    </tr>
    <tr>
        <th><?php echo($tests_all);?></th>
        <th style="color:#01b401"><?php echo($tests_success);?></th>
        <th style="color:#e6002d"><?php echo($tests_fail);?></th>
    </tr>

</table>
</body>
</html>

<!-- PRO XML TESTOVANI -->
<!-- java -jar /pub/courses/ipp/jexamxml/jexamxml.jar vas_vystup.xml referencni.xml delta.xml /pub/courses/ipp/jexamxml/options -->