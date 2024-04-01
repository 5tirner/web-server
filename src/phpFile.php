<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Login Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
        }
        form {
            margin: 0 auto;
            width: 300px;
            padding: 20px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }
        input[type="text"], input[type="password"] {
            width: 100%;
            padding: 10px;
            margin: 8px 0;
            display: inline-block;
            border: 1px solid #ccc;
            border-radius: 4px;
            box-sizing: border-box;
        }
        input[type="submit"] {
            width: 100%;
            background-color: #4CAF50;
            color: white;
            padding: 14px 20px;
            margin: 8px 0;
            border: none;
            border-radius: 4px;
            cursor: pointer;
        }
        input[type="submit"]:hover {
            background-color: #45a049;
        }
    </style>
</head>
<body>

<?php
// Check if form is submittednfs/homes/yachaab/Desktop/web-server/src/phpFile.php
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Check if username and password are correct (dummy credentials for demonstration)
    $username = "user";
    $password = "password";

    if ($_POST["username"] == $username && $_POST["password"] == $password) {
        // Redirect to a protected page if login is successful
        header("Location: protected_page.php");
        exit();
    } else {
        $login_error = "Invalid username or password.";
    }
}
?>

<form method="post" action="/php/phpFile.php">
    <h2>Login</h2>
    <?php if(isset($login_error)) { echo "<p style='color:red;'>$login_error</p>"; } ?>
    <label for="username">Username:</label>
    <input type="text" id="username" name="username" required><br>
    <label for="password">Password:</label>
    <input type="password" id="password" name="password" required><br>
    <input type="submit" value="Login">
</form>

</body>
</html>