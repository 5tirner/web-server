
<html>

<head>
<title>Dynamic Background Color Change</title>
</head>

<body bgcolor="<?php
if (isset($_GET['btn']))
{
$col=$_GET['t1'];
if(isset($col))
{
echo $p=$col;
}
else
{
echo $p="#ffffff";
}
}
?>">

<form action="" method="get" >
<strong> Choose Color to Change Background :- </strong>
<select name="t1">
<option value="">Choose Color </option>
<option value="#000000"> Black </option>
<option value="#0000ff"> Blue </option>
<option value="#a52a2a"> Brown </option>
<option value="#00ffff"> Cyan </option>
<option value="#006400"> Dark Green </option>
<option value="#808080"> Grey </option>
<option value="#008000"> Green </option>
<option value="#ffa500"> Orange </option>
<option value="#ffc0cb"> Pink </option>
<option value="#800080"> Purple </option>
<option value="#ff0000"> Red </option>
<option value="#ffffff"> White </option>
<option value="#ffff00"> Yellow </option>
</select>
<br>
<input type="submit" name="btn" value="Submit">
</form>

</body>
</html>

