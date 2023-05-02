#include <iostream>
#include "LTLFormula.h"
#include "LTLFormulaSet.h"
#include "Model.h"
#include "PreTableau.h"

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


int main(int argc, char *argv[])
{
	
	std::set<char> variableNames;
	auto s = LTLFormula::readFormule("(a&b)|c", variableNames);
	
	std::cout << s->operator std::string() << std::endl;


	LTLFormulaSet fs(LTLFormula::readFormule("Fr&G(r>!d)", variableNames));
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

	PreTableau preTab(&model, LTLFormula::readFormule("Fr&G(r>!d)", variableNames));
	preTab.preTableauComputation();

	std::cout << "PreTableau Computation finished" << std::endl;
	std::cout << "State count : " << preTab.states.size() << ", Prestate count : " << preTab.preStates.size() << "    " << PreTableauState::lastID  << std::endl;


	Tableau tab = preTab.convertToTableau();
	std::cout << "State count : " << tab.states.size() << std::endl;
	tab.tableauComputation();
	std::cout << "State count : " << tab.states.size() << std::endl;

	tab.print();

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
}