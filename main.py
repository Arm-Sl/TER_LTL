__author__ = 'Antonin'
__Filename = 'main'
__Creationdate__ = '21/03/2023'







from copy import deepcopy
from typing import List
from Model import Model, InterpretationFunction, Interpretation, PreTableau
from LTL_Formules import readFormule, LTLFormula, Variable
from FormulaSet import FormulaSet


litterals : List[Variable] = list()
formula = readFormule("Fr&G(r>!d)", litterals)

fct = InterpretationFunction(2)
fct.add(0, d=Interpretation.TRUE, r=Interpretation.UNKNOWN)
fct.add(1, d=Interpretation.UNKNOWN, r=Interpretation.UNKNOWN)

model = Model(2, [[1],[0,1]], fct)
tab = PreTableau(model, deepcopy(formula), deepcopy(litterals))
tab.createInitialState()
tab.preTableauComputation()


print(len(tab.preStates), "    ", len(tab.states))
print("1 : ")
print(tab.rootState.children[0].formulas)
print("fe : ")
print(tab.rootState.children[0].formulas.fullExpansion())
print("2 : ")
print(tab.rootState.children[0].children[0].formulas)
print("3 : ")
print(tab.rootState.children[0].children[0].children[0].formulas)
print("6 : ")
print(tab.rootState.children[0].children[0].children[0].children[0].formulas)
print("7 : ")
print(tab.rootState.children[0].children[0].children[0].children[1].formulas)

#print(tab.rootState.children[0].children[0].children[0].children[1].formulas)
#print(tab.rootState.children[0].children[0].children[0].children[1].children[1].id)

print("==================")

s3 = tab.rootState.children[0].children[0].children[0]
s7 = tab.rootState.children[0].children[0].children[0].children[1]
print(s3.id)
print(s7.children[1].id)

# fs = FormulaSet(formula)
d = s7.formulas.fullExpansion()

print(len(d))
for a in d:
    print("\n" + str(a))


tab.show()
