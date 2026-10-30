import re
from dataclasses import dataclass, field
from typing import Dict, List


# One template per token. <name> marks a field. The content field
# may hold anything (including other tokens)
TOKENS: Dict[str, str] = {
    "code": "{{code:<content>}}",
    "bold": "{{bold:<content>}}",
    "ref": "{{ref:<target_id>:<content>}}",
    "codeblock": "{{codeblock:<content>}}",
    "callout": "{{callout:<type>:<content>}}",
    "table": "{{table:<content>}}",
    "row": "{{row:<content>}}",
    "cell": "{{cell:<content>}}",
    "para": "{{para:<content>}}",
}

_PLACEHOLDER = re.compile(r"<(\w+)>")
_ESCAPE = re.compile(r"([\\{}])")
_UNESCAPE = re.compile(r"\\([\\{}])")
_ID_BODY = r"[^:{}\\]+"                 # ref target, callout type
_CONTENT_BODY = r"(?:[^{}\\]|\\.)*?"   # no bare braces: innermost token first


@dataclass(slots=True)
class Constant:
    id: str
    name: str
    value: str
    description: str = ""


@dataclass(slots=True)
class ConstantGroup:
    id: str
    name: str
    brief: str = ""
    description: str = ""
    constants: List[Constant] = field(default_factory=list)


@dataclass(slots=True)
class Param:
    name: str
    type: str
    direction: str = "in"
    brief: str = ""


@dataclass(slots=True)
class Field:
    name: str
    type: str
    description: str = ""
    argsstring: str = ""


@dataclass(slots=True)
class Function:
    id: str
    name: str
    return_type: str
    signature: str
    brief: str = ""
    description: str = ""
    params: List[Param] = field(default_factory=list)


@dataclass(slots=True)
class Struct:
    id: str
    name: str
    brief: str = ""
    description: str = ""
    fields: List[Field] = field(default_factory=list)


@dataclass(slots=True)
class Typedef:
    id: str
    name: str
    type: str
    brief: str = ""
    description: str = ""


@dataclass(slots=True)
class Module:
    id: str
    name: str
    brief: str = ""
    description: str = ""
    functions: List[Function] = field(default_factory=list)
    structs: List[Struct] = field(default_factory=list)
    typedefs: List[Typedef] = field(default_factory=list)
    constants: List[Constant] = field(default_factory=list)
    constant_groups: List[ConstantGroup] = field(default_factory=list)


@dataclass(slots=True)
class Page:
    id: str
    title: str
    brief: str = ""
    description: str = ""


def esc(text: str) -> str:
    """Escapes raw text so it can never be mistaken for token syntax."""
    return _ESCAPE.sub(r"\\\1", text)


def unesc(text: str) -> str:
    """Undoes esc(). Renderers call this last, after all tokens are replaced."""
    return _UNESCAPE.sub(r"\1", text)


def make_token(key: str, **values: str) -> str:
    """Builds a token. Content is passed ready-made (escaped text and
    inner tokens); the other fields are escaped here."""
    template = TOKENS[key]
    names = _PLACEHOLDER.findall(template)

    if set(values) != set(names):
        raise ValueError(
            f"token '{key}' needs {names}, got {list(values)}"
        )

    content = names[-1]

    def fill(match: re.Match) -> str:
        name = match.group(1)
        return values[name] if name == content else esc(values[name])

    return _PLACEHOLDER.sub(fill, template)


def token_pattern(key: str) -> re.Pattern:
    """Returns a regex for one token, with a named group per field."""
    template = TOKENS[key]
    names = _PLACEHOLDER.findall(template)
    out = []
    pos = 0

    for match in _PLACEHOLDER.finditer(template):
        out.append(re.escape(template[pos:match.start()]))
        body = (
            _CONTENT_BODY
            if match.group(1) == names[-1]
            else _ID_BODY
        )

        out.append(f"(?P<{match.group(1)}>{body})")
        pos = match.end()

    out.append(re.escape(template[pos:]))
    return re.compile("".join(out), re.S)
