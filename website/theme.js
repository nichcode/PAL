
const storageKey = 'darkmode-toggle';
const toggleBtn = document.getElementById('theme-toggle');

// 1. Read theme choice instantly from browser cache memory
let isDarkMode = localStorage.getItem(storageKey) !== 'light';
document.body.classList.toggle('light-mode', !isDarkMode);

toggleBtn.addEventListener('click', () => {
    isDarkMode = !isDarkMode;
    document.body.classList.toggle('light-mode', !isDarkMode);
    localStorage.setItem(storageKey, isDarkMode ? 'dark' : 'light');
});
