#!/usr/bin/env bash

# update the remote info
$(git fetch)
# get current branch name
BRANCH=$(git rev-parse --abbrev-ref HEAD)

if [ "$1" == "-f" -o "$1" == "--force" ]
    then
    FORCE="-f"
fi

# check remote branch exists
REMOTE_EXISTS=$(git ls-remote | grep -sw ${BRANCH} 2>&1)

if  [ -z "$REMOTE_EXISTS" ]
then
    # Remote branch not found
    # List of commits which differ from remote master
    COMMITS=($(git rev-list ${BRANCH} ^master))
    echo "Remote branch does not exist, pushing commits different from master"
else
    # list of commits which differ with remote branch
    COMMITS=($(git rev-list ${BRANCH} ^origin/${BRANCH}))
    echo "Remote branch found, pushing commits different from remote branch"
fi

# Push commits one by one, starting with the oldest
for ((i=${#COMMITS[@]}-1; i>=0; i--))
do
    if $(git push $FORCE origin ${COMMITS[$i]}:refs/heads/${BRANCH})
    then
        echo "pushed ${COMMITS[$i]}"
    else
        echo -en "\033[31mCould not push to the remote. "
        echo -e  "Execute with -f to force the push.\033[0m"
        break
    fi
done