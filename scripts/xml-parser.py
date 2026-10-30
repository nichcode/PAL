#!/usr/bin/env python3

import pickle
from pathlib import Path
import xml.etree.ElementTree as ET
from typing import Dict, Optional

from models import (
    Constant,
    ConstantGroup,
    Field,
    Function,
    Module,
    Page,
    Param,
    Struct,
    Typedef,
    esc,
    make_token,
)

# Doxygen tag -> token key
DOXYGEN_TOKENS: Dict[str, str] = {
    "computeroutput": "code",
    "bold": "bold",
    "ref": "ref",
    "programlisting": "codeblock",
    "simplesect": "callout",
    "table": "table",
    "row": "row",
    "entry": "cell",
    "para": "para",
}

# Self-closing tags that stand for a character.
DOXYGEN_CHARS: Dict[str, str] = {
    "sp": " ",
    "linebreak": "\n",
}

# Dxoygen tags that are not used.
DOXYGEN_SKIP = {"parameterlist"}


class XMLParser:
    def __init__(self, root_dir: Path):
        self.xml_dir = root_dir / "build/xml"
        self._xml_cache: Dict[str, ET.Element] = {}

    def parse_function(self, memberdef: ET.Element) -> Function:
        """Parses a Doxygen function into a data object."""
        refid = memberdef.attrib.get("id", "")
        name = memberdef.findtext("name")
        return_type = self.convert_to_tokens(memberdef.find("type"))

        brief = self.convert_to_tokens(memberdef.find("briefdescription"))
        description = self.convert_to_tokens(
            memberdef.find("detaileddescription"))

        param_docs = {}
        for item in memberdef.findall(".//parameteritem"):
            doc = self.convert_to_tokens(item.find("parameterdescription"))

            for pname in item.findall("parameternamelist/parametername"):
                if pname.text:
                    param_docs[pname.text] = (
                        pname.attrib.get("direction", "in"),
                        doc,
                    )

        params = []
        args_list = []

        for param in memberdef.findall("param"):
            param_type = self.convert_to_tokens(param.find("type"))
            param_name = param.findtext("declname") or ""

            direction, param_brief = param_docs.get(
                param_name, ("in", "")
            )
            params.append(
                Param(param_name, param_type, direction, param_brief))

            if param_name:
                args_list.append(f"{param_type} {param_name}")
            else:
                args_list.append(param_type)

        signature = f"{return_type} {name}({', '.join(args_list)})"

        return Function(
            refid,
            name,
            return_type,
            signature,
            brief,
            description,
            params,
        )

    def parse_struct(self, inner_class: ET.Element) -> Struct:
        """Parses a Doxygen struct into a data object."""
        refid = inner_class.get("refid")
        compounddef = self._load_compound(refid)
        name = compounddef.findtext("compoundname")

        brief = self.convert_to_tokens(compounddef.find("briefdescription"))
        description = self.convert_to_tokens(
            compounddef.find("detaileddescription")
        )

        fields = []
        for field in compounddef.findall(
            ".//memberdef[@kind='variable']"
        ):
            field_name = field.findtext("name")
            field_type = self.convert_to_tokens(field.find("type"))
            argsstring = field.findtext("argsstring") or ""

            field_description = self.convert_to_tokens(
                field.find("detaileddescription")
            )

            fields.append(Field(
                field_name,
                field_type,
                field_description,
                argsstring,
            ))

        return Struct(
            refid,
            name,
            brief,
            description,
            fields,
        )

    def parse_typedef(self, memberdef: ET.Element) -> Typedef:
        """Parses a Doxygen typedef into a data object."""
        refid = memberdef.attrib.get("id", "")
        name = memberdef.findtext("name")
        type_ = self.convert_to_tokens(memberdef.find("type"))

        brief = self.convert_to_tokens(memberdef.find("briefdescription"))
        description = self.convert_to_tokens(
            memberdef.find("detaileddescription")
        )

        return Typedef(refid, name, type_, brief, description)

    def parse_constant(self, memberdef: ET.Element) -> Constant:
        """Parses a Doxygen constant into a data object."""
        refid = memberdef.attrib.get("id", "")
        name = memberdef.findtext("name")
        value = memberdef.findtext("initializer") or ""

        description = self.convert_to_tokens(
            memberdef.find("detaileddescription")
        )

        return Constant(refid, name, value, description)

    def parse_constant_group(self, inner_group: ET.Element) -> ConstantGroup:
        """Parses a Doxygen group/constant group into a data object."""
        refid = inner_group.get("refid")
        compounddef = self._load_compound(refid)
        name = compounddef.findtext("title")

        brief = self.convert_to_tokens(compounddef.find("briefdescription"))
        description = self.convert_to_tokens(
            compounddef.find("detaileddescription")
        )

        constants = []
        for memberdef in compounddef.findall(".//memberdef"):
            if memberdef.get("kind") == "define":
                constants.append(self.parse_constant(memberdef))

        return ConstantGroup(
            refid,
            name,
            brief,
            description,
            constants,
        )

    def parse_module(self, refid: str) -> Module:
        """Parses a Doxygen group/module into a Module data object."""
        compounddef = self._load_compound(refid)

        name = compounddef.findtext("title")
        brief = self.convert_to_tokens(compounddef.find("briefdescription"))
        description = self.convert_to_tokens(
            compounddef.find("detaileddescription")
        )

        functions = []
        structs = []
        typedefs = []
        constants = []
        constant_groups = []

        for memberdef in compounddef.findall(".//memberdef"):
            kind = memberdef.get("kind")

            if kind == "function":
                functions.append(self.parse_function(memberdef))
            elif kind == "typedef":
                typedefs.append(self.parse_typedef(memberdef))
            elif kind == "define":
                constants.append(self.parse_constant(memberdef))

        for inner_class in compounddef.findall(".//innerclass"):
            structs.append(self.parse_struct(inner_class))

        for inner_group in compounddef.findall(".//innergroup"):
            constant_groups.append(
                self.parse_constant_group(inner_group)
            )

        return Module(
            refid,
            name,
            brief,
            description,
            functions,
            structs,
            typedefs,
            constants,
            constant_groups,
        )

    def parse_page(self, refid: str) -> Page:
        """Parses a Doxygen page into a Page data object."""
        compounddef = self._load_compound(refid)
        title = compounddef.findtext("title")
        brief = self.convert_to_tokens(
            compounddef.find("briefdescription")
        )

        description = self.convert_to_tokens(
            compounddef.find("detaileddescription")
        )

        return Page(refid, title, brief, description)

    def _load_xml(self, refid: str) -> ET.Element:
        """Loads and caches a Doxygen compound XML document."""
        if refid not in self._xml_cache:
            self._xml_cache[refid] = ET.parse(
                self.xml_dir / f"{refid}.xml"
            ).getroot()

        return self._xml_cache[refid]

    def _load_compound(self, refid: str) -> ET.Element:
        """Returns the compound definition for a Doxygen XML document."""
        return self._load_xml(refid).find("compounddef")

    def convert_to_tokens(self, element: Optional[ET.Element], strip: bool = True) -> str:
        """Recursively converts Doxygen XML elements into tokens, optionally trimming outer whitespace.."""
        if element is None:
            return ""

        parts = []
        if element.text:
            parts.append(esc(element.text))

        for child in element:
            tag = child.tag

            if tag in DOXYGEN_CHARS:
                parts.append(DOXYGEN_CHARS[tag])
            elif tag not in DOXYGEN_SKIP:
                if tag in DOXYGEN_TOKENS:
                    token = DOXYGEN_TOKENS[tag]
                    content = self.convert_to_tokens(child, False)

                    if token == "ref":
                        parts.append(make_token(
                            "ref",
                            target_id=child.attrib.get("refid", ""),
                            content=content))

                    elif token == "callout":
                        parts.append(make_token(
                            "callout",
                            type=child.attrib.get("kind", "note"),
                            content=content))

                    else:
                        parts.append(make_token(token, content=content))
                else:
                    parts.append(self.convert_to_tokens(child, False))

            if child.tail:
                parts.append(esc(child.tail))

        parts_string = "".join(parts)
        if strip:
            return parts_string.strip()
        else:
            return parts_string


