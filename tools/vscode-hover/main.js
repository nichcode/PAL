
const vscode = require('vscode');

function activate(context)
{
    console.log("Hover Extension Activated");
    
    const test = vscode.commands.registerCommand('PAL-docs-hover.hello', function () {
        vscode.window.showInformationMessage('Hello World');
    });

    context.subscriptions.push(test);
}

function deactivate()
{
    console.log("Hover Extension Deactivated");
}

module.exports = {
    activate,
    deactivate
};