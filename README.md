# GitHub & Markdown Beginner Guide

This README is for beginners learning **Git**, **GitHub**, and **Markdown**.

---

## What is Git vs GitHub?

- **Git** → tracks changes in your code (version control)
- **GitHub** → website that stores Git repositories online

---

## Git Setup (Run Once)

### Check Git version
```bash
git --version
```

### Configure your name and email
```bash
git config --global user.name "Your Name"
git config --global user.email "youremail@example.com"
```

---

## Basic Git Commands

### Create a new repository
```bash
git init
```

### Clone an existing repository
```bash
git clone https://github.com/username/repository-name.git
```

### Check repository status
```bash
git status
```

### Add files to staging
```bash
git add .
```

### Commit changes
```bash
git commit -m "Your commit message"
```

### Connect local repo to GitHub
```bash
git remote add origin https://github.com/username/repo-name.git
```

### Push code to GitHub
```bash
git push -u origin main
```

### Pull latest changes
```bash
git pull
```

---

## Branching

### Create a new branch
```bash
git checkout -b branch-name
```

### Switch branches
```bash
git checkout main
```

---

## Markdown Basics

Markdown files end with `.md`

### Headings
```md
# Heading 1
## Heading 2
### Heading 3
```

### Text Formatting
```md
**Bold**
*Italic*
~~Strikethrough~~
```

---

## Lists

Bullet list:
```md
- Item one
- Item two
  - Sub item
```

Numbered list:
```md
1. First
2. Second
3. Third
```

---

## Code in Markdown

Inline code:
```md
Use `git status` to check changes
```

Code block:
```md
```bash
git add .
git commit -m "message"
```
```

---

## Links and Images

Link:
```md
[GitHub](https://github.com)
```

Image:
```md
![Alt text](image-url.png)
```

---

## Blockquotes

```md
> This is a note
```

---

## Tables

```md
| Command     | Description        |
|------------|--------------------|
| git init   | Create repository  |
| git status | Check repo status  |
| git push   | Upload code        |
```

---

## Example Beginner README

```md
# My First GitHub Project

This is my first project using Git and GitHub.

## How to Run
```bash
git clone https://github.com/username/repo.git
cd repo
```

## What I Learned
- Git basics
- Markdown
- GitHub workflow
```

---

## Tips

- Commit often
- Write clear commit messages
- Push regularly
- README.md is the first thing people see