def find_compounds(parser: XMLParser):
    """Finds top-level groups and all Doxygen pages from the XML index."""
    index = parser._load_xml("index")
    groups = []
    pages = []

    for compound in index.iterfind("compound"):
        kind = compound.get("kind")
        refid = compound.get("refid")

        if kind == "group":
            groups.append(refid)
        elif kind == "page":
            pages.append(refid)

    children = set()
    for refid in groups:
        compounddef = parser._load_compound(refid)

        for inner_group in compounddef.findall("innergroup"):
            children.add(inner_group.get("refid"))

    modules = [refid for refid in groups if refid not in children]
    return modules, pages


def save_cache(filepath: Path, data):
    """Saves data objects to disk using pickle"""
    with open(filepath, "wb") as file:
        pickle.dump(data, file, protocol=pickle.HIGHEST_PROTOCOL)


def main():
    root_dir = Path(__file__).resolve().parents[1]
    parser = XMLParser(root_dir)
    module_refids, page_refids = find_compounds(parser)

    module_objects = [
        parser.parse_module(refid) for refid in module_refids
    ]

    page_objects = [
        parser.parse_page(refid) for refid in page_refids
    ]

    save_cache(root_dir / "build/modules.pkl", module_objects)
    save_cache(root_dir / "build/pages.pkl", page_objects)

    print(f"xml files parsed successfully")


if __name__ == "__main__":
    main()
