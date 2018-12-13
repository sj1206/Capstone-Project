<?php
define("GOOGLE_SERVER_KEY", "AIzaSyC065da-OKAbTQTCRx0XRwHyGIKWQCYFb8");
function send_fcm($message, $id) {
$url = 'https://fcm.googleapis.com/fcm/send';

$headers = array (
'Authorization: key=' . GOOGLE_SERVER_KEY,
'Content-Type: application/json'
);

$fields = array ( 
'data' => array ("message" => $message),
'notification' => array ("body" => $message)	
);

if(is_array($id)) {
$fields['registration_ids'] = $id;
} else {
$fields['to'] = $id;
}

$fields['priority'] = "high";

$fields = json_encode ($fields);

$ch = curl_init ();
curl_setopt ( $ch, CURLOPT_URL, $url );
curl_setopt ( $ch, CURLOPT_POST, true );
curl_setopt ( $ch, CURLOPT_HTTPHEADER, $headers );
curl_setopt ( $ch, CURLOPT_RETURNTRANSFER, true );
curl_setopt ( $ch, CURLOPT_POSTFIELDS, $fields );

$result = curl_exec ( $ch );
if ($result === FALSE) {
//die('FCM Send Error: ' . curl_error($ch));
} 
curl_close ( $ch );
return $result;
}
?>