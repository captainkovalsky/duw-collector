#!/bin/bash

REPO_DIR="/Users/viktordzundza/projects/duw"
LOG_FILE="$REPO_DIR/autonomous_execution.log"

cd "$REPO_DIR"

echo "$(date)" >> "$LOG_FILE"

OPEN_ISSUES=$(gh issue list --json number --jq 'length')
echo "issues:$OPEN_ISSUES" >> "$LOG_FILE"

if [ "$OPEN_ISSUES" -gt 0 ]; then
    gh issue list --json number,title --jq '.[] | "\(.number):\(.title)"' >> "$LOG_FILE"
    echo "workflow:execute" >> "$LOG_FILE"
else
    echo "status:clean" >> "$LOG_FILE"
fi

echo "" >> "$LOG_FILE"
