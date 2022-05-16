# Artifact and Source Code For Our TOSEM Paper

Buddy Stacks: Protecting Return Addresses with Efficient Thread-Local Storage and Runtime Rerandomization


## 1. How to Reuse the Prebuilt Virtual Machine


#### (1). Download and intall [VMware Workstation Player 15](https://www.vmware.com/products/workstation-player/workstation-player-evaluation.html). 
     
> The free version is available for non-commercial use. 

#### (2). Download the compressed image of our virtual machine [Ubuntu18.04.vmx](https://drive.google.com/file/d/1RcqPySuE65ew-EW3k6YU9QCwa3yvL8oV/view?usp=sharing)  and uncompress it.

> TOSEM.7z

#### (3). Use VMware Workstation Player 15 to open the uncompressed virtual machine Ubuntu18.04.vmx.

> The **password** to login is **123456**


#### (4). Follow the steps in BuddyStack.txt


## 2. How to Build and Use BuddyStack on Your Own Machine from Scratch

#### (1) Install Rustc and Set the Default Version to Be 1.43 

```sh
iron@CSE:~$ curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh

iron@CSE:~$ rustup toolchain install 1.43

iron@CSE:~$ rustup default 1.43

# iron@CSE:~$ rustup self uninstall
```
#### (2) How to Build BuddyStack on Your Own Machine

#####  Open a New Terminal

```sh
iron@CSE:~$ cd github

iron@CSE:github$ pwd

/home/iron/github

iron@CSE:github$ git clone https://github.com/sheisc/BuddyStack.git

iron@CSE:github$ cd BuddyStack/

iron@CSE:BuddyStack$ . ./env.sh 

iron@CSE:BuddyStack$ ./build.sh
```

#### (3) How to Use BuddyStack to Build an Existing Project

#####  Open a New Terminal

```sh
iron@CSE:~$ cd github/BuddyStack/

iron@CSE:BuddyStack$ . ./runtime.sh 

iron@CSE:BuddyStack$ cd demo/

iron@CSE:demo$ make CC=spa-clang CXX=spa-clang++

spa-clang main.c -o main	
spa-cc 0.01
spa-as 0.01
###SPA###  /tmp/main-0713db.s contains main().
[+] Instrumented 2 locations (2-startproc, 2-endproc, 64-bit, non-hardened mode, ratio 100%).
spa-clang -c main.c -o main.o
spa-cc 0.01
spa-as 0.01
###SPA###  /tmp/main-876250.s contains main().
[+] Instrumented 2 locations (2-startproc, 2-endproc, 64-bit, non-hardened mode, ratio 100%).
objdump -d ./main.o

./main.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <test>:
   0:	49 89 e2             	mov    %rsp,%r10
   3:	49 81 e2 00 00 80 ff 	and    $0xffffffffff800000,%r10
   a:	4d 8b 92 00 00 00 ff 	mov    -0x1000000(%r10),%r10
  11:	4c 03 14 24          	add    (%rsp),%r10
  15:	4c 89 94 24 00 00 80 	mov    %r10,-0x800000(%rsp)
  1c:	ff 
  1d:	55                   	push   %rbp
  1e:	48 89 e5             	mov    %rsp,%rbp
  21:	48 83 ec 10          	sub    $0x10,%rsp
  25:	48 bf 00 00 00 00 00 	movabs $0x0,%rdi
  2c:	00 00 00 

...

```


#### (4) How to Use Our Pin Tool to Get the Stack Information of a Running Program

#####  Open a New Terminal

```sh
iron@CSE:~$ cd github/BuddyStack/

iron@CSE:BuddyStack$ . ./env.sh 

iron@CSE:BuddyStack$ pin -follow-execv -mt -t ./src/ParallelShadowStacks/MyPinTool/obj-intel64/MyPinTool.so -- ls

SPA.call.stack.20019.20019.0.txt   demo	       pin.log
TACO_MajorRevision.pdf		    env.sh	       remove.sh
...

iron@CSE:BuddyStack$ cat SPA.call.stack.*.txt 

20019 20019 0 11264 13 ls

The call stack size is 11264 bytes and the call stack depth is 13.
```
