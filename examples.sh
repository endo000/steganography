case $1 in
"1")
  flags="-h -i files/lenna.png -m files/message.txt -o files/hidden.png"
  ;;
"2")
  flags="-h -i files/test.jpg -m files/message.txt -o files/hidden_test.jpg"
  ;;
"3")
  flags="-h -i files/test_bmp.bmp -m files/message.txt -o files/hidden_bmp.bmp"
  ;;
"4")
  flags="-h -i files/lenna.png -m files/message.txt -o files/hidden.png --random"
  ;;
"5")
  flags="-h -i files/lenna.png -m files/message.txt -o files/hidden.png --bits 4"
  ;;
"6")
  flags="-h -i files/lenna.png -m files/message.txt -o files/hidden.png -r --bits 3"
  ;;
"7")
  flags="-h -i files/lenna.png -m files/message.txt -o files/hidden.png -g"
  ;;
"8")
  flags="-h -i files/lenna.png -m files/message.txt -o files/hidden.png -b"
  ;;
"9")
  flags="-h -i files/lenna.png -m files/message.txt -o files/hidden.png -b --random --bits 2"
  ;;
"10")
  flags="-h -i files/lenna.png -m files/alice.txt -o files/hidden.png --bits 4"
  ;;
"11")
  flags="-u -i files/hidden.png -o files/revealed_message.txt"
  ;;
"12")
  flags="-u -i files/hidden_test.jpg -o files/revealed_message.txt"
  ;;
"13")
  flags="-u -i files/hidden_bmp.bmp -o files/revealed_message.txt"
  ;;
esac

./do_steganography $flags
