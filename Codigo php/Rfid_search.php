<?php
$hostname = "localhost";
$username = "root";
$password = "123";
$database = "tarjetero";

// Conexión a la base de datos
$conn = mysqli_connect($hostname, $username, $password, $database);

// Verificar si se ha enviado el identificador de tarjeta
if(isset($_POST['idCard'])) {
    $id = $_POST['idCard'];
    
    // Consulta preparada para evitar inyección SQL
    $query = "SELECT id, saldo FROM tabla_idCard WHERE idCard LIKE ?";
    $stmt = $conn->prepare($query);
    $stmt->bind_param("s", $id);
    $stmt->execute();
    $result = $stmt->get_result();

    // Verificar si se encontraron resultados
    if($result->num_rows > 0) {
        // Obtener el saldo y el ID de la fila encontrada
        $row = $result->fetch_assoc();
        $saldo = $row['saldo'];
        $id = $row['id'];
        echo $saldo . "," . $id; // Devolver saldo e ID separados por una coma
    } else {
        echo " ,"; // Devolver espacio en blanco para saldo e ID
    }

    // Liberar recursos
    $stmt->close();
    $conn->close();
}else{
 echo"datos no recibidos";
}
?>
