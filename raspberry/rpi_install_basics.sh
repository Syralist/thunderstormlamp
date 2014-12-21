#!/bin/bash
echo "apt-get: Installing zsh, vim, and pip"
sudo apt-get install zsh vim vim-nox vim-doc python-pip

echo "git: cloning config-repository"
git clone https://github.com/Syralist/alltheconfs.git

echo "copying config files: .zshrc, .zshprompt, .vimrc"
cp alltheconfs/.zshrc ~
cp alltheconfs/.zshprompt ~
cp alltheconfs/.vimrc ~

echo "changing to zsh"
chsh -s $(which zsh)
