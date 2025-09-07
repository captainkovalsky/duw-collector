#!/bin/bash

# GitHub Issues Checker Script
# This script checks for GitHub issues and reports them

REPO_DIR="/Users/viktordzundza/projects/duw"
LOG_FILE="$REPO_DIR/issues_check.log"

cd "$REPO_DIR"

echo "=== GitHub Issues Check - $(date) ===" >> "$LOG_FILE"

# Check for open issues
OPEN_ISSUES=$(gh issue list --json number,title,labels,updatedAt --jq 'length')
echo "Open issues: $OPEN_ISSUES" >> "$LOG_FILE"

if [ "$OPEN_ISSUES" -gt 0 ]; then
    echo "Found $OPEN_ISSUES open issues:" >> "$LOG_FILE"
    gh issue list --json number,title,labels,updatedAt --jq '.[] | "  #\(.number): \(.title) (Updated: \(.updatedAt))"' >> "$LOG_FILE"
    
    # You could add notification here
    echo "ALERT: There are $OPEN_ISSUES open issues that need attention!" >> "$LOG_FILE"
else
    echo "No open issues found." >> "$LOG_FILE"
fi

echo "" >> "$LOG_FILE"

# Display recent log entries
echo "Recent issue checks:"
tail -20 "$LOG_FILE"
