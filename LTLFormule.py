from enum import Enum
from abc import ABC
from abc import abstractmethod
from typing import List, Self

class OperatorType (Enum):
    CONJONCTIVE = 1
    DISJONCTIVE = 2
    SUCCESSOR = 3



class LTLFormule(ABC):

    @abstractmethod
    def getType(self) -> OperatorType:
        pass

    @abstractmethod
    def getComponents(self) -> List[Self] :
        pass

    @abstractmethod
    def neg(self) -> Self:
        pass

