CC=gcc
N=10000
all:
	$(CC) -c -mavx2 -O3  simd.c -o simd.o
	$(CC) -O3 test_simd.c simd.o -o test_simd
	$(CC) test_mprotect.c -o test_mprotect
	python3 ./gen_tls_pic.py $(N) > tls.pic.py.c
	$(CC) -c -S -fPIC tls.pic.py.c -o tls.pic.py.s
	$(CC) -O3 -fPIC -shared tls.pic.py.s -o tls.pic.so
	$(CC) test_tls.c -o test_tls ./tls.pic.so  -ldl
clean:
	rm -rf main test_simd *.o *.bc *.s *.so test_pic_tls test_mprotect test_tls



