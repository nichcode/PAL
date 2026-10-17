(() => {
const moon = `
            <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" 
            stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round"
            class="feather-icon"><path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"></path>
            </svg>`;

const sun = `
            <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" 
            stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round" 
            class="feather-icon">
            <circle cx="12" cy="12" r="5"></circle>
            <line x1="12" y1="1" x2="12" y2="3"></line>
            <line x1="12" y1="21" x2="12" y2="23"></line>
            <line x1="4.22" y1="4.22" x2="5.64" y2="5.64"></line>
            <line x1="18.36" y1="18.36" x2="19.78" y2="19.78"></line>
            <line x1="1" y1="12" x2="3" y2="12"></line>
            <line x1="21" y1="12" x2="23" y2="12"></line>
            <line x1="4.22" y1="19.22" x2="5.64" y2="17.78"></line>
            <line x1="18.36" y1="5.64" x2="19.78" y2="4.22"></line>
            </svg>`;

const button = document.getElementById('nav-bar-theme-toggle');
const bodyElement = document.body;
const savedTheme = localStorage.getItem('theme');
const systemPrefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;

// determine our initial theme string loop setting
const initialTheme = savedTheme || (systemPrefersDark ? 'dark-mode' : 'light-mode');
function updateButton(currentClass) {
    button.innerHTML = currentClass === 'light-mode' ? moon : sun;
}

bodyElement.classList.add(initialTheme);
updateButton(initialTheme);

button.addEventListener('click', () => {
    if (bodyElement.classList.contains('light-mode')) {
        bodyElement.classList.remove('light-mode');
        bodyElement.classList.add('dark-mode');
        
        // cache choices and refresh button layout states
        localStorage.setItem('theme', 'dark-mode');
        updateButton('dark-mode');
        
    } else {
        // swap back to light state parameters
        bodyElement.classList.remove('dark-mode');
        bodyElement.classList.add('light-mode');
        
        localStorage.setItem('theme', 'light-mode');
        updateButton('light-mode');
    }
});
})();