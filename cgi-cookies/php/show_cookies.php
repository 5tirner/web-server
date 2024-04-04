<?php
// Check if cookies are set
if (isset($_COOKIE['name']) && isset($_COOKIE['email']) && isset($_COOKIE['age']) && isset($_COOKIE['time'])) {
    $name = $_COOKIE['name'];
    $email = $_COOKIE['email'];
    $age = $_COOKIE['age'];
    $expiryTime = $_COOKIE['time'] - time();

    echo "<h2>Cookies:</h2>";
    echo "<p><strong>Name:</strong> $name</p>";
    echo "<p><strong>Email:</strong> $email</p>";
    echo "<p><strong>Age:</strong> $age</p>";
    echo "<p><strong>Expire in:</strong> $expiryTime seconds</p>";
} else {
    echo "<p>No cookies set</p>";
}
?>
