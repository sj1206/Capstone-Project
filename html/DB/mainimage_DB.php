<?php
    header("Content-Type: text/html; charset=UTF-8");
    $con=mysqli_connect('localhost', 'tjzks77', 'rkttmxhs1', 'tjzks77');   

    if (mysqli_connect_errno($con))  
    {  
      echo "Failed to connect to MySQL: " . mysqli_connect_error();  
    }    

    $host = 'localhost';
    $user = 'tjzks77';
    $pw = 'rkttmxhs1';
    $dbName = 'tjzks77';
    $mysqli = new mysqli($host, $user, $pw, $dbName);

    $firstig =  utf8_encode($_GET[first]);   
    $secondig =  utf8_encode($_GET[second]);   
    $thirdig =  utf8_encode($_GET[third]);   
    $fourthig =  utf8_encode($_GET[fourth]);   

    
    $sql = "insert into Main_image (first, second, third, fourth)";
    $sql = $sql. "values('$firstig', '$secondig', '$thirdig','$fourthig')"; 

    echo $sql;

    if ($mysqli->query($sql)) {      
      echo "success";     
    }
    else
      echo "failed";

  mysqli_close($con);
?>