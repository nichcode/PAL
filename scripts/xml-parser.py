#!/usr/bin/env python3

import re
import os
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
    "ulink": "link",
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
        refid = memberdef.get("id", "")
        name = memberdef.findtext("name") or ""
        return_type = self._convert_to_tokens(memberdef.find("type"))
        if "*" in return_type:
            return_type = return_type.replace(" *", "* ")
        else:
            return_type += " "

        filepath = self._get_filepath(memberdef)
        brief = self._convert_to_tokens(memberdef.find("briefdescription"))
        description = self._convert_to_tokens(memberdef.find("detaileddescription"))

        # cut description from callout
        match = re.search(r"^(.*?)(\{\{callout:.*)", description, re.DOTALL)
        detailed = match.group(1).strip()
        description = match.group(2).strip()

        if detailed.endswith("{{para:"):
            detailed = detailed[:-7].strip()

        if description.endswith("}}"):
            description = description[:-2].strip()

        # Extract parameter documentation mapping
        param_docs = {}
        for item in memberdef.findall(".//parameteritem"):
            doc = self._convert_to_tokens(item.find("parameterdescription"))
            for pname in item.findall("parameternamelist/parametername"):
                name_text = pname.text.strip() if pname.text else ""
                if name_text:
                    param_docs[name_text] = (pname.get("direction", "in"), doc)

        params = []
        args_list = []

        for param in memberdef.findall("param"):
            param_type = self._convert_to_tokens(param.find("type"))
            param_name = (param.findtext("declname") or "").strip()

            direction, param_brief = param_docs.get(param_name, ("in", ""))
            params.append(Param(param_name, param_type, direction, param_brief))

            if param_name:
                full_arg = f"{param_type} {param_name}".replace(" *", "* ").replace("  ", " ")
                args_list.append(full_arg)
            else:
                args_list.append(param_type)

        args_string = ", ".join(args_list)

        return Function(
            refid,
            name,
            return_type,
            args_string,
            brief,
            detailed,
            description,
            filepath,
            params
        )

    def parse_struct(self, inner_class: ET.Element) -> Struct:
        """Parses a Doxygen struct into a data object."""
        refid = inner_class.get("refid")
        compounddef = self._load_compound(refid)
        name = compounddef.findtext("compoundname")
        filepath = self._get_filepath(compounddef)

        brief = self._convert_to_tokens(compounddef.find("briefdescription"))
        description = self._convert_to_tokens(
            compounddef.find("detaileddescription")
        )

        fields = []
        for field in compounddef.findall(
            ".//memberdef[@kind='variable']"
        ):
            field_name = field.findtext("name")
            field_type = self._convert_to_tokens(field.find("type"))
            argsstring = field.findtext("argsstring") or ""

            field_description = self._convert_to_tokens(
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
            filepath,
            fields,
        )

    def parse_typedef(self, memberdef: ET.Element) -> Typedef:
        """Parses a Doxygen typedef into a data object."""
        refid = memberdef.get("id")
        name = memberdef.findtext("name")
        type_ = self._convert_to_tokens(memberdef.find("type"))

        brief = self._convert_to_tokens(memberdef.find("briefdescription"))
        description = self._convert_to_tokens(
            memberdef.find("detaileddescription")
        )

        return Typedef(refid, name, type_, brief, description)

    def parse_constant(self, memberdef: ET.Element) -> Constant:
        """Parses a Doxygen constant into a data object."""
        refid = memberdef.get("id")
        name = memberdef.findtext("name")
        value = memberdef.findtext("initializer") or ""

        description = self._convert_to_tokens(
            memberdef.find("detaileddescription")
        )

        return Constant(refid, name, value, description)

    def parse_constant_group(self, inner_group: ET.Element) -> ConstantGroup:
        """Parses a Doxygen group/constant group into a data object."""
        refid = inner_group.get("refid")
        compounddef = self._load_compound(refid)
        name = compounddef.findtext("title")

        brief = self._convert_to_tokens(compounddef.find("briefdescription"))
        description = self._convert_to_tokens(
            compounddef.find("detaileddescription")
        )

        constants: list[Constant] = []
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

        filepath = f"pal2/{compounddef.findtext("compoundname")}.h"
        name = compounddef.findtext("title")
        brief = self._convert_to_tokens(compounddef.find("briefdescription"))
        description = self._convert_to_tokens(
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
            filepath,
            functions,
            structs,
            typedefs,
            constants,
            constant_groups,
        )

    def parse_page(self, refid: str) -> Page:
        """Parses a Doxygen page into a Page data object."""
        compounddef = self._load_compound(refid)
        name = compounddef.findtext("title")
        brief = self._convert_to_tokens(
            compounddef.find("briefdescription")
        )

        description = self._convert_to_tokens(
            compounddef.find("detaileddescription")
        )

        return Page(refid, name, brief, description)

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

    def _convert_to_tokens(self, element: Optional[ET.Element], strip: bool = True) -> str:
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
                    content = self._convert_to_tokens(child, False)

                    if token == "ref":
                        parts.append(make_token(
                            "ref",
                            target_id=child.get("refid"),
                            content=content))

                    elif token == "callout":
                        parts.append(make_token(
                            "callout",
                            type=child.get("kind", "note"),
                            content=content))

                    elif token == "link":
                        parts.append(make_token(
                            "link",
                            url=child.get("url"),
                            content=content))

                    else:
                        parts.append(make_token(token, content=content))
                else:
                    parts.append(self._convert_to_tokens(child, False))

            if child.tail:
                parts.append(esc(child.tail))

        parts_string = "".join(parts)
        if strip:
            return parts_string.strip()
        else:
            return parts_string

    def _get_filepath(self, element: ET.Element) -> str:
        filepath = element.find("location").get("file")
        base = os.path.normpath(filepath)
        filename = os.path.basename(base)
        parent_dir = os.path.dirname(base)
        last_folder = os.path.basename(parent_dir)

        # make relative path for sub module files
        if last_folder:
            return f"{last_folder}/{filename}"

        return filename


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
