<?php
$con=mysqli_connect('localhost', 'tjzks77', 'rkttmxhs1', 'tjzks77');
if (mysqli_connect_errno($con))  
{  
   echo "Failed to connect to MySQL: " . mysqli_connect_error();  
}
mysqli_set_charset($con,"utf8");
$token = $_POST["Token"];
$_SESSION["dktn"] = $token;
mysqli_query($con,"INSERT INTO Users(Token) Values ('$token') ON DUPLICATE KEY UPDATE Token = '$token'; ");
mysqli_close($con);

?>

