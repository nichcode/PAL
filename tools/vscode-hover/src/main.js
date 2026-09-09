
const vscode = require('vscode');

/**
 * Get the field description from its parent struct, gets the position of the
 * field. Use the field position to find the struct tag @struct. This works
 * with multiline comments and only on a predefined doc style.
*/
function findFieldDescription(document, definition, field)
{
    const lines = document.getText().split(/\r?\n/);
    const fieldLine = definition.range.start.line;

    let openBraceLine = -1;
    for (let i = fieldLine; i >= 0; i--) {
        if (lines[i].includes('{')) {
            openBraceLine = i;
            break
        }
    }

    if (openBraceLine === -1) {
        return undefined;
    }
    
    let structTagLine = -1;
    structName = '';
    for (let i = openBraceLine; i >= 0; i--) {
        if (lines[i].includes('@struct')) {
            structTagLine = i;
            structName = lines[i].replace(/^\s*\*\s*@struct\s+/, '').trim();
            break;
        }
    }

    if (structTagLine === -1) {
        return undefined;
    }

    const fieldTag = '@var ' + structName + '::' + field;
    const descriptionLines = [];
    let found = false;

    for (let i = structTagLine; i < openBraceLine; i++) {
        const line = lines[i];
        if (!found) {
            const position = line.indexOf(fieldTag);
            if (position === -1) {
                continue;
            }

            const description = line.substring(position + fieldTag.length).trim();
            if (description !== '') {
                descriptionLines.push(description);
            }

            found = true;
            continue;
        }

        if (line.includes('*/')) {
            break;
        }
        
        if (/^\s*\*\s*@/.test(line)) {
            break;
        }

        const next = line.trim().replace(/^\*+/, '').trim();
        if (next !== '') {
            descriptionLines.push(next);
        }
    }

    if (descriptionLines.length === 0) {
        return undefined;
    }

    return descriptionLines.join(' ');
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