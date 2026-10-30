
from dataclasses import dataclass, field
from typing import List, Dict

TOKENS: Dict[str, str] = {
    "code": "{{code:{content}}}",
    "bold": "{{bold:{content}}}",
    "ref": "{{ref:{target_id}:{content}}}",
    "codeblock": "{{codeblock:{content}}}",
    "callout": "{{callout:{type}:{content}}}",
    "table": "{{table:{content}}}",
    "row": "{{row:{content}}}",
    "cell": "{{cell:{content}}}",
}


@dataclass
class Constant:
    id: str
    name: str
    value: str
    description: str = ""


@dataclass
class ConstantGroup:
    id: str
    name: str
    brief: str = ""
    description: str = ""
    constants: List[Constant] = field(default_factory=list)


@dataclass
class Param:
    name: str
    type: str
    direction: str = "in"
    brief: str = ""


@dataclass
class Field:
    name: str
    type: str
    description: str = ""
    argsstring: str = ""


@dataclass
class Function:
    id: str
    name: str
    return_type: str
    signature: str
    brief: str = ""
    description: str = ""
    params: List[Param] = field(default_factory=list)


@dataclass
class Struct:
    id: str
    name: str
    brief: str
    description: str
    fields: List[Field] = field(default_factory=list)


@dataclass
class Typedef:
    id: str
    name: str
    type: str
    brief: str = ""
    description: str = ""


@dataclass
class Module:
    id: str
    title: str
    brief: str = ""
    description: str = ""
    functions: List[Function] = field(default_factory=list)
    structs: List[Struct] = field(default_factory=list)
    typedefs: List[Typedef] = field(default_factory=list)
    constants: List[Constant] = field(default_factory=list)
    constant_groups: List[ConstantGroup] = field(default_factory=list)


@dataclass
class Page:
    id: str
    title: str
    brief: str
    description: str
