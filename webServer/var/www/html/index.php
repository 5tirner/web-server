<!DOCTYPE html>
<html>
<head>
    <title>My PHP Web Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f3f3f3;
            text-align: center;
        }
        .container {
            width: 80%;
            margin: 0 auto;
            background-color: #fff;
            padding: 20px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
        }
        h1 {
            color: #333;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Welcome to My PHP Page</h1>
        <p>Today is <?php echo date('l, F jS, Y'); ?>.</p>
        <p>The current time is <?php echo date('h:i:s A'); ?>.</p>
    </div>
</body>
</html>
