
const toggleBtn = document.getElementById('theme-toggle');
const iconContainer = document.getElementById('theme-icon-container');
const bodyElement = document.body;

const moonSvg = '<svg xmlns="http://w3.org" width="16" height="16" viewBox="0 0 24 24" fill="none" \
                stroke="currentColor" stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round" \
                class="feather-icon"><path d="M21 12.79A9 9 0 1 1 11.21 3 7 7 0 0 0 21 12.79z"></path> \
                </svg>';

const sunSvg = '<svg xmlns="http://w3.org" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" \
                stroke-width="2.5" stroke-linecap="round" stroke-linejoin="round" class="feather-icon"><circle \
                cx="12" cy="12" r="5"></circle><line x1="12" y1="1" x2="12" y2="3"></line><line x1="12" y1="21" \
                x2="12" y2="23"></line><line x1="4.22" y1="4.22" x2="5.64" y2="5.64"></line><line x1="18.36" y1="18.36" \
                x2="19.78" y2="19.78"></line><line x1="1" y1="12" x2="3" y2="12"></line><line x1="21" y1="12" x2="23" \
                y2="12"></line><line x1="4.22" y1="19.22" x2="5.64" y2="17.78"></line><line x1="18.36" y1="5.64" x2="19.78" \
                 y2="4.22"></line> \
                 </svg>';
     
const savedTheme = localStorage.getItem('theme');
const systemPrefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;

// Determine our initial theme string loop setting
const initialTheme = savedTheme || (systemPrefersDark ? 'dark-mode' : 'light-mode');
function updateIcon(currentClass) {
    if (iconContainer) {
        // If the body has the light-mode class, show the moon (to switch to dark)
        iconContainer.innerHTML = currentClass === 'light-mode' ? moonSvg : sunSvg;
    }
}

// Apply the layout state variables safely on boot setup
bodyElement.classList.add(initialTheme);
updateIcon(initialTheme);

if (toggleBtn) {
    toggleBtn.addEventListener('click', () => {
        // Check which class is currently active on the body node string
        if (bodyElement.classList.contains('light-mode')) {
            // Swap classes cleanly
            bodyElement.classList.remove('light-mode');
            bodyElement.classList.add('dark-mode');
            
            // Cache choices and refresh button layout states
            localStorage.setItem('theme', 'dark-mode');
            updateIcon('dark-mode');
        } else {
            // Swap back to light state parameters
            bodyElement.classList.remove('dark-mode');
            bodyElement.classList.add('light-mode');
            
            localStorage.setItem('theme', 'light-mode');
            updateIcon('light-mode');
        }
    });
}
