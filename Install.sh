#!/bin/bash
set -e -x 
echo "Installing libfcgi-dev"
echo "----------------------------------------"
apt-get install libfcgi-dev -y 

apt-get install g++ -y
echo "Installing spawn-fcgi"
echo "----------------------------------------"
apt-get install spawn-fcgi -y 

echo "Installing nginx"
echo "----------------------------------------"
apt-get install nginx-full -y 
echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
echo "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^"
echo "Installing curl"
echo "----------------------------------------"
apt-get install curl -y 1>&2

echo "Stopping nginx server"
echo "----------------------------------------"
service nginx stop

echo "Transfering nginx configurion files"
echo "----------------------------------------"
sudo cp -r  ./nginx/nginx.conf /etc/nginx/ 
sudo cp -r  ./nginx/sites-enabled/default /etc/nginx/sites-enabled/ 
sudo cp  ./nginx.conf /etc/init/nginx.conf

make

echo "Starting nginx server"
echo "----------------------------------------"
service nginx start
 
echo "Starting the File Server"
echo "----------------------------------------"

sudo fuser  -f -k 8000/tcp 

spawn-fcgi -p 8000 -n /var/www/run &
