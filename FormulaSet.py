__author__ = 'Antonin'
__Filename = 'FormulaSet'
__Creationdate__ = '22/03/2023'

from LTL_Formules import Variable
from typing import List, Text, cast
from LTLFormula import LTLFormula, OperatorType
from copy import deepcopy



class FormulaSet:

    def __init__(self, *formula : LTLFormula):
        self.readIndex : int = 0
        self.formulas : List[LTLFormula] = []
        for f in formula:
            self.formulas.append(f)


    def __deepcopy__(self, memodict):
        c = FormulaSet()
        c.formulas = deepcopy(self.formulas)
        c.readIndex = self.readIndex
        return c

    def getLitterals(self) -> List[Variable]:
        variables : List[Variable] = list()

        for formula in self.formulas:
            if(formula.getType() == OperatorType.VARIABLE):
                variables.append(cast(Variable, formula))
        return variables


    def fullExpansion(self) -> List['FormulaSet']:
        sets : List['FormulaSet'] = [self]


        while (self.readIndex < len(self.formulas)):

            if(self.formulas[self.readIndex].getType() == OperatorType.CONJONCTIVE):
                comps = self.formulas[self.readIndex].getComponents()
                for i in range(len(comps)):
                    if(not comps[i] in self.formulas):
                        self.formulas.append(comps[i])

            if(self.formulas[self.readIndex ].getType() == OperatorType.DISJONCTIVE):
                components = self.formulas[self.readIndex].getComponents()

                if(len(components) == 0):
                    self.readIndex += 1
                    continue

                # componentAlreadyPresent = False
                # for j in range(0, len(components)):
                #     if(components[j] in self.formulas):
                #         componentAlreadyPresent = True
                # if(componentAlreadyPresent):
                #     self.readIndex += 1
                #     continue

                for j in range(1, len(components)):
                    newFormula = deepcopy(self)
                    if(not components[j] in newFormula.formulas):
                        newFormula.formulas.append(components[j])
                    newFormula.readIndex += 1
                    sets.extend(newFormula.fullExpansion())
                if(not components[0] in self.formulas):
                    self.formulas.append(components[0])

            self.readIndex += 1

        i = 0
        while(i < len(sets)):
            if(sets[i].isPatentlyInconsistent()):
                sets.pop(i)
            else:
                i+= 1
        return sets


    def __str__(self) -> Text:
        strr : Text = ""
        for formula in self.formulas:
            strr += "\t" + str(formula) + "\n"
        return strr


    def __eq__(self, other):
        if(not isinstance(other, FormulaSet)):
            return False

        if(len(self.formulas) != len(other.formulas)):
            return False

        for a in self.formulas:
            if(not a in other.formulas):
                return False

        return True

    def isPatentlyInconsistent(self) -> bool:

        for i in range(len(self.formulas)):
            for j in range(len(self.formulas)):
                if(i == j):
                    continue
                if(self.formulas[i] == deepcopy(self.formulas[j]).neg()):
                    return True

        return False

