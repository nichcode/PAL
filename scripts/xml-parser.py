#!/usr/bin/env python3

import sys
import pickle
from pathlib import Path
import xml.etree.cElementTree as ET

from typing import Optional
from models import *


class XMLParser:
    def __init__(self, root_dir: str):
        self.xml_dir = root_dir / "build/xml"

    def convert_to_tokens(self, element: Optional[ET.Element]) -> str:
        """Recursively converts Doxygen XML elements into tokens"""
        if element is None:
            return ""

        doxygen_tokens: Dict[str, dict] = {
            "computeroutput":     "code",
            "bold":               "bold",
            "ref":                "ref",
            "programlisting":     "codeblock",
            "simplesect":         "callout",
            "table":              "table",
            "row":                "row",
            "entry":              "cell"
        }

        parts = []
        if element.text:
            parts.append(element.text)

        for child in element:
            tag = child.tag

            if tag in doxygen_tokens:
                token = doxygen_tokens[tag]
                content = self.convert_to_tokens(child)
                template = TOKENS[token]

                if token == "ref":
                    id = child.attrib.get("refid", "")
                    formatted = template.format(target_id=id, content=content)
                elif token == "callout":
                    callout_type = child.attrib.get("kind", "note")
                    formatted = template.format(
                        type=callout_type, content=content)
                else:
                    formatted = template.format(content=content)

                parts.append(formatted)
            else:
                parts.append(self.convert_to_tokens(child))

            if child.tail:
                parts.append(child.tail)

        return "".join(parts).strip()

    def parse_function(self, memberdef: ET.Element) -> Function:
        """Parses a Doxygen function into data object"""
        id = memberdef.attrib.get("id", "")
        name = memberdef.findtext("name")
        type_element = memberdef.find("type")
        return_type = self.convert_to_tokens(type_element)

        brief_element = memberdef.find("briefdescription")
        brief = self.convert_to_tokens(brief_element)

        detailed_element = memberdef.find("detaileddescription")
        description = self.convert_to_tokens(detailed_element)

        params = []
        args_list = []

        for param in memberdef.findall("param"):
            param_type_element = param.find("type")
            param_type = self.convert_to_tokens(param_type_element)
            param_name_element = param.find("declname")
            param_name = param_name_element.text if param_name_element is not None else ""

            direction = param.attrib.get("direction", "in")
            params.append(Param(param_name, param_type, direction, brief))

            if param_name:
                args_list.append(f"{param_type} {param_name}")
            else:
                args_list.append(param_type)

        signature = f"{return_type} {name}({", ".join(args_list)})"

        return Function(
            id,
            name,
            return_type,
            signature,
            brief,
            description,
            params)

    def parse_struct(self, inner_class: ET.Element) -> Struct:
        """Parses a Doxygen struct into data object"""
        refid = inner_class.get('refid')
        root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
        compounddef = root.find("compounddef")
        name = compounddef.findtext("compoundname"),

        brief_element = compounddef.find("briefdescription")
        brief = self.convert_to_tokens(brief_element)

        detailed_element = compounddef.find("detaileddescription")
        description = self.convert_to_tokens(detailed_element)

        fields = []
        for field in compounddef.findall(".//memberdef"):
            field_name = field.findtext("name")
            type_element = field.find("type")
            field_type = self.convert_to_tokens(type_element)
            argsstring = field.findtext("argsstring")

            field_detailed_element = field.find("detaileddescription")
            field_description = self.convert_to_tokens(field_detailed_element)

            fields.append(Field(field_name, field_type,
                          field_description, argsstring))

        return Struct(
            refid,
            name,
            brief,
            description,
            fields)

    def parse_typedef(self, memberdef: ET.Element) -> Typedef:
        """Parses a Doxygen typedef into data object"""
        id = memberdef.attrib.get("id", "")
        name = memberdef.findtext("name")
        type_element = memberdef.find("type")
        type_ = self.convert_to_tokens(type_element)

        brief_element = memberdef.find("briefdescription")
        brief = self.convert_to_tokens(brief_element)

        detailed_element = memberdef.find("detaileddescription")
        description = self.convert_to_tokens(detailed_element)

        return Typedef(
            id,
            name,
            type_,
            brief,
            description)

    def parse_constant(self, memberdef: ET.Element) -> Constant:
        """Parses a Doxygen constant into data object"""
        id = memberdef.attrib.get("id", "")
        name = memberdef.findtext("name")
        value = memberdef.findtext("initializer")

        detailed_element = memberdef.find("detaileddescription")
        description = self.convert_to_tokens(detailed_element)

        return Constant(
            id,
            name,
            value,
            description)

    def parse_constant_group(self, inner_group: ET.Element) -> ConstantGroup:
        """Parses a Doxygen group/constant group into data object"""
        refid = inner_group.get('refid')
        root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
        compounddef = root.find("compounddef")
        name = compounddef.findtext("title")

        brief_element = compounddef.find("briefdescription")
        brief = self.convert_to_tokens(brief_element)

        detailed_element = compounddef.find("detaileddescription")
        description = self.convert_to_tokens(detailed_element)

        constants = []
        for memberdef in compounddef.findall(".//memberdef"):
            kind = memberdef.attrib.get("kind", "")

            if kind == "define":
                constants.append(self.parse_constant(memberdef))

        return ConstantGroup(
            refid,
            name,
            brief,
            description,
            constants)

    def parse_module(self, refid: str) -> Module:
        """Parses a Doxygen group/module into Module data object"""
        root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
        compounddef = root.find("compounddef")

        if compounddef is None:
            print(f"Invalid Doxygen XML: missing <compounddef> in {refid}")
            sys.exit(1)

        title = compounddef.findtext("title")
        brief_element = compounddef.find("briefdescription")
        brief = self.convert_to_tokens(brief_element)

        detailed_element = compounddef.find("detaileddescription")
        description = self.convert_to_tokens(detailed_element)

        functions = []
        structs = []
        typedefs = []
        constants = []
        constant_groups = []

        for memberdef in compounddef.findall(".//memberdef"):
            kind = memberdef.attrib.get("kind", "")

            if kind == "function":
                functions.append(self.parse_function(memberdef))
            elif kind == "typedef":
                typedefs.append(self.parse_typedef(memberdef))
            elif kind == "define":
                constants.append(self.parse_constant(memberdef))

        for inner_class in compounddef.findall(".//innerclass"):
            structs.append(self.parse_struct(inner_class))

        for inner_group in compounddef.findall(".//innergroup"):
            constant_groups.append(self.parse_constant_group(inner_group))

        return Module(
            refid,
            title,
            brief,
            description,
            functions,
            structs,
            typedefs,
            constants,
            constant_groups)

    def parse_page(self, refid: str) -> Page:
        """Parses a Doxygen page into Page data object"""
        root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
        compounddef = root.find("compounddef")
        title = compounddef.findtext("title")
        brief = compounddef.findtext("briefdescription/para")

        detailed_element = compounddef.find("detaileddescription")
        description = self.convert_to_tokens(detailed_element)

        return Page(refid, title, brief, description)


