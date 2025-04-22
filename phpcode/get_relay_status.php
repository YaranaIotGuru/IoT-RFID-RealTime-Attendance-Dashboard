<?php
require_once 'config.php'; // Include database configuration

header("Content-Type: application/json"); // Set header for JSON response

$response = ["status" => "success", "relays" => []];

// Query to get relay status
$sql = "SELECT relay_id, status FROM relay_status";
$result = $conn->query($sql);

if ($result && $result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $response["relays"][$row['relay_id']] = (int)$row['status'];
    }
} else {
    $response["status"] = "error";
    $response["message"] = "No relay data found";
}

// Send JSON response
echo json_encode($response);

$conn->close();
?>
