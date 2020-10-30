#!/bin/bash

set -e

SCRIPT_PATH="$(cd $(dirname $0) ; pwd -P)"
OUTPUT_PATH="/output/coverage/"
COVERAGE_JOB_NAME="code-coverage"
PROJECT_ID=2467

# Functions
die() { echo "error: $@" 1>&2 ; exit 1; }
confDie() { echo "error: $@ Check the server configuration!" 1>&2 ; exit 2; }
debug() {
  [ "$debug" = "true" ] && echo "debug: $@"
}

# Validate global configuration
[ -z "$GITLAB_TOKEN" ] && confDie "GITLAB_TOKEN not set."

# Validate Gitlab hook
[ "$x_gitlab_token" != "$GITLAB_TOKEN" ] && die "bad hook token"

# Validate parameters
payload=$1
payload="$(echo "$payload"|tr -d '\n')"
[ -z "$payload" ] && die "missing request payload"
payload_type=$(echo $payload | jq type -r)
[ $? != 0 ] && die "bad body format: expecting JSON"
[ ! $payload_type = "object" ] && die "bad body format: expecting JSON object but having $payload_type"

debug "received payload: $payload"

# Extract values
object_kind=$(echo $payload | jq .object_kind -r)
[ $? != 0 -o "$object_kind" = "null" ] && die "unable to extract 'object_kind' from JSON payload"

# Make sure this is a CI job
if [[ "$object_kind" != 'build' ]]
then
    exit 0
fi

# Make sure this is a successful code coverage build
is_success="$(echo "$payload" | jq -r "select(.build_name == \"$COVERAGE_JOB_NAME\" and .build_status == \"success\") | .build_id")"

if [[ -z "$is_success" ]]
then
    exit 1
fi

build_id="$(echo "$payload" | jq -r '.build_id')"
branch_name="$(echo "$payload" | jq -r '.ref')"

# Download build artifact
debug "Build ID $build_id has produced an artifact. Attempting to download"

output_file="/tmp/artifacts_$(date +%F-%H-%M-%S.$RANDOM).zip"

curl -s --output "$output_file" --header "PRIVATE-TOKEN: $GITLAB_PRIVATE_TOKEN" "https://gitlab.inf.ethz.ch/api/v4/projects/$PROJECT_ID/jobs/$build_id/artifacts"

if [[ "$branch_name" != "master" ]]
then
    prefix="$branch_name"
fi

# Extract
mkdir -p "$OUTPUT_PATH/$prefix/"
find "$OUTPUT_PATH/$prefix/" -maxdepth 1 -type f -delete

tmpdir="$(mktemp -d)"
cd "$tmpdir"

unzip "$output_file"

cd coverage
find . -mindepth 1 -maxdepth 1 -exec mv {} "$OUTPUT_PATH/$prefix/" \;

cd
rm -r "$tmpdir"
