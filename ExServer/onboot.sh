#!/bin/bash
export LD_LIBRARY_PATH=/usr/local/lib:/var/www/authoring/converter/bin/linux64
export EX_SERVER_WORKING_DIR=/tmp/
export SC_MODELS_DIR=/var/www/sc_models/
cd /var/www/authoring/converter/bin/linux64
./ExServer 8888