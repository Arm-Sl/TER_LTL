#include <iostream>
#include "LTLFormula.h"
#include "LTLFormulaSet.h"

class A
{
public:
	A() {};
	~A() {};
	void feur() {};
private:
	int* c;
};


int main(int argc, char *argv[])
{
	
	auto s = LTLFormula::readFormule("(a&b)|c");
	
	std::cout << s->operator std::string() << std::endl;


	LTLFormulaSet fs(LTLFormula::readFormule("Fr&G(r>!d)"));
	auto fexp = fs.fullExpansion();


	std::cout << std::endl << std::endl << std::endl << "FULL EXP : "<< std::endl<< std::endl;
	for (const auto& fexpEntry : fexp)
	{
		std::cout << fexpEntry.operator std::string() << std::endl << std::endl;

	}

	VariableOp a('a', false);
	VariableOp b('a', true);
	
	std::cout << a.operator std::string() << "    " << b.neg()->operator std::string() << "   " << (a==*b.neg()) << std::endl;

	std::cout << "Size : " << sizeof(A) << std::endl;
	A ab;
	getchar();
}