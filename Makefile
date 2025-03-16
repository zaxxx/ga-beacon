CC=i686-w64-mingw32-g++
CFLAGS=-L/usr/i686-w64-mingw32/lib -shared -static -static-libgcc -static-libstdc++ -fpermissive -s
LDFLAGS=-lkernel32 -luser32 -lpsapi -lstdc++
SRC=./src/Detours.cpp ./src/Utils.cpp ./src/main.cpp ./src/version.def
OUT=out/version.dll

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LDFLAGS)

clean:
	rm -f $(OUT)
