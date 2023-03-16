from enum import Enum
from abc import ABC
from abc import abstractmethod
from typing import List

class OperatorType (Enum):
    CONJONCTIVE = 1
    DISJONCTIVE = 2
    SUCCESSOR = 3
    VARIABLE = 4



class LTLFormule(ABC):

    @abstractmethod
    def getType(self) -> OperatorType:
        pass

    @abstractmethod
    def getComponents(self) -> List['LTLFormule'] :
        pass

    @abstractmethod
    def neg(self) -> 'LTLFormule':
        pass






