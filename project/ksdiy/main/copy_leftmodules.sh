#!/bin/sh
echo "current directory $PWD"
if [ ! -e ../../submodule/esp-idf/components/nghttp/nghttp2 ] ; then
echo "create nghttp2 esp-idf submodule..."
(cd ../../submodule/esp-idf/ && git submodule update --init components/nghttp/nghttp2)
fi
if [ ! -e ../../submodule/micropython/lib/berkeley-db-1.xx ] ; then
echo "create berkeley-db-1.xx micropython submodule..."
(cd ../../submodule/micropython && git submodule update --init lib/berkeley-db-1.xx)
fi