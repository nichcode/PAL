
const arrowButton = document.getElementById('sidebar-arrow');
const arrowContainer = document.getElementById('sidebar-arrow-container');

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

const savedState = localStorage.getItem('state');
const initialState = savedState || 'closed';
currentState = initialState;

function updateIcon(state) {
    if (arrowContainer) {
        arrowContainer.innerHTML = state === 'opened' ? arrowOpened : arrowClosed;
    }
}

updateIcon(initialState);

if (arrowButton) {
    arrowButton.addEventListener('click', () => {
        console.log(currentState)
        if (currentState === 'closed') {
            currentState = 'opened';
        } else {
            currentState = 'closed';
        }

        localStorage.setItem('state', currentState);
        updateIcon(currentState);
    });
}