#include "Model.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cctype>



#pragma region InterpretationFunction

InterpretationFunction::InterpretationFunction()
{
}

InterpretationFunction::InterpretationFunction(int stateCount)
{
	this->interpretations.resize(stateCount);
}

void InterpretationFunction::setStateCount(int stateCount)
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
		if (i != 0)
			str += "\n";
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

Transitions::Transitions()
{
}

Transitions::Transitions(int stateCount) : transitions(stateCount)
{
}


Transitions::~Transitions()
{
}

void Transitions::setStateCount(int stateCount)
{
	this->transitions.resize(stateCount);
}

Transitions::operator std::string() const
{
	std::string str;

	for (int i = 0; i < this->transitions.size(); i++)
	{
		str += "state " + std::to_string(i) + " : ";

		for (const auto succ : this->transitions[i])
			str += std::to_string(succ) + " ";
		
		str += "\n";
	}


	return str;
}

#pragma endregion



Model::Model(int stateCount) : 
	interpretationFunction(stateCount),
	transitions(stateCount)
{
}


Model::Model(const std::string & filepath, bool relative)
{
	auto path = relative ? std::filesystem::current_path().append(filepath) : filepath;

	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cout << "Error while reading file : " << path << std::endl;
		return;
	}

	std::cout << "Reading Model in file : " << path << std::endl;


	std::string line;

	std::vector<char> variables;
	std::vector<std::vector<int>> aTrans;
	std::vector<std::vector<Interpretation>> aInters;


	//read variables names
	if(!std::getline(file, line))
	{
		std::cout << "Error while reading file : " << path << std::endl;
		return;
	}
	for (char c : line)
	{
		if (isalpha(c))
			variables.push_back(c);
		if (c == '%')
			break;
	}




	//read states : 
	int currentState = 0;
	while (std::getline(file, line))
	{
		int currentVariable = 0;
		std::vector<Interpretation> inters(variables.size());
		std::vector<int> trans;
		std::string currentSuccessorIndex;
		bool wasDigit = false;
		for (char c : line)
		{
			bool nextLine = false;

			if (currentVariable < variables.size())
			{
				switch (c)
				{
				case 'T' :
					inters[currentVariable] = Interpretation::TRUE;
					currentVariable++;
					break;
				case 'F' :
					inters[currentVariable] = Interpretation::FALSE;
					currentVariable++;
					break;
				case '?' :
					inters[currentVariable] = Interpretation::UNKNOWN;
					currentVariable++;
					break;
				case '%':
					nextLine = true;
					return;
				default:
					if(isdigit(c))
						nextLine = true;
					break;
				}


				if (nextLine)
					break;
			}
			else
			{
				if (c == '%')
					break;
				if (isdigit(c))
				{
					if (!wasDigit)
					{
						currentSuccessorIndex.clear();
						wasDigit = true;
					}
					currentSuccessorIndex += c;
				}
				else
				{
					if (wasDigit)
					{
						try
						{
							int sucessor = std::stoi(currentSuccessorIndex);
							trans.push_back(sucessor);
							wasDigit = false;
						}
						catch (const std::exception&)
						{
							std::cout << "Error while reading sucessor index for state " << currentState << std::endl;
							return;
						}
					}
				}
			}

		}
		if (currentVariable == 0)
			continue;
		if (currentVariable < variables.size())
		{

			std::cout << "Error while reading file : " << path << "\n\t not enought variable values set" << std::endl;
			return;
		}

		std::cout << "State " << currentState << " has " << trans.size() << " transitions" << std::endl;
		currentState++;

		aTrans.push_back(std::move(trans));
		aInters.push_back(std::move(inters));
	}

	std::cout << "Model has " << aTrans.size() << " states" << std::endl << std::endl;

	this->transitions.setStateCount(static_cast<int>(aTrans.size()));
	this->interpretationFunction.setStateCount(static_cast<int>(aTrans.size()));


	for (int i = 0; i < aTrans.size(); i++)
	{
		this->transitions.setSuccessors(i, std::move(aTrans[i]));

		for (int interIndex=0; interIndex < aInters[i].size(); interIndex++)
		{
			if (aInters[i][interIndex] != Interpretation::UNKNOWN)
				this->interpretationFunction.set(i, variables[interIndex], aInters[i][interIndex]);
		}
	}

}

Model::~Model()
{
}

Model::operator std::string() const
{
	std::string str;
	str += "\n\nModel with " + std::to_string(this->getStateCount()) + " states\n\nTransitions : \n";
	str += this->transitions.operator std::string();
	str += "\nInterpretation Function : \n";
	str += this->interpretationFunction.operator std::string();
	str += "\n\n\n";
	return str;
}

