#!/bin/sh

/home/1/nginx/sbin/spawn-fcgi -a 127.0.0.1 -p 9092 -F 1 -f /home/1/fcgi/test.cgi
