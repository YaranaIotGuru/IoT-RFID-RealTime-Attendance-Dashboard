<?php
// Check if a session is already active before starting one
if (session_status() === PHP_SESSION_NONE) {
    session_start();
}

// Set default timezone
date_default_timezone_set('Asia/Kolkata');

// Database Configuration
$host = "localhost"; // Database Host
$user = "your_use"; // Database Username
$pass = "your_pass"; // Database Password
$db = "your_db_name"; // Database Name

// Create Database Connection
$conn = mysqli_connect($host, $user, $pass, $db);

// Validate Connection
if (!$conn) {
    die("Database connection failed: " . mysqli_connect_error());
}

// Set Character Set
mysqli_set_charset($conn, 'utf8');
$conn->set_charset('utf8mb4');
?>
