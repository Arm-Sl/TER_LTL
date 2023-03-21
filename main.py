__author__ = 'Antonin'

from copy import deepcopy

__Filename = 'main'
__Creationdate__ = '21/03/2023'





from typing import List
from Model import Model, InterpretationFunction, Interpretation, Tableau
from LTL_Formules import readFormule, LTLFormula, FormulaSet, Variable


litterals : List[Variable] = list()
formula = readFormule("Fr&G(r>!d)", litterals)


fct = InterpretationFunction(2)
fct.add(0, d=Interpretation.TRUE, r=Interpretation.UNKNOWN)
fct.add(1, d=Interpretation.UNKNOWN, r=Interpretation.UNKNOWN)

model = Model(2, [[1],[0,1]], fct)
tab = Tableau(model, deepcopy(formula), deepcopy(litterals))
tab.createInitialState()



# fs = FormulaSet(formula)
# d = fs.fullExpansion()
#
# print(len(d))
# for a in d:
#     print("\n" + str(a))