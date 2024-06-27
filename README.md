# microshell
A micro version of shell

# Testes
./microshell /bin/ls "|" /usr/bin/grep microshell ";" /bin/echo i love my microshell

./a.out /bin/echo "Hello World" "|" /usr/bin/tr 'a-z' 'A-Z'

* Execução do microshell VS bash
./a.out /bin/echo "Changing to the directory now" ";" cd /Users/david/Desktop ";" /bin/ls
./a.out /bin/echo "Changing to the directory now" ; cd /Users/david/Desktop ; /bin/ls

./a.out  cd /Users/davifern ; /bin/ls    
./a.out  cd /Users/davifern ";" /bin/ls    

* Entendendo melhor o main
./a.out   /bin/ls     "|"      /usr/bin/grep     microshell      “;”     /bin/echo       i love my microshell
^argv
i=0