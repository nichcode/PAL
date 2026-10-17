
const arrowClosed = `
                    <svg xmlns="www.http://w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" 
                    stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round" 
                    class="feather-icon">
                    <polyline points="9 18 15 12 9 6"></polyline>
                    </svg>`;

const arrowOpened = `
                <svg xmlns="www.http://w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" 
                stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round" 
                class="feather-icon">
                <polyline points="6 9 12 15 18 9"></polyline>
                </svg>`;

function updateButton(button, state) {
    if (button) {
        button.innerHTML = state === 'opened' ? arrowOpened : arrowClosed;
    }
}

document.querySelectorAll('.sidebar-group').forEach(group => {
    const button = group.querySelector('.sidebar-arrow');
    const sidebarTree = group.querySelector('.sidebar-tree');
    updateButton(button, 'closed');

    button.addEventListener('click', () => {
        const expanded = sidebarTree.hidden;
        sidebarTree.hidden = !expanded;

        if (expanded) {
            updateButton(button, 'opened');
        } else {
            updateButton(button, 'closed');
        }

        button.setAttribute('aria-expanded', expanded);
    });
});