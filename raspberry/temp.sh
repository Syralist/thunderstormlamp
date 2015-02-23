#!/bin/bash

while [ true ]
do
    vcgencmd measure_temp
    sleep 10
done
