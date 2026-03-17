mkdir a
echo hello > a/b
mkdir c
echo hello > c/b
echo hello > b
echo "a/b
c/b
b" | xargs grep hello
(echo 1 ; echo 2) | xargs -n 1 echo
rm -rf a c b 

