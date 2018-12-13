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

    $camNumber = 1;
    $accountId = 1;
    $tiime = utf8_encode($_GET[tiime]);   

    echo $tiime;

    $pre_sql = $mysqli->query("SELECT Image_ID FROM Image_info");
    
    if($pre_sql->num_rows == 0){

      $sql = "ALTER TABLE Image_info AUTO_INCREMENT = 1";
      $mysqli->query($sql);

    }

    $weekago = mktime(date('H')+0, date('i'), date('s'), date('m'), date('d'), date('y'));

    $sql = "insert into Image_info (Cam_number, Account_ID, Time)";
    $sql = $sql. "values('$camNumber', '$accountId', '$tiime')"; 

    echo $sql;

    if ($mysqli->query($sql)) {      
      echo "success";     
    }
    else
      echo "failed";

  mysqli_close($con);
?>