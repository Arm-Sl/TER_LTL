#include <iostream>
#include "LTLFormula.h"
#include "LTLFormulaSet.h"
#include "Model.h"
#include "PreTableau.h"
#include "Solver.h"

class A
{
public:
	A() {};
	A(const A& a) = delete; //copy constructor
	A(A&& a) {}; //move constructor

	~A() {};
	void feur() {};
private:
	int* c;
};


void printHelp()
{
	std::cout << "Partial model checking and partial model synthesis in LTL\n\
Help : args, in order : \n\
\t- model file location \n\
\t- LTL fomula, with operators \n\
\t\t- G : Globally \n\
\t\t- F : Finally \n\
\t\t- N : Next \n\
\t\t- U : Until\n\
\t\t- R : Release\n\
\t\t- ! : not\n\
\t\t- & : logical and\n\
\t\t- | : logical or\n\
\t\t- > : implies\n\
\t- problem to solve, in : \n\
\t\t- EE\n\
\t\t- AA\n\
\t-optional : v for verbose";

	getchar();
}

int main(int argc, char *argv[])
{
	if (argc != 4 && argc != 5)
	{
		printHelp();
		return 0;
	}
	Model model(argv[1], false);

	//Solver solver(&model, LTLFormula::readFormule("Fr&G(r>!d)"));
	Solver solver(&model, LTLFormula::readFormule(argv[2]));

	std::string problem(argv[3]);
	if (problem == "EE")
	{
		bool res = solver.solveEE(argc == 5);

		std::cout << "\n\n Result to EE problem : " << (res ? "True" : "False") << std::endl;
	}
	else if (problem == "AA")
	{
		bool res = solver.solveAA(argc == 5);

		std::cout << "\n\n Result to AA problem : " << (res ? "True" : "False") << std::endl;

	}

	getchar();

	return 0;
}

int mainTest(int argc, char *argv[])
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

	std::cout << "PRETABLEAU : " << sizeof(A) << std::endl;
	Model model(2);
	model.setSuccessors(0, { 1 });
	model.setSuccessors(1, { 0, 1 });
	model.setInt(0, 'd', Interpretation::TRUE);

	std::cout << model.operator std::string() << std::endl;

	std::unique_ptr<LTLFormula> formula = LTLFormula::readFormule("Fr&G(r>!d)");
	PreTableau preTab(&model, formula.get());
	preTab.preTableauComputation();

	std::cout << "PreTableau Computation finished" << std::endl;
	std::cout << "State count : " << preTab.states.size() << ", Prestate count : " << preTab.preStates.size() << "    " << PreTableauState::lastID  << std::endl;


	Tableau tab = preTab.convertToTableau();
	std::cout << "State count : " << tab.getStateCount() << std::endl;
	tab.tableauComputation();
	std::cout << "State count : " << tab.getStateCount() << std::endl;

	std::cout << tab.operator std::string();

	//TableauState* s0 = tab.preStates[0].get();
	//std::cout << "0 : " << s0->getChildren().size() << "     " << s0->id << std::endl;
	//
	//TableauState* s1 = s0->getChildren()[0];
	//std::cout << "1 : " << s1->getChildren().size() << "     " << s1->id << std::endl;
	//
	//TableauState* s2 = s1->getChildren()[0];
	//std::cout << "2 : " << s2->getChildren().size() << "     " << s2->id << std::endl;
	//
	//TableauState* s3 = s2->getChildren()[0];
	//std::cout << "3 : " << s3->getChildren().size() << "     " << s3->id << std::endl;
	//
	//TableauState* s6 = s3->getChildren()[0];
	//std::cout << "6 : " << s6->getChildren().size() << "     " << s6->id << std::endl;
	//	
	//TableauState* s7 = s3->getChildren()[1];
	//std::cout << "7 : " << s7->getChildren().size() << "     " << s7->id <<"     " << s7->getChildren()[1]->id << std::endl;
	//
	//TableauState* s12 = s6->getChildren()[0];
	//std::cout << "12 : " << s12->getChildren().size() << "     " << s12->id << std::endl;
	//std::cout << "12C : " << s12->getChildren()[0]->id << std::endl;
	//
	//
	//TableauState* s4 = s2->getChildren()[1];
	//std::cout << "4 : " << s4->getChildren().size() << "     " << s4->id << std::endl;
	//
	//TableauState* s8 = s4->getChildren()[0];
	//std::cout << "8 : " << s8->getChildren().size() << "     " << s8->id  << "     " << s8->getChildren()[0]->id << std::endl;
	//
	//TableauState* s9 = s4->getChildren()[1];
	//std::cout << "9 : " << s9->getChildren().size() << "     " << s9->id  << "     " << s9->getChildren()[0]->id << "     " << s9->getChildren()[1]->id  << "     " << s9->getChildren()[2]->id << std::endl;
	//
	//TableauState* s16 = s9->getChildren()[1];
	//
	//std::cout << s4->getFormulas().operator std::string() << std::endl;
	//std::cout << s16->getFormulas().operator std::string() << std::endl;

	//std::cout << "Size : " << sizeof(A) << std::endl;
	//A ab;
	getchar();

	return 0;
}