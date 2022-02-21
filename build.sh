PEM_FILE="~/Downloads/test.pem"
HOST="ec2-user@ec2-54-91-119-58.compute-1.amazonaws.com"

ssh -i $PEM_FILE $HOST 'rm -rf tiny_os;'

scp -i $PEM_FILE -r ~/Desktop/hack/tiny-os $HOST:~/tiny_os

ssh -i $PEM_FILE $HOST '\
    cd tiny_os; \
    nasm -f elf32 loader.s; \
    ld -T link.ld -melf_i386 loader.o -o kernel.elf; \
    cp kernel.elf iso/boot/; \
    genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os -input-charset utf8 -quiet -boot-info-table -o os.iso iso; \
'

scp -i $PEM_FILE $HOST:~/tiny_os/os.iso .
