#include "LTLFormulaSet.h"

#include <iostream>

#include "LTLFormula.h"



LTLFormulaSet::LTLFormulaSet()
{
}


LTLFormulaSet::LTLFormulaSet(const LTLFormulaSet & fs)
{
	this->formulas.resize(fs.formulas.size());

	for (int i=0; i<this->formulas.size(); i++)
	{
		this->formulas[i] = fs.formulas[i]->copy();
	}

	this->readIndex = fs.readIndex;
}

LTLFormulaSet::LTLFormulaSet(LTLFormulaSet && fs)
{
	this->formulas = std::move(fs.formulas);
	this->readIndex = fs.readIndex;
}

LTLFormulaSet::LTLFormulaSet(std::unique_ptr<LTLFormula>&& formula)
{
	this->formulas.push_back(std::move(formula));
}


LTLFormulaSet & LTLFormulaSet::operator=(LTLFormulaSet && fs)
{
	this->formulas = std::move(fs.formulas);
	this->readIndex = fs.readIndex;
	return *this;
}

LTLFormulaSet::operator std::string() const
{
	std::string str;

	for (const auto& f : this->formulas)
		str += "\t" + f->operator std::string() + "\n";

	return str;
}

std::vector<LTLFormulaSet> LTLFormulaSet::fullExpansion() const
{
	std::vector<LTLFormulaSet> fullExpansion;
	fullExpansion.push_back(*this);


	while (fullExpansion[0].readIndex < fullExpansion[0].formulas.size())
	{
		std::vector<std::unique_ptr<LTLFormula>> comps;

		std::cout <<  fullExpansion[0].formulas[fullExpansion[0].readIndex]->operator std::string() << std::endl;

		switch (fullExpansion[0].formulas[fullExpansion[0].readIndex]->getOperatorType())
		{
		case OperatorType::CONJONCTIVE :

			fullExpansion[0].formulas[fullExpansion[0].readIndex]->getComponents(comps);
			for (auto& comp : comps)
			{
				if (std::find(fullExpansion[0].formulas.begin(), fullExpansion[0].formulas.end(), comp) == fullExpansion[0].formulas.end())
					fullExpansion[0].formulas.push_back(std::move(comp));
			}
			break;
		case OperatorType::DISJONCTIVE :
			fullExpansion[0].formulas[fullExpansion[0].readIndex]->getComponents(comps);
			
			for (int j=1; j<comps.size();j++)
			{
				LTLFormulaSet newSet(fullExpansion[0]);
				if (std::find(newSet.formulas.begin(), newSet.formulas.end(), comps[j]) == newSet.formulas.end())
					newSet.formulas.push_back(std::move(comps[j]));
				newSet.readIndex++;
				std::vector<LTLFormulaSet> fe = newSet.fullExpansion();
				fullExpansion.insert(fullExpansion.end(), std::make_move_iterator(fe.begin()), std::make_move_iterator(fe.end()));
			}
			if (std::find(fullExpansion[0].formulas.begin(), fullExpansion[0].formulas.end(), comps[0]) == fullExpansion[0].formulas.end())
				fullExpansion[0].formulas.push_back(std::move(comps[0]));
			break;
		case OperatorType::SUCCESSOR :
		case OperatorType::VARIABLE :
		default:
			break;
		}

		fullExpansion[0].readIndex++;
	}

	int i = 0;
	while (i < fullExpansion.size())
	{
		if (fullExpansion[i].isPatentlyInconsistent())
			fullExpansion.erase(fullExpansion.begin() + i);
		else
			i += 1;
	}
	return fullExpansion;
}


bool LTLFormulaSet::isPatentlyInconsistent() const
{
	for (int i = 0; i < this->formulas.size(); i++)
	{
		for (int j = 0; j < this->formulas.size(); j++)
		{
			if (i == j)
				continue;
			if (*this->formulas[i] == *this->formulas[j]->neg())
				return true;
		}
	}
	return false;
}



LTLFormulaSet::~LTLFormulaSet()
{
}
