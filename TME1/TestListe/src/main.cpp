#include "List.h"
#include <string>
#include <iostream>
#include <cstring>
using namespace pr;
int main () {

	std::string abc = "abc";
	char * str = new char[4];
	str[0] = 'a';
	str[1] = 'b';
	str[2] = 'c';
	str[3] = '\0';
	int i = 0;

	if (! strcmp (str, abc.c_str())) {
		std::cout << "Equal !"<<std::endl;
	}

	pr::List list;
	list.push_front(abc);
	list.push_front(abc);

	std::cout << "Liste : " << list << std::endl;
	std::cout << "Taille : " << list.size() << std::endl;

	// Affiche à l'envers
	for (i= list.size() - 1 ; i > 0 ; i--) {
		std::cout << "elt " << i << ": " << list[i] << std::endl;
	}

	// liberer les char de la chaine
	// et la chaine elle meme
	//FAUTE : on delete seulement str le contenue est automatiquement deleted
	delete[] str;

}

/*
Equal !
Liste : [abc, abc]
Taille : 2
elt 1: abc
==7455== 
==7455== HEAP SUMMARY:
==7455==     in use at exit: 0 bytes in 0 blocks
==7455==   total heap usage: 5 allocs, 5 frees, 74,836 bytes allocated
==7455== 
==7455== All heap blocks were freed -- no leaks are possible
==7455== 
==7455== For lists of detected and suppressed errors, rerun with: -s
==7455== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
*/