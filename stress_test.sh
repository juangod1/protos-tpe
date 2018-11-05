
MUAs=0

create_mua(){
  (while true; do echo "CAPA"; sleep 2; done) | nc localhost 1110 > /dev/null
}

write(){
  clear
  echo "
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  ~~~~~~~~~  Welcome to the POP3 PROXY stress test.  ~~~~~~~~~
  ~~~~~~~~~                                          ~~~~~~~~~
  ~~~~~~~~~        Write 1 to generate a MUA         ~~~~~~~~~
  ~~~~~~~~~                                          ~~~~~~~~~
  ~~~~~~~~~                                          ~~~~~~~~~
  ~~~~~~~~~        Write quit to exit program        ~~~~~~~~~
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  ┌──────────────────┐
  │  Number of MUAs  │ $MUAs
  └──────────────────┘";
}

varname = "a"

while [ "$varname" != "quit" ]; do
  write
  read varname
  if [ "$varname" =  "1" ]; then
    create_mua &
    MUAs=$((MUAs+1))
  elif [ "$varname" = "quit" ]; then
    exit 1
  else
    echo "Unknown command.\n"
  fi
done

echo "Exiting program."
