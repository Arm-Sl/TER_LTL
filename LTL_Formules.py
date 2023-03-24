from copy import deepcopy, copy

from LTLFormula import *
from typing import Text, ClassVar, Any


class And(LTLFormula):

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

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, And) and (self.comp1 == other.comp1 and self.comp2 == other.comp2)


class Or(LTLFormula):

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

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Or) and (self.comp1 == other.comp1 and self.comp2 == other.comp2)

class Implies(LTLFormula):

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

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Implies) and (self.comp1 == other.comp1 and self.comp2 == other.comp2)

class Next(LTLFormula):

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

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Next) and (self.comp1 == other.comp1)

class Globally(LTLFormula):

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

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Globally) and (self.comp1 == other.comp1)

class Finally(LTLFormula):

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

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Finally) and (self.comp1 == other.comp1)

class Until(LTLFormula):

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

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Until) and (self.comp1 == other.comp1 and self.comp2 == other.comp2)

class Release(LTLFormula):

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

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Release) and (self.comp1 == other.comp1 and self.comp2 == other.comp2)

class Variable(LTLFormula):


    def __init__(self, name, isNeg = False):
        self.name = name
        self.isNeg = isNeg

    def getType(self) -> OperatorType:
        return OperatorType.VARIABLE

    def getComponents(self) -> List[LTLFormula]:
            return []

    def neg(self) -> LTLFormula:
        cpy = Variable(self.name)
        cpy.isNeg = not self.isNeg
        return cpy

    def __str__(self):
        return ("!" if self.isNeg else "") + self.name

    def __eq__(self, other : Any) -> bool:
        return isinstance(other, Variable) and (self.name == other.name and self.isNeg == other.isNeg)


def readFormule(s : Text, litterals : List[Variable]) -> LTLFormula:

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
    firstUnaryIndex = -1

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
        if(parentheseDeepness == 0 and s[i] in unary and firstUnaryIndex < 0):
            firstUnaryIndex = i

    if(maxOpIndexB >= 0):
        return getOperatorFromString(s[maxOp])(readFormule(s[:maxOp], litterals), readFormule(s[maxOp+1:], litterals))
    if(firstUnaryIndex >= 0):
        if(s[firstUnaryIndex] == "!"):
            return readFormule(s[firstUnaryIndex+1:], litterals).neg()
        else:
            return getOperatorFromString(s[firstUnaryIndex])(readFormule(s[firstUnaryIndex+1:], litterals))
    if(hasParenthese):
        return readFormule(s[s.find('(')+1:s.rfind(')')], litterals)
    else:
        v = Variable(s)
        if(v not in litterals):
            litterals.append(v)
            litterals.append(Variable(s, True))
        return v









