
def build_module_page(module):
    content = []
    content.append(f"<h1>{html.escape(module['title'])}</h1>")

    titles = []
    heads = []
    kinds = []
    brief = "Tmp brief"

    if module['functions']:
        titles.append("<h3>Functions</h3>")
        heads.append("Function")
        kinds.append("functions")

    if module['structs']:
        titles.append("<h3>Structs</h3>")
        heads.append("Struct")
        kinds.append("structs")

    if module['types']:
        titles.append("<h3>Types</h3>")
        heads.append("Type")
        kinds.append("types")

    if module['constants'] or module['subgroups']:
        titles.append("<h3>Constants</h3>")
        heads.append("Constant")
        kinds.append("constants")

    for index, head in enumerate(heads):
        content.append(titles[index])
        content.append("<table>")
        content.append("<tr>")
        content.append(f"<th>{head}</th>")
        content.append("<th>Brief</th>")
        content.append("</tr>")

        for refid, name in module[kinds[index]]:
            content.append("<tr>")
            content.append(f'<td><a href="{refid}.html">{name}</a></td>')
            content.append(f'<td>{brief}</td>')
            content.append("</tr>")

        content.append("</table>")

    return {"title": module["title"], "content": "\n".join(content)}