#include "LTLFormulaSet.h"

#include <iostream>

#include "LTLFormula.h"



LTLFormulaSet::LTLFormulaSet()
{
}

LTLFormulaSet::~LTLFormulaSet()
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



void LTLFormulaSet::addFormula(std::unique_ptr<LTLFormula>&& formula)
{
	this->formulas.push_back(std::move(formula));
}


std::unique_ptr<LTLFormula>* LTLFormulaSet::addFormulas(size_t size)
{
	size_t oldSize = this->formulas.size();

	this->formulas.resize(oldSize + size);
	return &this->formulas[oldSize];
}

bool LTLFormulaSet::foundInFormula(const std::vector<std::unique_ptr<LTLFormula>>& vec, const std::unique_ptr<LTLFormula>& ptr) const
{
	for (int i = 0; i < vec.size(); i++)
	{
		if (*vec[i] == *ptr)
			return true;
	}
	return false;
}


std::vector<LTLFormulaSet> LTLFormulaSet::fullExpansion() const
{

	//TODO use parameter instead of return value
	std::vector<LTLFormulaSet> fullExpansion;
	fullExpansion.push_back(*this);


	while (fullExpansion[0].readIndex < fullExpansion[0].formulas.size())
	{
		switch (fullExpansion[0].formulas[fullExpansion[0].readIndex]->getOperatorType())
		{
		case OperatorType::CONJONCTIVE :
		{
			std::vector<std::unique_ptr<LTLFormula>> comps = fullExpansion[0].formulas[fullExpansion[0].readIndex]->getComponents();
			for (auto& comp : comps)
			{
				if(!this->foundInFormula(fullExpansion[0].formulas, comp))
					fullExpansion[0].formulas.push_back(std::move(comp));
			}
			break;
		}
		case OperatorType::DISJONCTIVE :
		{
			std::vector<std::unique_ptr<LTLFormula>> comps = fullExpansion[0].formulas[fullExpansion[0].readIndex]->getComponents();

			for (int j = 1; j < comps.size(); j++)
			{
				LTLFormulaSet newSet(fullExpansion[0]);
				if (!this->foundInFormula(newSet.formulas, comps[j]))
					newSet.formulas.push_back(std::move(comps[j]));
				newSet.readIndex++;
				std::vector<LTLFormulaSet> fe = newSet.fullExpansion();
				fullExpansion.insert(fullExpansion.end(), std::make_move_iterator(fe.begin()), std::make_move_iterator(fe.end()));
			}
			if (!this->foundInFormula(fullExpansion[0].formulas, comps[0]))
				fullExpansion[0].formulas.push_back(std::move(comps[0]));
			break;
		}
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


std::vector<VariableOp*> LTLFormulaSet::getLitterals() const
{
	std::vector<VariableOp*> res;

	for (const auto& formula : this->formulas)
	{
		if (formula->getOperatorType() == OperatorType::VARIABLE)
			res.push_back(static_cast<VariableOp*>(formula.get()));
	}

	return res;
}



LTLFormulaSet::operator std::string() const
{
	std::string str;

	for (const auto& f : this->formulas)
		str += "\t" + f->operator std::string() + "\n";

	return str;
}


bool LTLFormulaSet::operator==(const LTLFormulaSet& other) const
{
	if (this->formulas.size() != other.formulas.size())
		return false;

	for (int i = 0; i < this->formulas.size(); i++)
	{
		bool found = false;
		for (int j = 0; j < other.formulas.size(); j++)
		{
			if (*this->formulas[i] == *other.formulas[j])
			{
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}
	return true;
}





std::vector<std::unique_ptr<LTLFormula>>::const_iterator LTLFormulaSet::begin() const
{
	return this->formulas.begin();
}

std::vector<std::unique_ptr<LTLFormula>>::const_iterator LTLFormulaSet::end() const
{
	return this->formulas.end();
}
