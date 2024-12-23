#!/bin/bash

# Configuration file for storing private users' information
CONFIG_FILE="$HOME/.git-init-private-users-config.txt"

# Function to fetch GitHub no-reply email address (for private users)
get_github_email() {
    local github_user=$1
    echo "$github_user@users.noreply.github.com"
}

# Function to fetch GitLab no-reply email address (for private users)
get_gitlab_email() {
    local gitlab_user=$1
    echo "$gitlab_user@users.noreply.gitlab.com"
}

# Function to show spinner
spinner() {
    local pid=$!
    local delay=0.1
    local spinstr='|/-\\'
    while [ "$(ps a | awk '{print $1}' | grep $pid)" ]; do
        local temp="${spinstr#?}"
        printf " [%c]  " "${spinstr%"$temp"}"
        spinstr=$temp${spinstr%"$temp"}
        sleep $delay
        printf "\b\b\b\b"
    done
    printf "    \b\b\b\b"  # Clear spinner
}

# Ensure the user has provided a project name (GitHub/GitLab repository name)
if [ -z "$1" ]; then
    echo "Usage: ./git-init-setup.sh <repository_name> [--private-user]"
    exit 1
fi

# The project name (repository name)
REPO_NAME=$1

# Flag to check if the user wants to initialize with private email addresses
PRIVATE_USER=false

# Parse the command line for --private-user flag
while [[ "$#" -gt 0 ]]; do
    case $1 in
        --private-user)
            PRIVATE_USER=true
            shift
            ;;
        *)
            shift
            ;;
    esac
done

# Initialize variables for normal users (public setup)
GITHUB_USER=""
GITLAB_USER=""
GITHUB_EMAIL=""
GITLAB_EMAIL=""

# If PRIVATE_USER flag is set, attempt to load the configuration for private users
if $PRIVATE_USER; then
    # Read the configuration file if it exists
    if [ -f "$CONFIG_FILE" ]; then
        source "$CONFIG_FILE"
        echo "Loaded private user configuration."
        echo "GitHub username: $GITHUB_USER"
        echo "GitLab username: $GITLAB_USER"
        echo "GitHub private email: $GITHUB_EMAIL"
        echo "GitLab private email: $GITLAB_EMAIL"
    else
        echo "Configuration file not found. Let's set up private user details."

        # Prompt for GitHub username (private)
        read -p "Enter your GitHub username: " GITHUB_USER

        # Prompt for GitLab username (private)
        read -p "Enter your GitLab username: " GITLAB_USER

        # Prompt for GitHub and GitLab no-reply email addresses (for privacy)
        read -p "Enter your GitHub no-reply email address (e.g., <username>@users.noreply.github.com): " GITHUB_EMAIL
        read -p "Enter your GitLab no-reply email address (e.g., <username>@users.noreply.gitlab.com): " GITLAB_EMAIL

        # Validate the no-reply emails
        if [[ ! "$GITHUB_EMAIL" =~ "users.noreply.github.com" ]]; then
            echo "Warning: GitHub email should be a no-reply email address (e.g., <username>@users.noreply.github.com)."
        fi

        if [[ ! "$GITLAB_EMAIL" =~ "users.noreply.gitlab.com" ]]; then
            echo "Warning: GitLab email should be a no-reply email address (e.g., <username>@users.noreply.gitlab.com)."
        fi

        # Save the private user details in the configuration file
        echo "GITHUB_USER=$GITHUB_USER" > "$CONFIG_FILE"
        echo "GITLAB_USER=$GITLAB_USER" >> "$CONFIG_FILE"
        echo "GITHUB_EMAIL=$GITHUB_EMAIL" >> "$CONFIG_FILE"
        echo "GITLAB_EMAIL=$GITLAB_EMAIL" >> "$CONFIG_FILE"
        echo "Private user configuration saved to $CONFIG_FILE"
    fi
else
    # Normal (public) user configuration (no private user flag set)
    echo "Initializing with public user details..."

    # Prompt for GitHub username (public)
    read -p "Enter your GitHub username: " GITHUB_USER

    # Prompt for GitLab username (public)
    read -p "Enter your GitLab username: " GITLAB_USER

    # Prompt for GitHub and GitLab regular email addresses (public)
    read -p "Enter your GitHub email address: " GITHUB_EMAIL
    read -p "Enter your GitLab email address: " GITLAB_EMAIL
fi

# Ensure the user has provided both GitHub and GitLab details
if [ -z "$GITHUB_USER" ] || [ -z "$GITLAB_USER" ] || [ -z "$GITHUB_EMAIL" ] || [ -z "$GITLAB_EMAIL" ]; then
    echo "Error: GitHub or GitLab username/email is missing. Please provide all required details."
    exit 1
fi

# Get the GitHub and GitLab remote URLs
GITHUB_URL="git@github.com:$GITHUB_USER/$REPO_NAME.git"
GITLAB_URL="git@gitlab.com:$GITLAB_USER/$REPO_NAME.git"

# Initialize the Git repository
git init

# Start the spinner
(echo "Adding remotes..." && git remote add github "$GITHUB_URL" && git config remote.github.url "$GITHUB_URL" && git remote add gitlab "$GITLAB_URL" && git config remote.gitlab.url "$GITLAB_URL") &
spinner

# Set the email and name for GitHub and GitLab
git config user.email "$GITHUB_EMAIL"
git config user.name "$GITHUB_USER"

# If private user is set, set the private emails
if $PRIVATE_USER; then
    # Set the private email for remotes
    git config remote.github.email "$GITHUB_EMAIL"
    git config remote.gitlab.email "$GITLAB_EMAIL"

    # Set the global config for email/name to private user details
    git config --global user.email "$GITHUB_EMAIL"
    git config --global user.name "$GITHUB_USER"

    # Print status for private setup
    echo "GitHub and GitLab remotes have been added with private email configuration."
    echo "GitHub remote URL: $GITHUB_URL"
    echo "GitLab remote URL: $GITLAB_URL"
else
    # Print status for normal setup
    echo "GitHub and GitLab remotes have been added with public user emails."
    echo "GitHub remote URL: $GITHUB_URL"
    echo "GitLab remote URL: $GITLAB_URL"
fi

