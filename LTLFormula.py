from copy import deepcopy
from enum import Enum
from abc import ABC
from abc import abstractmethod
from typing import List, Text


class OperatorType (Enum):
    CONJONCTIVE = 1
    DISJONCTIVE = 2
    SUCCESSOR = 3
    VARIABLE = 4



class LTLFormula(ABC):

    @abstractmethod
    def getType(self) -> OperatorType:
        pass

    @abstractmethod
    def getComponents(self) -> List['LTLFormula'] :
        pass

    @abstractmethod
    def neg(self) -> 'LTLFormula':
        pass


class FormulaSet:
    formulas : List[LTLFormula] = []
    readIndex : int = 0

    def __init__(self, formula : LTLFormula = None):
        if(formula is not None):
            self.formulas.append(formula)


    def __deepcopy__(self, memodict):
        c = FormulaSet()
        c.formulas = deepcopy(self.formulas)
        c.readIndex = self.readIndex
        return c

    def fullExpansion(self) -> List['FormulaSet']:
        sets : List['FormulaSet'] = [self]
        while (self.readIndex < len(self.formulas)):
            if(self.formulas[self.readIndex].getType() == OperatorType.SUCCESSOR or self.formulas[self.readIndex ].getType() == OperatorType.VARIABLE):
                self.readIndex +=1
                continue

            if(self.formulas[self.readIndex ].getType() == OperatorType.CONJONCTIVE):
                self.formulas.extend(self.formulas[self.readIndex].getComponents())

            if(self.formulas[self.readIndex ].getType() == OperatorType.DISJONCTIVE):
                components = self.formulas[self.readIndex].getComponents()

                if(len(components) == 0):
                    self.readIndex += 1
                    continue

                for j in range(1, len(components)):
                    newFormula = deepcopy(self)
                    newFormula.formulas.append(components[j])
                    newFormula.readIndex += 1
                    sets.extend(newFormula.fullExpansion())

                self.formulas.append(components[0])

            self.readIndex += 1
        return sets


    def __str__(self) -> Text:
        strr : Text = ""
        for formula in self.formulas:
            strr += "\t" + str(formula) + "\n"
        return strr

