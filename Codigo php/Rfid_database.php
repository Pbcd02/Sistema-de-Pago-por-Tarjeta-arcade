<?php
 
$hostname = "localhost"; 
$username = "root"; 
$password = "123"; 
$database = "tarjetero"; 

$conn = mysqli_connect($hostname, $username, $password, $database);


if(isset($_POST['idCard']) && isset($_POST["saldo"])) {
	$id = $_POST['idCard'];
	$s = $_POST["saldo"];

	$sql = "INSERT INTO tabla_idCard (idCard, saldo) VALUES ('".$id."', ".$s.")"; 

	if (mysqli_query($conn, $sql)) { 
		echo "Registro Exitoso"; 
	} else { 
		echo "Fallo Al Registrar la Tarjeta"; 
	}
}
else {
    echo "Error: No se recibieron los datos esperados desde el Arduino.";
}
?>