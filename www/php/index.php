<?php
header('Content-Type: text/html; charset=utf-8');

$name = isset($_GET['name']) ? htmlspecialchars($_GET['name']) : 'No name';
$id = isset($_GET['id']) ? (int)$_GET['id'] : 0;
?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Test - C HTTP Server</title>
    <style>
        body { font-family: system-ui, sans-serif; background-color: #f4f4f9; color: #333; margin: 40px; }
        .container { background: #fff; padding: 20px 40px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); max-width: 600px; margin: auto; }
        h1 { color: #0056b3; }
        table { border-collapse: collapse; width: 100%; margin-top: 20px; }
        th, td { text-align: left; padding: 10px; border-bottom: 1px solid #ddd; }
        th { background-color: #f8f9fa; width: 40%; }
        .success { color: green; font-weight: bold; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Welcome, <?php echo $name; ?></h1>
        <p>If you can see this page properly formatted, your C HTTP server with CGI support is working perfectly.</p>

        <h2>Query String Parameters (GET)</h2>
        <table>
            <tr><th>Parameter 'name'</th><td><?php echo $name; ?></td></tr>
            <tr><th>Parameter 'id'</th><td><?php echo $id > 0 ? $id : '<i>missing / invalid</i>'; ?></td></tr>
        </table>

        <h2>Server Variables (CGI Environment)</h2>
        <table>
            <tr>
                <th>Request Method</th>
                <td><span class="success"><?php echo $_SERVER['REQUEST_METHOD'] ?? 'Missing'; ?></span></td>
            </tr>
            <tr>
                <th>Script Path</th>
                <td><?php echo $_SERVER['SCRIPT_FILENAME'] ?? 'Missing'; ?></td>
            </tr>
            <tr>
                <th>PHP Version</th>
                <td><?php echo phpversion(); ?></td>
            </tr>
        </table>
    </div>
</body>
</html>