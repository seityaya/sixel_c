#!/bin/sh
for i in *.png ; do convert "$i" "${i%.*}.sixel" ; done
