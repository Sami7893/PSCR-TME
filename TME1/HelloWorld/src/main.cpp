#include <iostream>
int main()
{
    std::cout << "Hello World!" << std::endl;
    
    int tab[10];
    
    for(int i = 9; i>=0;i--){
        tab[i]=i;
    }

    for (size_t i = 9; tab[i]; i--)
    {
        if (tab[i] - tab[i - 1] != 1)
        {
            std::cout << "probleme !";
        }
        std::cout << "i=" << i << std::endl;
    }

    return 0;
}