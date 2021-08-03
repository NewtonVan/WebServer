#!/bin/sh
/usr/local/mysql/bin/mysqld --initialize-insecure --user=root --basedir=/user/local/mysql --datadir=/usr/local/mysql/data
service mysql start
mysqladmin -u root password "root"
cd /TinyServer
cp /usr/local/mysql/include/ . -fr
cp /usr/local/mysql/lib/ . -fr