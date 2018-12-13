<?php
// new file uploaded?

if(strlen(basename($_FILES["userfile"]["name"])) > 0)
{
	$uploadfile = basename($_FILES["userfile"]["name"]);

	if(move_uploaded_file($_FILES["userfile"]["tmp_name"], $uploadfile))
	{
		@chmod($uploadfile,0777);
		echo "Image Upload is OK!";
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
		$tiime = date('Y-m-d_His');

		$weekago = mktime(date('H')+0, date('i'), date('s'), date('m'), date('d'), date('y'));

		$sql = "insert into Image_info (Cam_number, Account_ID, Time)";
		$sql = $sql. "values('$camNumber', '$accountId', '$tiime')"; 

		echo $sql;

		if ($mysqli->query($sql)) {      
			echo "success";     
		}

		include_once('fcm/push_notification.php');

		$conn = mysqli_connect(DB_HOST, DB_USER, DB_PASSWORD, DB_NAME);

		$sql = "Select Token From users";

		$result = mysqli_query($conn,$sql);
		$tokens = array();

		if(mysqli_num_rows($result) > 0 ){

		while ($row = mysqli_fetch_assoc($result)) {
			$tokens[] = $row["Token"];
		}
	}

	mysqli_close($conn);
	
        $myMessage = $_POST['message'];
	if ($myMessage == ""){
		$myMessage = "Smoke Detected.";
	}

	$message = iconv("euc-kr","utf-8",$message) ;
	$message = array("message" => $myMessage);
	$message_status = send_notification($tokens, $message);
	echo $message_status;



	}
	else
		echo "Error copying!";
}

echo "add to arch ";
copy($uploadfile,"Cam_image/".date('Y-m-d_His', $weekago).".jpg");


?>