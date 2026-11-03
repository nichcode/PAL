(function () {
    'use strict';

    var index = window.PAL_SEARCH_INDEX;
    var box = document.getElementById('search-box');
    var input = document.getElementById('search');
    var panel = document.getElementById('search-results');
    var sidebar = document.querySelector('.sidebar');
    if (!index || !box || !input || !panel || !sidebar) return;

    box.hidden = false; // the search box only appears when JavaScript runs

    var MAX_RESULTS = 40;
    var KIND_LABELS = {
        module: 'module', function: 'function', struct: 'struct', type: 'typedef',
        constant: 'constant', constants: 'constants', guide: 'guide'
    };
    var active = -1;
    var links = [];

    function norm(s) { return s.toLowerCase().replace(/[^a-z0-9]/g, ''); }

    for (var i = 0; i < index.length; i++) { index[i]._n = norm(index[i].n); }

    // 0 exact, 1 prefix, 2 contains, 3 brief contains, -1 no match
    function score(entry, q, raw) {
        var n = entry._n;
        if (n === q) return 0;
        if (n.indexOf(q) === 0) return 1;
        if (n.indexOf(q) > 0) return 2;
        if (entry.b && entry.b.toLowerCase().indexOf(raw) !== -1) return 3;
        return -1;
    }

    function search(raw) {
        var q = norm(raw);
        if (!q) return [];
        var found = [];
        for (var i = 0; i < index.length; i++) {
            var s = score(index[i], q, raw.toLowerCase().trim());
            if (s >= 0) found.push({ e: index[i], s: s });
        }
        found.sort(function (a, b) {
            return a.s - b.s || a.e.n.length - b.e.n.length || (a.e.n < b.e.n ? -1 : 1);
        });
        return found.slice(0, MAX_RESULTS).map(function (f) { return f.e; });
    }

    function setActive(i) {
        if (links.length === 0) return;
        if (active >= 0) {
            links[active].classList.remove('is-active');
            links[active].setAttribute('aria-selected', 'false');
        }
        active = (i + links.length) % links.length;
        links[active].classList.add('is-active');
        links[active].setAttribute('aria-selected', 'true');
        links[active].scrollIntoView({ block: 'nearest' });
    }

    function show(on) {
        sidebar.classList.toggle('is-searching', on);
        panel.hidden = !on;
    }

    function render(results, raw) {
        panel.textContent = '';
        links = [];
        active = -1;

        if (results.length === 0) {
            var empty = document.createElement('div');
            empty.className = 'search-empty';
            empty.textContent = 'No results for \u201c' + raw.trim() + '\u201d.';
            panel.appendChild(empty);
            return;
        }

        results.forEach(function (e) {
            var a = document.createElement('a');
            a.className = 'search-result';
            a.href = e.u;
            a.setAttribute('role', 'option');
            a.setAttribute('aria-selected', 'false');

            var name = document.createElement('span');
            name.className = 'result-name';
            name.textContent = e.n;
            a.appendChild(name);

            var meta = document.createElement('span');
            meta.className = 'result-meta';
            var label = KIND_LABELS[e.k] || e.k;
            meta.textContent = e.m ? e.m + ' \u00b7 ' + label : label;
            a.appendChild(meta);

            panel.appendChild(a);
            links.push(a);
        });
    }

    function run() {
        var raw = input.value;
        if (!norm(raw)) { show(false); links = []; active = -1; return; }
        render(search(raw), raw);
        show(true);
    }

    function clear() {
        input.value = '';
        show(false);
        links = [];
        active = -1;
    }

    input.addEventListener('input', run);

    input.addEventListener('keydown', function (ev) {
        if (ev.key === 'ArrowDown') { ev.preventDefault(); setActive(active + 1); }
        else if (ev.key === 'ArrowUp') { ev.preventDefault(); setActive(active < 0 ? -1 : active - 1); }
        else if (ev.key === 'Enter') {
            var target = links[active >= 0 ? active : 0];
            if (target) { ev.preventDefault(); window.location.href = target.href; }
        }
        else if (ev.key === 'Escape') { clear(); input.blur(); }
    });

    document.addEventListener('keydown', function (ev) {
        if (ev.key !== '/' || ev.ctrlKey || ev.metaKey || ev.altKey) return;
        var t = ev.target;
        var typing = t && (t.tagName === 'INPUT' || t.tagName === 'TEXTAREA' || t.isContentEditable);
        if (typing) return;
        ev.preventDefault();
        input.focus();
        input.select();
    });
})();
