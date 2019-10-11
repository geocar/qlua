mkdir -p kx/l64 kx/l32 kx/m32 kx/m64
BASEURL="https://raw.githubusercontent.com/KxSystems/kdb/master"
curl -o kx/k.h "$BASEURL/c/c/k.h"
curl -o kx/l32/c.o "$BASEURL/l32/c.o"
curl -o kx/l64/c.o "$BASEURL/l64/c.o"
curl -o kx/m32/c.o "$BASEURL/m32/c.o"
curl -o kx/m64/c.o "$BASEURL/m64/c.o"
