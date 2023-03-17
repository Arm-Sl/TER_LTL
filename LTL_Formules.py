from copy import deepcopy

from LTLFormula import *
from typing import Text, ClassVar

class And(LTLFormula):
    comp1 : LTLFormula
    comp2 : LTLFormula

    def __init__(self, comp1 : LTLFormula, comp2 : LTLFormula):
        self.comp1 = comp1
        self.comp2 = comp2

    def getType(self) -> OperatorType:
        return OperatorType.CONJONCTIVE

    def getComponents(self) -> List[LTLFormula]:
        return [self.comp1, self.comp2]

    def neg(self) -> LTLFormula:
        return Or(self.comp1.neg(), self.comp2.neg())

    def __str__(self):
        return "(" + str(self.comp1) + ")" + "&" + "(" + str(self.comp2) + ")"

class Or(LTLFormula):
    comp1 : LTLFormula
    comp2 : LTLFormula

    def __init__(self, comp1 : LTLFormula, comp2 : LTLFormula):
        self.comp1 = comp1
        self.comp2 = comp2

    def getType(self) -> OperatorType:
        return OperatorType.DISJONCTIVE

    def getComponents(self) -> List[LTLFormula]:
        return [self.comp1, self.comp2]

    def neg(self) -> LTLFormula:
        return And(self.comp1.neg(), self.comp2.neg())

    def __str__(self):
         return "(" + str(self.comp1) + ")" + "|" + "(" + str(self.comp2) + ")"

class Implies(LTLFormula):
    comp1 : LTLFormula
    comp2 : LTLFormula

    def __init__(self, comp1 : LTLFormula, comp2 : LTLFormula):
        self.comp1 = comp1
        self.comp2 = comp2


    def getType(self) -> OperatorType:
        return OperatorType.DISJONCTIVE

    def getComponents(self) -> List[LTLFormula]:
        return [self.comp1.neg(), self.comp2]

    def neg(self) -> LTLFormula:
        return And(self.comp1, self.comp2.neg())

    def __str__(self):
        return "(" + str(self.comp1) + ")" + ">" + "(" + str(self.comp2) + ")"

class Next(LTLFormula):
    comp1 : LTLFormula

    def __init__(self, comp1 : LTLFormula):
        self.comp1 = comp1

    def getType(self) -> OperatorType:
        return OperatorType.SUCCESSOR

    def getComponents(self) -> List[LTLFormula]:
        return [self.comp1]

    def neg(self) -> LTLFormula:
        return self.comp1.neg()

    def __str__(self):
        return "N(" + str(self.comp1) + ")"

class Globally(LTLFormula):
    comp1 : LTLFormula

    def __init__(self, comp1 : LTLFormula):
        self.comp1 = comp1

    def getType(self) -> OperatorType:
        return OperatorType.CONJONCTIVE

    def getComponents(self) -> List[LTLFormula]:
        return [self.comp1, Next(self)]

    def neg(self) -> LTLFormula:
        return Finally(self.comp1.neg())

    def __str__(self):
        return "G(" + str(self.comp1) + ")"

class Finally(LTLFormula):
    comp1 : LTLFormula

    def __init__(self, comp1 : LTLFormula):
        self.comp1 = comp1

    def getType(self) -> OperatorType:
        return OperatorType.DISJONCTIVE

    def getComponents(self) -> List[LTLFormula]:
        return [self.comp1, Next(self)]

    def neg(self) -> LTLFormula:
        return Globally(self.comp1.neg())

    def __str__(self):
        return "F(" + str(self.comp1) + ")"

class Until(LTLFormula):
    comp1 : LTLFormula
    comp2 : LTLFormula

    def __init__(self, comp1 : LTLFormula, comp2 : LTLFormula):
        self.comp1 = comp1
        self.comp2 = comp2


    def getType(self) -> OperatorType:
        return OperatorType.DISJONCTIVE

    def getComponents(self) -> List[LTLFormula]:
        return [self.comp2, And(self.comp2, Next(self))]

    def neg(self) -> LTLFormula:
        return Release(self.comp1.neg(), self.comp2.neg())

    def __str__(self):
        return "(" + str(self.comp1) + ")" + "U" + "(" + str(self.comp2) + ")"

class Release(LTLFormula):
    comp1 : LTLFormula
    comp2 : LTLFormula

    def __init__(self, comp1 : LTLFormula, comp2 : LTLFormula):
        self.comp1 = comp1
        self.comp2 = comp2


    def getType(self) -> OperatorType:
        return OperatorType.CONJONCTIVE

    def getComponents(self) -> List[LTLFormula]:
        return [Or(self.comp1, Next(self)), self.comp2]

    def neg(self) -> LTLFormula:
        return Until(self.comp1.neg(), self.comp2.neg())

    def __str__(self):
        return "(" + str(self.comp1) + ")" + "R" + "(" + str(self.comp2) + ")"

class Variable(LTLFormula):
    _neg:bool
    name:str

    def __init__(self, name):
        self.name = name
        self._neg = False

    def getType(self) -> OperatorType:
        return OperatorType.VARIABLE

    def getComponents(self) -> List[LTLFormula]:
            return []

    def neg(self) -> LTLFormula:
        self._neg = not self._neg
        return self

    def __str__(self):
        return ("!" if self._neg else "") + self.name


def readFormule(s : Text) -> LTLFormula:

    def getOperatorFromString(op : Text) -> ClassVar[LTLFormula]:
        if(op == "G") : return Globally
        elif(op == "F") : return Finally
        elif(op == "N") : return Next
        elif(op == "U") : return Until
        elif(op == "R") : return Release
        elif(op == "&") : return And
        elif(op == "|") : return Or
        elif(op == ">") : return Implies

    unary = ["G", "F", "N", "!"]
    binaryOrder = [ "U", "R", "&", "|", ">"]
    maxOp = -1
    maxOpIndexB = -1

    parentheseDeepness = 0
    hasParenthese = False


    for i in range(len(s)):
        if(s[i] == "("):
            parentheseDeepness += 1
            hasParenthese = True
            continue
        if(s[i] == ")"):
            parentheseDeepness -= 1
            continue
        if(parentheseDeepness == 0 and s[i] in binaryOrder):
            idx = binaryOrder.index(s[i])

            if(idx >= maxOpIndexB):
                maxOp = i
                maxOpIndexB = idx

    if(maxOpIndexB < 0):
        for i in range(len(s)):
            if(s[i] in unary):
                if(s[i] == "!"):
                    return readFormule(s[i+1:]).neg()
                else:
                    return getOperatorFromString(s[i])(readFormule(s[i+1:]))
        if(hasParenthese):
            return readFormule(s[s.find('(')+1:s.rfind(')')])
        else:
            return Variable(s)
    else :
        return getOperatorFromString(s[maxOp])(readFormule(s[:maxOp]), readFormule(s[maxOp+1:]))




formula = readFormule("(a&(b|c))")
fs = FormulaSet(formula)
d = fs.fullExpansion()

for a in d:
    print("\n" + str(a))








