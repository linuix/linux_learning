#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>

int main()
{
    if ((write(1,"Here is some data\n",18) != 18))
    {
        write(2,"A Write error has occurred on file descriptor l\n",46);
    }
    exit(0);
    
}
