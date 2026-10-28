#!/usr/bin/env python3

import html
from pathlib import Path
from xml.etree import ElementTree as ET
from models import *


class XMLParser:
    def __init__(self, root_dir):
        self.build_dir = root_dir / "build"
        self.xml_dir = self.build_dir / "xml"
        self.root = ET.parse(self.xml_dir / "index.xml").getroot()

    def parse(self):
        """Parses pages and modules"""

        self._parse_pages()

    def _xml_tags_to_html(self, element: ET.Element | None) -> str:
        """converts XML tags into HTML"""

        html_out = ""
        if element is None:
            return ""

        tag_map = {
            "para": ("<p>", "</p>"),
            "computeroutput": ("<code>", "</code>"),
            "c": ("<code>", "</code>"),
            "codeline": ("", "\n"),
            "highlight": ("", ""),

            "table": ("<table>", "</table>"),
            "row": ("<tr>", "</tr>"),
            "entry": ("<td>", "</td>"),
            "tablecols": ("", ""),

            "itemizedlist": ("<ul>", "</ul>"),
            "orderedlist": ("<ol>", "</ol>"),
            "listitem": ("<li>", "</li>"),

            "parameterlist": ("<div><h4>Parameters</h4><dl>", "</dl></div>"),
            "parameteritem": ("<div>", "</div>"),
            "parameternamelist": ("", ""), 
            "parameterdescription": ("<dd>", "</dd>"),
        }

        open_tag, close_tag = tag_map.get(element.tag, ("", ""))
        dir_badge = ""

        if element.tag == "parametername":
            direction = element.get("direction")
            if direction:
                dir_badge = f"<span>{html.escape(direction)}</span> "

            open_tag = f"<dt>{dir_badge}<code>"
            close_tag = "</code></dt>"

        elif element.tag == "entry" and element.get("thead") == "yes":
            open_tag, close_tag = ("<th>", "</th>")

        elif element.tag == "simplesect":
            kind = element.get("kind", "note")
            if kind == "since":
                open_tag, close_tag = ("<div><strong>Since:</strong> ", "</div>")
            elif kind == "par":
                open_tag, close_tag = ("<div>", "</div>")
            else:
                open_tag, close_tag = (f'<div-{html.escape(kind)}">', "</div>")
            
        elif element.tag == "ulink":
            url = element.get("url", "#")
            open_tag = f'<a href="{html.escape(url)}" target="_blank" rel="noopener noreferrer">'
            close_tag = "</a>"

        elif element.tag == "ref":
            refid = element.get("refid", "#")
            open_tag = f'<a href="{html.escape(refid)}.html">'
            close_tag = "</a>"

        html_out += open_tag

        if element.text:
            html_out += html.escape(element.text)

        for child in element:
            html_out += self._xml_tags_to_html(child)
            if child.tail:
                html_out += html.escape(child.tail)

        html_out += close_tag

        return html_out

    def _parse_pages(self):
        """Parses XML pages"""

        pages = []
        for cp in self.root.iterfind("compound"):
            if cp.get("kind") == "page":
                refid = cp.get("refid")
                root = ET.parse(self.xml_dir / f"{refid}.xml").getroot()
                cpdef = root.find("compounddef")
                        
                name = cpdef.findtext("title")
                brief = self._xml_tags_to_html(cpdef.find("briefdescription"))
                detailed = brief + self._xml_tags_to_html(cpdef.find("detaileddescription"))

                page = Page(name, brief, detailed, refid)
                pages.append(page)

        save_cache(self.build_dir / "pages.pkl", pages)

def main():
    root_dir = Path(__file__).resolve().parents[1]
    parser = XMLParser(root_dir)
    parser.parse()

if __name__ == "__main__":
    main()