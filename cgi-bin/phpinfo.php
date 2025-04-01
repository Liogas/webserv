#!/usr/bin/php-cgi
<?php
// Définir le bon en-tête pour CGI
header("Content-type: text/html; charset=utf-8");
echo "<!DOCTYPE html>";
echo "<html lang='fr'>";
echo "<head><title>Résultat CGI</title></head>";
echo "<body>";

echo "<h1>Test de PHP-CGI</h1>";

echo "<h2>Informations sur le serveur</h2>";
echo "<p>Version de PHP : " . phpversion() . "</p>";
echo "<p>Version de l'interpréteur : " . php_uname() . "</p>";


echo "</body></html>";
?>
