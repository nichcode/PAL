
const vscode = require('vscode');

/**
 * Get the field description from its parent struct, gets the position of the
 * field. Use the field position to find the struct tag @struct. This works
 * with multiline comments and only on a predefined doc style.
 * 
 * The field parameter is still kept as field even though this function
 * works for constants as well.
*/
function findFieldDescription(document, definition, field)
{
    const lines = document.getText().split(/\r?\n/);
    const descriptionLines = [];
    const fieldLine = definition.range.start.line;
    let found = false;

    let isMacro = false;
    let startIndex = -1;
    let endIndex = -1;
    fieldTag = '';

    const tmp = document.lineAt(fieldLine).text;
    if (tmp.includes('#define')) {
        isMacro = true;
    }

    if (!isMacro) {
        for (let i = fieldLine; i >= 0; i--) {
            if (lines[i].includes('{')) {
                endIndex = i;
                break
            }
        }

        if (endIndex === -1) {
            return undefined;
        }

        let structLineIndex = 0;
        structName = '';
        for (let i = endIndex; i >= 0; i--) {
            /**
             * get @since and start searching from there to reduce overhead.
             * We dont break after finding the tag because the since can be in
             * any types def. So we also find the @struct tag in addition.
             */
            if (lines[i].includes('@since')) {
                startIndex = i;
            }

            if (lines[i].includes('@struct')) {
                structLineIndex = i;
                structName = lines[i].replace(/^\s*\*\s*@struct\s+/, '').trim();
                break;
            }
        }

        if (startIndex === -1 && structLineIndex === -1) {
            return undefined;
        }

        fieldTag = '@var ' + structName + '::' + field;

    } else {
        startIndex = 0;
        endIndex = lines.length;
        for (let i = 0; i < endIndex; i++) {
            if (lines[i].includes('@def')) {
                const tmp = lines[i].substring(lines[i].indexOf('@def') + 4).trim();
                if (tmp === field) {
                    startIndex = i;
                    break;
                }
            }
        }

        if (startIndex === -1) {
            return undefined;
        }

        fieldTag = '@def ' + field;
    }
    
    let brief = false;
    for (let i = startIndex; i < endIndex; i++) {
        const line = lines[i];
        if (!found) {
            const pos = line.indexOf(fieldTag);
            if (pos === -1) {
                continue;
            }

            found = true;
            continue;
        }

        // end of documentation block
        if (line.includes('*/')) {
            break;
        }

        // next @tag
        if (/^\s*\*\s*@/.test(line)) {
            break;
        }

        const next = line.trim().replace(/^\*+/, '').trim();
        if (next !== '') {
            if (found && brief === false) {
                brief = true;
                descriptionLines.push(next);
                descriptionLines.push('');
                continue;
            }

            if (found && brief) {
                // check if there is @nl in the line.
                if (next.includes('@nl')) {
                    const tmp = next.replace('@nl', '');
                    descriptionLines.push(tmp);
                    descriptionLines.push('');

                } else {
                    descriptionLines.push(next);
                }
            }
        }
    }

    if (descriptionLines.length === 0) {
        return undefined;
    }

    return descriptionLines.join('\n');
}

/**
 * Get the markdown text for PAL documentation. This defaults to the normal
 * hover if it fails to get the definitions. An error is logged to the 
 * debug console.
*/
async function provideHover(
    document, 
    position, 
    token)
{
    const range = document.getWordRangeAtPosition(position);
    if (!range) {
        return;
    }

    try {
        const definitions = await vscode.commands.executeCommand(
            'vscode.executeDefinitionProvider',
            document.uri,
            position
        );

        if (!definitions || definitions.length === 0) {
            return;
        }

        const definition = definitions[0];
        const targetDocument = await vscode.workspace.openTextDocument(definition.uri);

        const description = findFieldDescription(
            targetDocument, 
            definition, 
            document.getText(range));

        if (!description) {
            return;
        }

        const markdown = new vscode.MarkdownString(description);
        return new vscode.Hover(markdown);

    } catch (error) {
        console.error('PAL Hover:', error);
    }
}

function activate(context)
{
    const provider = vscode.languages.registerHoverProvider(
        ['c', 'cpp'],
        { provideHover }
        );

    context.subscriptions.push(provider);
}

function deactivate () {}

module.exports = {
    activate,
    deactivate
};