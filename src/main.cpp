#include <stdio.h>
#include "../include/menu.h"
#include "../include/kitchen.h"

int main()
{
    StartKitchenThread();
    //int run = 1;
    while (RenderMenu())
    {
    }
    return 0;
}