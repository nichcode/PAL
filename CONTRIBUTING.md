
# Contributing to PAL

Thank you for your interest in contributing to **PAL**, a low-level, explicit,
cross-platform abstraction layer. Contributions of all kinds are welcome —
bug fixes, platform correctness updates, documentation, or new backend
improvements.

Please take a moment to read these guidelines before opening an issue or pull request.

---

## 🐛 Reporting Issues

When filing an issue, include:

- Platform + version (Windows / Linux / etc.)
- Compiler + version
- Backend involved (`win32`, `x11`, `wayland`, etc.)
- A minimal reproducible example if possible
- Exact error codes or logs produced by PAL

Clear reports make it much easier to fix the problem.

---

## 💡 Suggesting Enhancements

Before starting work on any major change:

1. Open an **Issue** or **Discussion**.
2. Describe:
   - The problem you’re solving  
   - Why it belongs in PAL  
   - How it aligns with PAL goals (explicit, low-level, predictable)

This helps keep the project focused and consistent.

---

## 🧩 Pull Requests

### PR Checklist

Before submitting a PR:

- Ensure it compiles on **all** supported platforms.
- Run any relevant backend smoke tests.
- Document any public API changes.
- Update `CHANGELOG.md` if applicable.
- Keep the PR focused — avoid mixing unrelated edits.

### PR Style

- Clear commit messages (e.g. `wayland: fix surface_enter behavior`).
- Prefer small, incremental commits over one large “everything” commit.
- Keep platform code isolated (`*_win32.c`, `*_x11.c`, etc.).

---

## 🧪 Coding Style

PAL uses simple, consistent C:

- **C99** for C source.
- `lower_camel_case` for functions + variables. (e.g. `palCreateWindow`).
- `PascalCase` for public types (e.g. `PalResult`, `PalWindow`).
- Minimal runtime validation.
- No hidden work (ex: no automatic device enumeration).
- Avoid dynamic allocations unless documented.

Platform Rules:

- Follow each platform’s documentation strictly.
- Do not depend on undefined or unofficial behavior.
- Keep backend-specific files separated.

---

## 🧱 Architecture Philosophy

PAL aims to remain:

- **Explicit** — no hidden allocations or state.
- **Low-overhead** — almost equivalent to calling the OS directly.
- **Cross-platform** — similar API behavior across OSes.
- **Predictable** — the library never “does work behind your back.”

Please keep this in mind when contributing.

---

## ✔️ Thank You

Every contribution — tests, docs, bug reports, or backend fixes — helps PAL
remain a robust low-level foundation for engine developers.  
Thank you for being part of the project!