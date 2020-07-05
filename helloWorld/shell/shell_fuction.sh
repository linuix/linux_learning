foo() {
    printf "im in foo print!\n"
}

foo2() {
    echo "input param is $1"
}

foo3() {
    echo "im form main str = $1" 
    return 100
}

echo "shell is start"
foo
foo2 "input param"
foo3 "main str"
echo "foo3 return str = $?"
echo "shell end"
