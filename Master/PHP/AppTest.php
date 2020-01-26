<?php


if($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["test"]))
{
	echo json_encode(array("test_success" => true));
}


?>