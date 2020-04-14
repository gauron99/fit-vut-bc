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
$in_files = array();



#variables signaling what parameters were given (bool)
#note: int-only cant be combined with parse-only or parse-script!

$_directory = false;
$_recursive = false;
$_parse_script = false;
$_int_script = false;   #--interpret only
$_parse_only = false;
$_int_only = false;     #--interpret only
$_jexamxml = false;

#complete path to file
$parse_file;
$int_file;

#writes message provided specificaly by a calling function
#about the error and exits with the code
function err_exit($err_code,$err_reason){
    fputs(STDERR, "Error occured: ".$err_reason."; exiting w/ code(".$err_code.")\n");
    exit($err_code);
}

foreach(array_slice($argv,1) as $prgArg) { //array_slice >> dont include name of the script
    
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
        $tmp = explode("=",$prgArg);
        $directory = $tmp[1];
        if (!is_dir($directory)) {
            err_exit(11,"Path given to '--directory' is invalid");
        }
        // else{
        //     // echo ("hey baby the dir iz goood\n"); #debug
        // }
    }

    #--recursive
    elseif(preg_match('/^--recursive$/',$prgArg)){
        $_recursive = true;
    }
    #--parse_script=file
    elseif(preg_match('/^--parse_script=/',$prgArg)){

        $_parse_script = true;
        $tmp = explode("=",$prgArg);
        $parse_file = $tmp[1];
        if(preg_match('/parse.php$/',$parse_file)){

            if(file_exists($parse_file)); #absolute path found
            elseif(file_exists(getcwd() . $parse_file)){ #relative path found
                
                $parse_file = getcwd().$parse_file; #transform to absolute
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
    elseif(preg_match('/^--int_script=/', $prgArg)){
        $tmp = explode("=",$prgArg);
        $int_file = $tmp[1];
        if(preg_match('/interpret.py$/',$int_file)){

            if(file_exists($int_file)); #absolute path found
            elseif(file_exists(getcwd() . $int_file)){ #relative path found
                
                $int_file = getcwd().$int_file; #transform to absolute
            }
            else{ #file was not found using either absolute or relative path
                err_exit(11, "File 'interpret.py' in --int-script not found");
            }
        }
        else{
            err_exit(11, "File provided by --int-script is invalid");
        }
    }

    #--parse-only
    elseif(preg_match('/^--parse-only$/',$prgArg)){
        $_parse_only = true;
    }
    
    #--int_only
    elseif(preg_match('/^--int-only$/',$prgArg)){
        $_int_only = true;
    }
    // getcwd()."/jexamxml.jar"
    elseif(preg_match('/^--jexamxml=$/',$prgArg)){

        $tmp = explode("=",$prgArg);
        $jarfile = $tmp[1];
        if(preg_match('/jexamxml.jar$/',$jarfile)){

            if(file_exists($jarfile)); #absolute path found
            elseif(file_exists(getcwd() . $jarfile)){ #relative path found
                
                $jarfile = getcwd().$jarfile; #transform to absolute
            }
            else{ #file was not found using either absolute or relative path
                err_exit(11, "File 'jexamxml.jar' in --jexamxml not found");
            }
        }
        else{
            err_exit(11, "File provided by --jexamxml is invalid");
        }
    }
    
    else{
        echo ("Unknown program argument passed by cmd, exiting\n");#debug
        exit(0);
    }

} #end of parsing arguments

##~~~~~~~~~~~~~~~ additional checking ~~~~~~~~~~~~~~~##
# checking for script files if needed

if($_parse_script == false){ #cwd
    if(!file_exists(getcwd()."/parse.php")){
        err_exit(11, "File 'parse.php not found"); #path was not provided;file wasnt found in cwd
    }
    $parse_file = getcwd()."/parse.php";
}

if($_int_script == false){
    if(!file_exists(getcwd()."/interpret.py")){
        err_exit(11, "File 'interpret.py not found"); #path was not provided;file wasnt found in cwd
    }
    $int_file = getcwd()."/interpret.py";
}
// if($_jexamxml == false){
//     if(!file_exists("/pub/courses/ipp/jexamxml/jexamxml.jar")){
//         err_exit(11, "File 'jexamxml.jar not found"); #path was not provided;file wasnt found in cwd
//     }
//     $jarfile = "/pub/courses/ipp/jexamxml/jexamxml.jar";
// }

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
    // global $_parse_only;
    global $rc_files;
    global $out_files;
    global $in_files;
    
    // (preg_grep('/^([^.])/',$file)) #~~ ignore hidden files
    foreach (preg_grep('/^([^.])/', scandir($home_dir)) as $file) {
        
        if($_recursive){
            $dir = $home_dir."/".$file; #has to be set so subdirs are recognized as dirs
            if(is_dir($dir)){ #if file is directory, call function recursively
                if ($file !== '.' && $file !== '..'){
                    if ($home_dir[strlen($home_dir)-1] == "/"){
                        getFiles($home_dir.$file);
                    }
                    else{
                       getFiles($home_dir."/".$file);
                    }
                }
            continue; #~~ dont save dirs to $files
            }
        }
        if(is_dir($file)){ //if recursive is not set, dont include dirs
            continue;
        }

        if ($file !== '.' && $file !== '..'){
            if(preg_match('/(.*).src$/',$file,$matches)){
                if ($home_dir[strlen($home_dir)-1] == "/"){
                    $files[] = $home_dir.$file; #save files with absolute path
                }
                else{
                    $files[] = $home_dir."/".$file; #save files with absolute path
                }
                
                #cycle to find .out & .rc files if they exist
                $rc_exists = false;
                $out_exists = false;
                $in_exists = false;
                foreach(scandir($home_dir) as $adj_file){
                    
                    if(strpos($adj_file,$matches[1]) !== false){                            
                        if($adj_file === $matches[1].".rc"){
                            $rc_exists = true;
                            if ($home_dir[strlen($home_dir)-1] == "/"){
                                $rc_files[] = $home_dir.$adj_file;
                            }
                            else{
                                $rc_files[] = $home_dir."/".$adj_file;
                            }
                        }
                        elseif($adj_file === $matches[1].".out"){
                            $out_exists = true;
                            if ($home_dir[strlen($home_dir)-1] == "/"){
                                $out_files[] = $home_dir.$adj_file;
                            }
                            else{
                                $out_files[] = $home_dir."/".$adj_file;
                            }
                        }
                        elseif($adj_file === $matches[1].".in"){
                            $in_exists = true;
                            if ($home_dir[strlen($home_dir)-1] == "/"){
                                $in_files[] = $home_dir.$adj_file;
                            }
                            else{
                                $in_files[] = $home_dir."/".$adj_file;
                            }
                        }
                    }
                }#end of foreach

                if($rc_exists === false){
                    if($home_dir[strlen($home_dir)-1] == "/"){
                        file_put_contents($home_dir.$matches[1].".rc","0");
                        $rc_files[] = $home_dir.$matches[1].".rc";
                    }
                    else{
                        file_put_contents($home_dir."/".$matches[1].".rc","0");
                        $rc_files[] = $home_dir."/".$matches[1].".rc";
                    }
                }
                if($out_exists === false){
                    if($home_dir[strlen($home_dir)-1] == "/"){
                        file_put_contents($home_dir.$matches[1].".out",NULL);
                        $out_files[] = $home_dir.$matches[1].".out";
                    }
                    else{
                        file_put_contents($home_dir."/".$matches[1].".out",NULL);
                        $out_files[] = $home_dir."/".$matches[1].".out";

                    }
                }
                if($in_exists === false){
                    if($home_dir[strlen($home_dir)-1] == "/"){
                        file_put_contents($home_dir.$matches[1].".in",NULL);
                        $in_files[] = $home_dir.$matches[1].".in";
                    }
                    else{
                        file_put_contents($home_dir."/".$matches[1].".in",NULL);
                        $in_files[] = $home_dir."/".$matches[1].".in";
                    }
                }
            }
        }
    }
    // var_dump($files); #debug
} #end of function get files

// echo "~~~~~files~~~~~\n";
// var_dump($files);
// echo "~~~~~out_files~~~~~\n";
// var_dump($out_files);
// echo "~~~~~rc_files~~~~~\n";
// var_dump($rc_files);



$tests_all = 0;
$tests_fail = 0;
$tests_success = 0;

#-1 == not tested, parser mistake; 0 == parser was good, but int not; 1 == int was good
$html_test_int = array();
$html_rc = array(); 
$to_delete = array();#temp files, to be deleted at the very end

$nottested = "-1";
$fail = "0";
$ok = "1";
$notdiffed = "2";

if ($_int_only == false){


    $html_test_par = array(); #both hold values of 0 or 1 as follows: 1 == good test; 0 == bad test

    // cycle to "prohnat skrz" (get input through) parse.php
    $parse_script ="php " .$parse_file; # TODO MUST BE CHANGED TO PHP7.4 TO RUN ON MERLIN

    foreach ($files as $file) {
        
        preg_match('/(.*).src$/',$file,$matches);

        $xml_file = $matches[1].".xml";
        $to_delete[] = $xml_file;
        foreach ($out_files as $f) {
            if ( $f === $matches[1].".out")
                $out_file_ref = $f;

        }
        exec($parse_script . " <" . $file. " >" . $xml_file,$nic,$rc);
        
        foreach( $rc_files as $rc_file){
            if($rc_file === $matches[1].".rc"){

                $true_rc = file_get_contents($rc_file); #rc from a .rc file
                
                #trim because "$?" comes with '\n'
                $rc = trim($rc);
                $true_rc = trim($true_rc);
                $tests_all++;
                
                if($_parse_only){    
                    if($rc !== $true_rc){
                        $tests_fail ++;
                        $html_rc[] = $fail; #add 'no' at the end
                        $html_test_par[] = $notdiffed;
                        
                    }
                    elseif ($rc !== "0") {
                        $tests_success ++;
                        $html_rc[] = $ok;
                        $html_test_par[] = $notdiffed;

                    }
                    else{

                        $html_rc[] = $ok;
                        #cmp .xml & .out
                        $tmpDiffer = ($base_home_dir."diff");
                        file_put_contents($tmpDiffer,NULL);
                        
                        // $jarfileMerlin = "-jar /pub/courses/ipp/jexamxml/jexamxml.jar";
                        $jarfile = "/home/donnovan/skola/druhy/letni/IPP/IPP/jexamxml/jexamxml.jar";
                        exec("java -jar ".$jarfile." ".$xml_file." ".$out_file_ref." ".$tmpDiffer." /jexamxml/options",$nic,$xmlRC);

                        #cmp of files
                        clearstatcache();
                        if(filesize($tmpDiffer)){

                            $tests_fail ++;
                            $html_test_par[] = $fail; #add 0 at the end of the list
                            
                        } 
                        else{
                            $tests_success ++;
                            $html_test_par[] = $ok;
                        }
                        unlink($tmpDiffer);
                    }
                }### only for PARSE_ONLY
                else{# Rezim BOTH
                    #cmp RC 0 and thats it
                }
            }
        }
    }
}
// var_dump($files);
elseif($_parse_only == false){ #Rezim BOTH/int_only
    $int_output = "my_int.outpt.txt";
    file_put_contents($int_output,NULL);
    $to_delete[] = $int_output;
    // var_dump($files);
    $int_file ='python '.$int_file; #TODO MUST BE CHANGED TO WORK ON MERLIN 
    
    if($_int_only){ # use .src as xml input
        foreach ($files as $file){ #$file = file.src
            preg_match('/(.*).src$/',$file,$matches);
            $tests_all++;

            $tmp = array_search($matches[1].".out",$out_files);
            $out_file = $out_files[$tmp];
            $tmp = array_search($matches[1].".in",$in_files);
            $in_file = $in_files[$tmp];
            $tmp = array_search($matches[1].".rc",$rc_files);
            $rc_file = $rc_files[$tmp];
            
            $true_rc = file_get_contents($rc_file); #true rc now holds value in .rc for comparison
            #execute interpret.py
            $nic = array();
            exec($int_file." --source=".$file. " --input=".$in_file." >".$int_output,$nic,$rc);
            
            #trim because "$?" comes with '\n'

            $rc = trim($rc);
            $true_rc = trim($true_rc);
            strval($rc);
            strval($true_rc);

            if($rc !== $true_rc){ #if Ret codes are not equal, automaticaly big bad
                $tests_fail++;
                $html_rc[] = $fail;
                $html_test_int[] = $notdiffed;
            }
            else if($rc != "0"){ #if ret codes do match AND are not 0, all good, nothing to cmp
                $tests_success++;
                $html_rc[] = $ok;
                $html_test_int[] = $notdiffed;
            }
            else{ #codes are the same and they are both 0, therefore use diff
                $html_rc[] = $ok;
                $nic = array();
                exec("diff ".$int_output." ".$out_file,$nic,$intDiffRc);
                // var_dump($nic);
                if (count($nic) == 0){
                    #they are the same
                    $tests_success++;
                    $html_test_int[] = $ok;
                }
                else{
                    $tests_fail++;
                    $html_test_int[] = $fail;
                }
            }
        }
    }
    else{ # BOTH

    }
}
// exit(0);
if(!$_int_only){
    foreach($to_delete as $del){ // delete .xml files hence they are just to compare(BOTH or PARSE ONLY)
        unlink($del);
    }
}
// var_dump( $html_test_par);



##~~~~~~~~~~~~ HTML output ~~~~~~~~~~~~##
?>
<!DOCTYPE html> 
<html>
<head>
<style>
.beta, .beta th{
    border: 1px solid black;
    border-collapse: collapse;
}
.beta th{
    padding: 15px;
    font-size: 25px;
}
.theta{
  table-layout: fixed ;
  width: 100% ;
}
.theta td {
  width: 25% ;

}
</style>
</head>
<body style="background-color:#e6e6e6">

<h1 style="color:DodgerBlue;"><font size= "7">IPP 2020 tests evaluation</font size></h1>
<p><b><font size= "5">home dir:<?php echo(realpath($base_home_dir)); ?> </font size></b></p>
<table class="beta" style="width:100%">
<tr>
    <th>Tests found</th>
    <th style="color:#01b401">Tests accepted</th>
    <th style="color:#e6002d">Tests failed</th>
</tr>
<tr>
    <th><?php echo($tests_all) ?></th>
    <th style="color:#01b401"><?php echo($tests_success." (".(int)($tests_success/$tests_all*100)."%)") ?></th>
    <th style="color:#e6002d"><?php echo($tests_fail) ?></th>
</tr>
</table>
<h2>Tests <?php
if ($_parse_only){
    echo'(Mode ==  Parse_only)';
}
elseif($_int_only){
    echo'(Mode == Int_only)';
}
else{#both
    echo'(Mode == Both)';
}
?>
</h2> 

<!-- TABLE NUMBER 2 individual tests -->
<table class="theta" border=1 frame=void rules=rows style="margin-left:auto;margin-right:auto;">
<tr>
    <th>File name</th>
    <th>Ret Code check</th><?php
if ($_parse_only){
    echo'<th>Parser testing</th>';
    
}
elseif($_int_only){
   echo'<th>Interpret Testing</th>';
}
else{#both
    echo'<th>Parser testing</th>
    <th>Interpret testing</th>';
}
###### 'hlavicka' HTML done, pod timhle bude vypis testu a prislusne hodnoceni (dir,rc,output)
?>
</tr>
<?php
// echo ("<td>".."</td>\n");

for($i = 0; $i < count($files);$i++){
    
    echo ("<tr>\n"); # table row
    echo ("<td height='30' style='text-align:left'>".$files[$i]."</td>\n");
    if ($html_rc[$i] == "1"){
        echo ("<td height='30' style='text-align:center;color:#01b401'>OK</td>\n");
    }
    elseif($html_rc[$i] == "0"){
        echo("<td height='30' style='text-align:center;color:#e6002d'>FAIL</td>\n");
    }
    if(!$_int_only){
        if($html_test_par[$i] == "1"){
            echo ("<td height='30' style='text-align:center;color:#01b401'>OK</td>\n");
        }
        elseif($html_test_par[$i] == "0"){
            echo ("<td height='30' style='text-align:center;color:#e6002d'>Fail</td>\n");
        }
        else{
            echo ("<td height='30' style='text-align:center'>Not diffed</td>\n");
        }
    }
    if(!$_parse_only){
        if($html_test_int[$i] == "1"){
            echo ("<td height='30' style='text-align:center;color:#01b401'>OK</td>\n");
        }
        elseif($html_test_int[$i] == "0"){
            echo ("<td height='30' style='text-align:center;color:#e6002d'>Fail</td>\n");
        }
        else{
            echo ("<td height='30' style='text-align:center'>Not diffed</td>\n");
        }
    } 
}
?>
</tr><!-- table row end -->

