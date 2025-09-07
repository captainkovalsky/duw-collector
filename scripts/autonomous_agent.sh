#!/bin/bash

REPO_DIR="/Users/viktordzundza/projects/duw"
LOG_FILE="$REPO_DIR/autonomous_agent.log"

cd "$REPO_DIR"

echo "$(date)" >> "$LOG_FILE"

if [ -f "LOOP.md" ]; then
    TASK_COUNT=$(grep -c "^- " LOOP.md 2>/dev/null || echo "0")
    echo "tasks:$TASK_COUNT" >> "$LOG_FILE"
fi

OPEN_ISSUES=$(gh issue list --json number --jq 'length')
echo "issues:$OPEN_ISSUES" >> "$LOG_FILE"

if [ "$OPEN_ISSUES" -gt 0 ]; then
    ISSUE_NUMBERS=$(gh issue list --json number --jq '.[].number')
    for issue_num in $ISSUE_NUMBERS; do
        ISSUE_TITLE=$(gh issue view $issue_num --json title --jq '.title')
        echo "process:$issue_num:$ISSUE_TITLE" >> "$LOG_FILE"
        sleep 1
    done
fi

if cd build && make clean && make >> "$LOG_FILE" 2>&1; then
    echo "build:ok" >> "$LOG_FILE"
    if ./duw-collector >> "$LOG_FILE" 2>&1; then
        echo "test:ok" >> "$LOG_FILE"
    else
        echo "test:fail" >> "$LOG_FILE"
    fi
else
    echo "build:fail" >> "$LOG_FILE"
fi

cd "$REPO_DIR"

if [ -f "LOOP.md" ]; then
    TASK_COUNT=$(grep -c "^- " LOOP.md 2>/dev/null || echo "0")
    echo "status:$TASK_COUNT" >> "$LOG_FILE"
fi

echo "" >> "$LOG_FILE"
tail -20 "$LOG_FILE"
