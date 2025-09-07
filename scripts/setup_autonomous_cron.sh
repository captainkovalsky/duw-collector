#!/bin/bash

# Setup Autonomous Cron Job
# This script helps you set up automatic execution of the autonomous agent

REPO_DIR="/Users/viktordzundza/projects/duw"
SCRIPT_PATH="$REPO_DIR/scripts/autonomous_agent.sh"

echo "Setting up autonomous cron job for GitHub issues processing..."
echo ""

# Check if script exists
if [ ! -f "$SCRIPT_PATH" ]; then
    echo "Error: autonomous_agent.sh not found at $SCRIPT_PATH"
    exit 1
fi

echo "Script found at: $SCRIPT_PATH"
echo ""

# Show cron examples
echo "Cron job examples:"
echo ""
echo "# Run every hour:"
echo "0 * * * * $SCRIPT_PATH"
echo ""
echo "# Run every 30 minutes:"
echo "*/30 * * * * $SCRIPT_PATH"
echo ""
echo "# Run every day at 9 AM:"
echo "0 9 * * * $SCRIPT_PATH"
echo ""
echo "# Run every weekday at 8 AM:"
echo "0 8 * * 1-5 $SCRIPT_PATH"
echo ""

# Ask user for preference
echo "Choose your preferred schedule:"
echo "1) Every hour"
echo "2) Every 30 minutes"
echo "3) Every day at 9 AM"
echo "4) Every weekday at 8 AM"
echo "5) Custom"
echo "6) Skip (manual setup)"
echo ""

read -p "Enter your choice (1-6): " choice

case $choice in
    1)
        CRON_LINE="0 * * * * $SCRIPT_PATH"
        ;;
    2)
        CRON_LINE="*/30 * * * * $SCRIPT_PATH"
        ;;
    3)
        CRON_LINE="0 9 * * * $SCRIPT_PATH"
        ;;
    4)
        CRON_LINE="0 8 * * 1-5 $SCRIPT_PATH"
        ;;
    5)
        read -p "Enter your custom cron expression: " custom_cron
        CRON_LINE="$custom_cron $SCRIPT_PATH"
        ;;
    6)
        echo "Manual setup required. Add one of the cron lines above to your crontab."
        echo "Run: crontab -e"
        exit 0
        ;;
    *)
        echo "Invalid choice. Exiting."
        exit 1
        ;;
esac

echo ""
echo "Adding cron job: $CRON_LINE"
echo ""

# Add to crontab
(crontab -l 2>/dev/null; echo "$CRON_LINE") | crontab -

if [ $? -eq 0 ]; then
    echo "✅ Cron job added successfully!"
    echo ""
    echo "Current crontab:"
    crontab -l
    echo ""
    echo "The autonomous agent will now run automatically according to your schedule."
    echo "Check logs at: $REPO_DIR/autonomous_agent.log"
else
    echo "❌ Failed to add cron job. Please add manually:"
    echo "$CRON_LINE"
fi
