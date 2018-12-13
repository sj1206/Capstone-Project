<?php

// Let's pass in a $_GET variable to our example, in this case

// it's aid for actor_id in our Sakila database. Let's make it

// default to 1, and cast it to an integer as to avoid SQL injection

// and/or related security problems. Handling all of this goes beyond

// the scope of this simple example. Example:

//   http://example.org/script.php?aid=42

if (isset($_GET['Account_ID']) && is_numeric($_GET['Account_ID'])) {
	$Account_ID = (int) $_GET['Account_ID'];
} else {
	$Account_ID = 1;
}
// Perform an SQL query
$con=mysqli_connect('localhost', 'tjzks77', 'rkttmxhs1', 'tjzks77'); 
if (mysqli_connect_errno($con))  
{  
	echo "Failed to connect to MySQL: " . mysqli_connect_error();  
}  

mysqli_set_charset($con,"utf8");
$res = mysqli_query($con,"SELECT Image_ID, Time FROM Main_image WHERE Cam_number = 1");  
$result = array();  
while($row = mysqli_fetch_array($res)){  
	array_push($result,  
		array('Image_ID'=>$row[0],'Time'=>$row[1] 
	));  
}  

echo json_encode(array("result"=>$result));



mysqli_close($con);  







?>