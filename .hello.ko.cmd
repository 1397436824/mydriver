cmd_/home/oyk/mydriver/hello.ko := ld -r -m elf_x86_64  -z max-page-size=0x200000 -T ./scripts/module-common.lds --build-id  -o /home/oyk/mydriver/hello.ko /home/oyk/mydriver/hello.o /home/oyk/mydriver/hello.mod.o ;  true