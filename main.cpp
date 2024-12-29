#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <cstdlib>
#include <csignal>
#include <thread>
#include <chrono>
#include <sys/stat.h>

// Configuration file path for storing private users' information
const std::string CONFIG_FILE = std::getenv("HOME") + std::string("/.git-init-private-users-config.txt");

// Function to fetch GitHub no-reply email address (for private users)
std::string get_github_email(const std::string& github_user) {
    return github_user + "@users.noreply.github.com";
}

// Function to fetch GitLab no-reply email address (for private users)
std::string get_gitlab_email(const std::string& gitlab_user) {
    return gitlab_user + "@users.noreply.gitlab.com";
}

// Function to show a spinner while a task is running
void spinner() {
    const std::string spinstr = "|/-\\";
    size_t index = 0;
    while (true) {
        std::cout << "\r [" << spinstr[index % 4] << "]  ";
        std::cout.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        index++;
    }
}

// Function to check if the configuration file exists
bool config_file_exists() {
    struct stat buffer;
    return (stat(CONFIG_FILE.c_str(), &buffer) == 0);
}

// Function to load the configuration from file
bool load_config(std::string& github_user, std::string& gitlab_user, std::string& github_email, std::string& gitlab_email) {
    std::ifstream file(CONFIG_FILE);
    if (file.is_open()) {
        std::getline(file, github_user);
        std::getline(file, gitlab_user);
        std::getline(file, github_email);
        std::getline(file, gitlab_email);
        file.close();
        return true;
    }
    return false;
}

// Function to save the configuration to file
void save_config(const std::string& github_user, const std::string& gitlab_user, const std::string& github_email, const std::string& gitlab_email) {
    std::ofstream file(CONFIG_FILE);
    file << "GITHUB_USER=" << github_user << "\n";
    file << "GITLAB_USER=" << gitlab_user << "\n";
    file << "GITHUB_EMAIL=" << github_email << "\n";
    file << "GITLAB_EMAIL=" << gitlab_email << "\n";
    file.close();
}

// Function to validate the email format using regex
bool validate_email(const std::string& email, const std::string& domain) {
    std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@" + domain + R"($))");
    return std::regex_match(email, pattern);
}

int main(int argc, char* argv[]) {
    std::string repo_name;
    bool private_user = false;

    // Ensure the user has provided a project name (GitHub/GitLab repository name)
    if (argc < 2) {
        std::cerr << "Usage: ./git-init-setup <repository_name> [--private-user]" << std::endl;
        return 1;
    }

    repo_name = argv[1];

    // Check if the repository name is empty
    if (repo_name.empty()) {
        std::cerr << "Error: Repository name cannot be empty." << std::endl;
        return 1;
    }

    // Parse the command line for --private-user flag
    for (int i = 2; i < argc; ++i) {
        if (std::string(argv[i]) == "--private-user") {
            private_user = true;
        }
    }

    // Variables for GitHub and GitLab details
    std::string github_user, gitlab_user, github_email, gitlab_email;

    // Check if the configuration file exists and load it if available
    if (private_user && config_file_exists()) {
        if (load_config(github_user, gitlab_user, github_email, gitlab_email)) {
            std::cout << "Loaded private user configuration." << std::endl;
            std::cout << "GitHub username: " << github_user << std::endl;
            std::cout << "GitLab username: " << gitlab_user << std::endl;
            std::cout << "GitHub private email: " << github_email << std::endl;
            std::cout << "GitLab private email: " << gitlab_email << std::endl;
        } else {
            std::cerr << "Error loading configuration file." << std::endl;
            return 1;
        }
    } else {
        // If the configuration doesn't exist, prompt for new details
        std::cout << "Configuration file not found or private user not specified. Let's set up private user details." << std::endl;

        // Prompt for GitHub username (private)
        std::cout << "Enter your GitHub username: ";
        std::getline(std::cin, github_user);

        // Prompt for GitLab username (private)
        std::cout << "Enter your GitLab username: ";
        std::getline(std::cin, gitlab_user);

        // Prompt for GitHub and GitLab no-reply email addresses (for privacy)
        std::cout << "Enter your GitHub no-reply email address (e.g., <username>@users.noreply.github.com): ";
        std::getline(std::cin, github_email);

        std::cout << "Enter your GitLab no-reply email address (e.g., <username>@users.noreply.gitlab.com): ";
        std::getline(std::cin, gitlab_email);

        // Validate the no-reply emails
        if (!validate_email(github_email, "users.noreply.github.com")) {
            std::cerr << "Warning: GitHub email should be a no-reply email address." << std::endl;
        }

        if (!validate_email(gitlab_email, "users.noreply.gitlab.com")) {
            std::cerr << "Warning: GitLab email should be a no-reply email address." << std::endl;
        }

        // Save the private user details to the configuration file
        save_config(github_user, gitlab_user, github_email, gitlab_email);
        std::cout << "Private user configuration saved to " << CONFIG_FILE << std::endl;
    }

    // If the user chooses public setup, ask for public user details
    if (!private_user) {
        std::cout << "Initializing with public user details..." << std::endl;
        std::cout << "Enter your GitHub username: ";
        std::getline(std::cin, github_user);

        std::cout << "Enter your GitLab username: ";
        std::getline(std::cin, gitlab_user);

        std::cout << "Enter your GitHub email address: ";
        std::getline(std::cin, github_email);

        std::cout << "Enter your GitLab email address: ";
        std::getline(std::cin, gitlab_email);
    }

    // Ensure the user has provided all details
    if (github_user.empty() || gitlab_user.empty() || github_email.empty() || gitlab_email.empty()) {
        std::cerr << "Error: GitHub or GitLab username/email is missing. Please provide all required details." << std::endl;
        return 1;
    }

    // Get the GitHub and GitLab remote URLs
    std::string github_url = "git@github.com:" + github_user + "/" + repo_name + ".git";
    std::string gitlab_url = "git@gitlab.com:" + gitlab_user + "/" + repo_name + ".git";

    // Initialize the Git repository
    system("git init");

    // Start the spinner for adding remotes
    std::thread spinner_thread(spinner);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    system(("git remote add github " + github_url).c_str());
    system(("git config remote.github.url " + github_url).c_str());
    system(("git remote add gitlab " + gitlab_url).c_str());
    system(("git config remote.gitlab.url " + gitlab_url).c_str());
    spinner_thread.detach();

    // Set the email and name for GitHub and GitLab
    system(("git config user.email " + github_email).c_str());
    system(("git config user.name " + github_user).c_str());

    // If private user is set, set the private emails
    if (private_user) {
        system(("git config remote.github.email " + github_email).c_str());
        system(("git config remote.gitlab.email " + gitlab_email).c_str());
        system(("git config --global user.email " + github_email).c_str());
        system(("git config --global user.name " + github_user).c_str());

        std::cout << "GitHub and GitLab remotes have been added with private email configuration." << std::endl;
    } else {
        std::cout << "GitHub and GitLab remotes have been added with public user emails." << std::endl;
    }

    std::cout << "GitHub remote URL: " << github_url << std::endl;
    std::cout << "GitLab remote URL: " << gitlab_url << std::endl;

}
