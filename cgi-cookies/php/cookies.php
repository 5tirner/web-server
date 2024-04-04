<?php
// Start the session
session_start();

// Set a cookie
setcookie('test_cookie', 'test_value', time() + 3600, '/');

// Check if the cookie is set
if (isset($_COOKIE['test_cookie'])) {
    echo 'Cookies are enabled.';
} else {
    echo 'Cookies are disabled.';
}
?>

