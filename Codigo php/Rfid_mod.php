<?php
$hostname = "localhost";
$username = "root";
$password = "123";
$database = "tarjetero";


$conn = mysqli_connect($hostname, $username, $password, $database);


if(isset($_POST['idCard']) && isset($_POST['nuevoSaldo'])) {
    $id = $_POST['idCard'];
    $nuevoSaldo = $_POST['nuevoSaldo'];

    $query = "UPDATE tabla_idCard SET saldo = ? WHERE idCard LIKE ?";
    $stmt = $conn->prepare($query);
    $stmt->bind_param("is", $nuevoSaldo, $id);
    $stmt->execute();

    if($stmt->affected_rows > 0) {
        echo "Recarga exitosa";
    } else {
        echo "Error al recargar el saldo";
    }

    $stmt->close();
} else {
    echo "Parámetros incompletos";
}

$conn->close();
?>