# How to push this to GitHub

Run these from inside this folder (`prefix-search-engine/`).

## 1. Sanity check first
```bash
make cmp && ls data
```
Expect a clean compile and three `.txt` files. If either fails, stop — the
folder layout is wrong and pushing now would give you a repo that doesn't build.

```bash
make clean
```

## 2. One-time git identity (so commits link to your profile)
```bash
git config --global user.name "Divyansh Gupta"
git config --global user.email "your-github-email@example.com"
```

## 3. Create the repo and push
```bash
git init
git add -A
git commit -m "Initial commit"
git remote add origin https://github.com/Divyansh-55/prefix-search-engine.git
git branch -M main
git push -u origin main
```

Create the GitHub repo **empty** — no README, no .gitignore, no license.
This folder already has its own README and .gitignore.

## 4. At the password prompt
GitHub does not accept your account password. Use a Personal Access Token:
Settings -> Developer settings -> Personal access tokens -> Tokens (classic)
-> Generate new token -> tick the `repo` scope -> copy it -> paste at the
`Password:` prompt (nothing appears while pasting; that is normal).

To stop being asked every time (macOS):
```bash
git config --global credential.helper osxkeychain
```

## 5. Set the repo description on GitHub
This is what recruiters actually read:

> C++ prefix and substring search over 200k terms - trie vs. linear-scan benchmarks, KMP substring matching