def find_modules(root_dir) -> List[str]:
    """Finds Dxoygen modules from sub groups"""
    xml_file = root_dir / "build/xml/index.xml"
    root = ET.parse(xml_file).getroot()

    modules = []
    children = []

    for compound in root.iterfind("compound"):
        if compound.get("kind") == "group":
            refid = compound.get("refid")
            root = ET.parse(root_dir / f"build/xml/{refid}.xml").getroot()
            compounddef = root.find("compounddef")

            # find nested sub-groups
            for inner in compounddef.findall("innergroup"):
                group_refid = inner.get("refid")
                children.append(group_refid)

            # filter modules
            if refid not in children:
                modules.append(refid)

    return modules


def find_pages(root_dir) -> List[str]:
    """Finds Dxoygen modules from sub groups"""
    xml_file = root_dir / "build/xml/index.xml"
    root = ET.parse(xml_file).getroot()

    pages = []
    for compound in root.iterfind("compound"):
        if compound.get("kind") == "page":
            pages.append(compound.get("refid"))

    return pages


def save_cache(filepath: Path, data):
    """Saves data objects to disk using pickle"""
    with open(filepath, "wb") as file:
        pickle.dump(data, file, protocol=pickle.HIGHEST_PROTOCOL)


def main():
    root_dir = Path(__file__).resolve().parents[1]
    module_refids = find_modules(root_dir)
    pages = find_pages(root_dir)
    parser = XMLParser(root_dir)

    module_objects = []
    for refid in module_refids:
        module_objects.append(parser.parse_module(refid))

    page_objects = []
    for refid in pages:
        page_objects.append(parser.parse_page(refid))

    save_cache(root_dir / "build/modules.pkl", module_objects)
    save_cache(root_dir / "build/pages.pkl", page_objects)

    print(f"xml files parsed successfully")


if __name__ == "__main__":
    main()
