#!/bin/sh

/home/mu/nginx/sbin/spawn-fcgi -a 127.0.0.1 -p 9092 -F 10 -f /home/mu/fcgi_test/test.cgi
