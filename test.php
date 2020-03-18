<?php
#autor: Fridrich David             
#datum odevzdani: 11.03.2020       
#zadani: IPP20 tester              
#datum posledni upravy: 11.03.2020

$directory = '/home/donnovan/skola/druhy/letni/IPP/test_parse_only_for_now';
// echo($directory);

if (!is_dir($directory)) {
    exit('Invalid diretory path');
}

$file_dirs = array();
$files = array();
foreach (scandir($directory) as $file) {
    if ($file !== '.' && $file !== '..') {
        $file_dirs[] = $file;
    }
}  ##file_dirs je array vsech slozek kterej sou uvnitr slozky "/test_parse_only_for_now"

// var_dump($file_dirs);

for($i=0;$i<15;$i++){
    foreach (scandir($directory."/".$file_dirs[$i]) as $file) {
        if(strpos($directory."/".$file_dirs[$i],"GENERATE")){
            continue;
        }
        // echo($file."\n");
        if(preg_match("/\.src$/",$file)){
            if ($file !== '.' && $file !== '..') {
                $files[] = $file;
                
            }
        }
    }
}


foreach($files as $output){
    echo($output."\n");
}

// $i = 0;
// foreach ($files as $one_file) {
//     $i++;
//     echo($i."__".$file);
// }


?>