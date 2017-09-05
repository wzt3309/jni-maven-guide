#!/usr/bin/env bash

mvn validate generate-resources
make
mvn compile package