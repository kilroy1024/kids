entry=$1
input="cscope.out"

echo "please generate:"
#echo "find . -name '*.c' > cscope.files;cscope -b -c -R"
echo "find . -name '*.c' -o -name '*.h' -o -name '*.S' -o -name '*.s'  > cscope.files;cscope -b -c -R"
echo "[if found p-roblem: S-egmentation fault]=>[tceetree_src/tceetree_20170803 make ]"
echo "sudo apt install graphviz"

tceetree -V -C max -r $entry -i $input -o $entry.dot

ls -lh $entry.dot

dot -Grankdir=LR -Tsvg "$entry.dot" -o "$entry.svg"

