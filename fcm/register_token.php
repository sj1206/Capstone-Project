$token = $_POST["Token"] or $_GET["Token"];
$_SESSION["dtkn"] = $token;
$query = "INSERT INTO Users(Token) Values ('$token') ON DUPLICATE KEY UPDATE Token = '$token' ";
sql_query($query);