a="l64 l32 m32 m64"
b="https://raw.githubusercontent.com/KxSystems/kdb/master"
for x in $a; do
	mkdir -p "kx/$x"
	curl -o "kx/$x/c.o" "$b/$x/c.o"
done
curl -o "kx/k.h" "$b/c/c/k.h"
