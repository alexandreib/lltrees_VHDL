#!/bin/bash
git add .
key=$(cat ~/pwd/git_key)

echo 'Enter the commit message:'
read commitMessage
git commit -m "$commitMessage"
git push  https://alexandreib:${key}@github.com/alexandreib/lltrees.git

