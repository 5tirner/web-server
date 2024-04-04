<?php
session_start();
if ($_SESSION['bgcolor'] == 'red')
{
    echo "<h1>pink</h1>";
    $_SESSION['bgcolor'] = 'pink';
}
else if ($_SESSION['bgcolor'] == 'pink')
{
    echo "<h1>green</h1>";
    $_SESSION['bgcolor'] = 'green';
}
else if ($_SESSION['bgcolor'] == 'green')
{
    echo "<h1>yellow</h1>";
    $_SESSION['bgcolor'] = 'yellow';
}
else if ($_SESSION['bgcolor'] == 'yellow')
{
    echo "<h1>gray</h1>";
    $_SESSION['bgcolor'] = 'gray';
}
else if ($_SESSION['bgcolor'] == 'gray')
{
    echo "<h1>coral</h1>";
    $_SESSION['bgcolor'] = 'coral';
}
else if ($_SESSION['bgcolor'] == 'coral')
{
    echo "<h1>orange</h1>";
    $_SESSION['bgcolor'] = 'orange';
}
else
{
    echo "<h1>red</h1>";
    $_SESSION['bgcolor'] = 'red';
}

header("refresh: 1;");
?>

<!DOCTYPE html>
<html>
<head>
    <style>
        body {
            background-color: <?php echo $_SESSION['bgcolor']; ?>;
        }
    </style>
</head>
<body>

</body>
</html>
