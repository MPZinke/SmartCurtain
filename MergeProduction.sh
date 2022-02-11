#!/bin/bash

# FROM: https://gist.github.com/tmiller/5222478
# Merges the master branch into all other branches
#
# Process:
#
#	- Save the name of the current branch
#	- If the current branch is not master then checkout master.
#	- Pull the latest changes for master from its upstream branch.
#	- Loop over each local branch.
#	 - If the branch is not master.
#		- Checkout the branch.
#		- Merge master into the branch.
#	- If the current branch is not the saved branch name checkout the saved
#	 branch name

MAIN_BRANCH="Production"


# Returns the names of all the local branches
local_branches()
{
	git for-each-ref --format="%(refname:short)" refs/heads
}

# Returns the name of the current branch
current_branch()
{
	git symbolic-ref --short HEAD
}

saved_branch=$(current_branch)

[[ "${saved_branch}" != "${MAIN_BRANCH}" ]] && git checkout "${MAIN_BRANCH}"
git pull

for branch in $(local_branches); do
	if [[ "${branch}" != "${MAIN_BRANCH}" ]]; then
		echo
		git checkout "${branch}"
		git merge "${MAIN_BRANCH}"
	fi
done

echo
[[ "${saved_branch}" != "$(current_branch)" ]] && git checkout "${saved_branch}"
