#!/bin/bash

read -p "Update web version? " -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
then
    
	cp search.json /Users/danielbrandenburg/bnl/local/www/danbburg.me/pages/roobarb/search.json
	cp index.html /Users/danielbrandenburg/bnl/local/www/danbburg.me/pages/roobarb/index.html
	cp -rf xml/ /Users/danielbrandenburg/bnl/local/www/danbburg.me/pages/roobarb/xml/
	cp -rf mergedocs/ /Users/danielbrandenburg/bnl/local/www/danbburg.me/pages/roobarb/mergedocs/

fi