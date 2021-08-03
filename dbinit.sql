DROP DATABASE IF EXISTS brodb;
CREATE DATABASE brodb;

USE brodb;
CREATE TABLE user(
    username char(50) NULL,
    passwd char(50) NULL
)ENGINE=InnoDB;

INSERT INTO user(username, passwd) VALUES('cio', 'admin');
INSERT INTO user(username, passwd) VALUES('idi0tn3', 'admin');