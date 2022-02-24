HOST="ec2-user@ec2-3-88-225-230.compute-1.amazonaws.com"
PEM_FILE="~/Downloads/test.pem"


ssh -i $PEM_FILE $HOST 'sudo yum install -y nasm genisoimage gcc;'

rsync -a . $HOST:~/tiny_os -e "ssh -i $PEM_FILE"

ssh -i $PEM_FILE $HOST '\
    cd tiny_os; \
    make os.iso; \
'

scp -i $PEM_FILE $HOST:~/tiny_os/os.iso .
