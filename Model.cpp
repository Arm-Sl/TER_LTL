#include "Model.h"



#pragma region InterpretationFunction
InterpretationFunction::InterpretationFunction(int stateCount)
{
	this->interpretations.resize(stateCount);
}

Interpretation InterpretationFunction::get(int state, char name)
{
	auto it = this->interpretations[state].find(name);
	if (it != this->interpretations[state].end())
		return it->second;
	return Interpretation::UNKNOWN;
}

void InterpretationFunction::set(int state, char name, Interpretation i)
{
	if(i != Interpretation::UNKNOWN)
		this->interpretations[state].insert_or_assign(name, i);
}

bool InterpretationFunction::operator==(const InterpretationFunction & other) const
{
	return this->interpretations == other.interpretations;
}

InterpretationFunction::operator std::string() const
{
	std::string str;
	for (int i=0; i<this->interpretations.size(); i++)
	{
		str += "state " + std::to_string(i) + " : \n";
		for (const auto& pair : this->interpretations[i])
		{
			switch (pair.second)
			{
			case Interpretation::TRUE:
				str += std::string("\t") + pair.first + " : true";
				break;
			case Interpretation::FALSE:
				str += std::string("\t") + pair.first + " : false";
				break;
			case Interpretation::UNKNOWN:
				str += std::string("\t") + pair.first + " : unknowm";
				break;
			default:
				break;
			}
		}
	}

	return str;
}

InterpretationFunction::~InterpretationFunction()
{
}
#pragma endregion


#pragma region Transitions



Transitions::Transitions(int stateCount) : transitions(stateCount)
{
}


Transitions::~Transitions()
{
}

#pragma endregion



Model::Model(int stateCount) : 
	interpretationFunction(stateCount),
	transitions(stateCount)
{
}


Model::~Model()
{
}
