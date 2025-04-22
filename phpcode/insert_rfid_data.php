<?php
// Include the database connection
include 'config.php';
date_default_timezone_set('Asia/Kolkata'); // Set timezone to IST

// Enable error reporting for debugging (disable in production)
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

// Set response header to JSON
header('Content-Type: application/json');

// Initialize response array
$response = array();

// Check if the request method is POST
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Get the raw POST data
    $rawInput = file_get_contents("php://input");
    
    // Decode the JSON payload
    $data = json_decode($rawInput, true);
    
    // Check if JSON decoding was successful
    if (json_last_error() !== JSON_ERROR_NONE) {
        $response['error'] = "Invalid JSON format.";
        echo json_encode($response);
        exit;
    }
    
    // Validate 'rfid_tag'
    if (!isset($data['rfid_tag']) || empty(trim($data['rfid_tag']))) {
        $response['error'] = "Missing or empty 'rfid_tag' in request.";
        echo json_encode($response);
        exit;
    }
    $rfid_tag = mysqli_real_escape_string($conn, trim($data['rfid_tag']));

    // Start transaction
    mysqli_begin_transaction($conn);
    
    try {
        // Insert RFID log
        $stmt = $conn->prepare("INSERT INTO rfid_logs (rfid_tag) VALUES (?)");
        if (!$stmt) {
            throw new Exception("Prepare failed: " . $conn->error);
        }
        $stmt->bind_param("s", $rfid_tag);
        if (!$stmt->execute()) {
            throw new Exception("Execute failed: " . $stmt->error);
        }
        $log_id = $stmt->insert_id;
        $stmt->close();
        
        // Check if the RFID tag exists in users
        $stmt = $conn->prepare("SELECT id FROM users WHERE rfid_tag = ?");
        if (!$stmt) {
            throw new Exception("Prepare failed: " . $conn->error);
        }
        $stmt->bind_param("s", $rfid_tag);
        if (!$stmt->execute()) {
            throw new Exception("Execute failed: " . $stmt->error);
        }
        $result = $stmt->get_result();
        if ($result->num_rows === 0) {
            // RFID tag not associated with any user
            $stmt->close();
            mysqli_commit($conn);
            $response['success'] = "RFID Tag logged successfully. No associated user found.";
            echo json_encode($response);
            exit;
        }
        $user = $result->fetch_assoc();
        $user_id = $user['id'];
        $stmt->close();
        
        // Fetch all relays assigned to the user
        $relay_query = "SELECT relay_id FROM user_relays WHERE user_id = ?";
        $stmt = $conn->prepare($relay_query);
        if (!$stmt) {
            throw new Exception("Prepare failed: " . $conn->error);
        }
        $stmt->bind_param("i", $user_id);
        if (!$stmt->execute()) {
            throw new Exception("Execute failed: " . $stmt->error);
        }
        $relays_result = $stmt->get_result();
        $relay_ids = [];
        while ($relay = mysqli_fetch_assoc($relays_result)) {
            $relay_ids[] = intval($relay['relay_id']);
        }
        $stmt->close();
        
        if (empty($relay_ids)) {
            $response['success'] = "RFID Tag logged successfully. No relays assigned to the user.";
            mysqli_commit($conn);
            echo json_encode($response);
            exit;
        }
        
        // Update the relay statuses
        $stmt = $conn->prepare("INSERT INTO relay_status (relay_id, status) VALUES (?, 1)
                                ON DUPLICATE KEY UPDATE status = IF(status = 1, 0, 1), updated_at = CURRENT_TIMESTAMP");
        if (!$stmt) {
            throw new Exception("Prepare failed: " . $conn->error);
        }
        
        foreach ($relay_ids as $relay_id) {
            $stmt->bind_param("i", $relay_id);
            if (!$stmt->execute()) {
                throw new Exception("Execute failed for relay_id $relay_id: " . $stmt->error);
            }
        }
        $stmt->close();

        // Commit transaction
        mysqli_commit($conn);

        $response['success'] = "RFID Tag logged successfully. Relay statuses toggled.";
        echo json_encode($response);
        exit;
        
    } catch (Exception $e) {
        // Rollback transaction on error
        mysqli_rollback($conn);
        $response['error'] = $e->getMessage();
        echo json_encode($response);
        exit;
    }
} else {
    $response['error'] = "Invalid request method.";
    echo json_encode($response);
    exit;
}
?>
