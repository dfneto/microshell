# microshell
A micro version of shell

gcc microshell2_commented.c -o microshell && ./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell

./joan /bin/echo "Changing to the directory now"; cd /Users/davifern/Downloads ; /bin/ls


./joan cd /Users/davifern/Downloads; /bin/ls   


./a.out  cd /Users/davifern ; /bin/ls    
./a.out  cd /Users/davifern ";" /bin/ls    

./joan /bin/echo "Changing to the directory now" ";" cd /Users/davifern/Downloads ";" /bin/ls ";" cd /Users/davifern/dev -> Uso do joan
Changing to the directory now
JOAN CD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
42-EXAM         file1.tmp       file2.tmp       file3.tmp
JOAN CD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

./joan /bin/echo "Changing to the directory now" ; cd /Users/davifern/Downloads ; /bin/ls ; cd /Users/davifern/dev -> Use do linux
Changing to the directory now
42-EXAM         file1.tmp       file2.tmp       file3.tmp

gcc joan.c -o joan && ./joan /bin/echo "Hello World" "|" /usr/bin/tr 'a-z' 'A-Z' -- usando PIPE


./microshell   /bin/ls     "|"      /.usr/bin/grep     microshell      “;”     /bin/echo       i love my microshell
^ argv
I=0


./a.out         /bin/echo         "Changing to the directory now”     ;           cd               /Users/david/Desktop/        ;         /bin/ls
^argv
i=0