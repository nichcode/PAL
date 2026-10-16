
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


groups = document.querySelectorAll(".sidebar-group").forEach(group => {
    const button = group.querySelector("button");
    const tree = group.querySelector("sidebar-tree");
    const arrow = group.querySelector("button-img");

    button.addEventListener("click", () => {
        const expanded = tree.hidden;
        tree.hidden = !expanded;
        arrow.src = expanded
        ? arrowClosed
        : arrowOpened;

    button.setAttribute("aria-expanded", expanded);
    })
})
