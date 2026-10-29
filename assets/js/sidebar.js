(() => {
const arrowIcon = `
    <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none"
    stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"
    class="feather-icon" aria-hidden="true">
    <polyline points="9 18 15 12 9 6"></polyline>
    </svg>`;

// direct children only, so a group never grabs its nested groups parts.
function parts(group) {
    return {
        button: group.querySelector(':scope > .sidebar-heading > .sidebar-arrow'),
        tree: group.querySelector(':scope > .sidebar-tree')
    };
}

function setOpen(group, open) {
    const { button, tree } = parts(group);
    if (!button || !tree) return;
    tree.hidden = !open;
    button.setAttribute('aria-expanded', String(open));
}

// every group, at any depth, gets an arrow and a click handler.
document.querySelectorAll('.sidebar-group').forEach(group => {
    const { button, tree } = parts(group);
    if (!button || !tree) return;

    button.innerHTML = arrowIcon;
    setOpen(group, !tree.hidden);

    button.addEventListener('click', () => setOpen(group, tree.hidden));
});

// mark the link for the current page and open every tree above it.
const currentPage = location.pathname.split('/').pop() || 'index.html';

document.querySelectorAll('.sidebar a').forEach(link => {
    if (link.getAttribute('href') !== currentPage) return;
    link.setAttribute('aria-current', 'page');

    // A heading link also opens its own tree.
    const ownGroup = link.closest('.sidebar-heading')?.parentElement;
    if (ownGroup) setOpen(ownGroup, true);

    let tree = link.closest('.sidebar-tree');
    while (tree) {
        setOpen(tree.parentElement, true);
        tree = tree.parentElement.closest('.sidebar-tree');
    }
});
})();