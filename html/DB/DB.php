<?php


$con=mysqli_connect('localhost', 'tjzks77', 'rkttmxhs1', 'tjzks77'); 
if (mysqli_connect_errno($con))  
{  
    echo "Failed to connect to MySQL: " . mysqli_connect_error();  
}  

mysqli_set_charset($con,"utf8");
$res = mysqli_query($con,"SELECT first, second, third, fourth FROM Main_image");  
$result = array();  
while($row = mysqli_fetch_array($res)){  

	         for($i = 0 ; $i < 4 ; $i++){
        $sql = mysqli_query($con, "SELECT All Time FROM Image_info WHERE Image_ID =".$row[$i]);

        $Time_row = mysqli_fetch_array($sql);
    
if($i == 0){        
        $first_time = $Time_row[0];
}
else if($i == 1){
        $second_time = $Time_row[0];   
}
else if($i == 2){
        $third_time=$Time_row[0];   
}
else if($i == 3){
        $fourth_time=$Time_row[0];   
}  
    }
 	array_push($result,  
        array('first'=>$row[0],'second'=>$row[1],'third'=>$row[2],'fourth'=>$row[3] ,'first_time' => $first_time , 'second_time' => $second_time , 'third_time' => $third_time , 'fourth_time' => $fourth_time
    ));  


}


   



echo json_encode(array("result"=>$result));



mysqli_close($con);  




?>