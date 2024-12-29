# Configuring Git for GitHub and GitLab with Private Email and Separate Remotes

## Expected Result
At the conclusion of this, your Git environment will be configured to:
- Push to both GitHub and GitLab using separate remotes.
- Obfuscate your email address using the no-reply email addresses from both platforms.
- Use platform-specific usernames for commits pushed to GitHub and GitLab.

## Creating a key

First, you will need to generate an SSH key if you don’t have one. This key will allow you to authenticate securely with GitHub and GitLab.

To create the SSH key, run the following command:

`ssh-keygen -t [whichever type of key you use] -b [if key has a block size]`

For example, to generate a 4096-bit RSA key, you can use:

`ssh-keygen -t rsa -b 4096`

Follow the rest of the prompts to set a password and choose a location for saving the key.

## Creating a configuration for both platforms

You will need to create an SSH configuration to tell Git which key to use for GitHub and GitLab.

To do this, edit the SSH config file. You can open it using your preferred editor, for example:

`vim ~/.ssh/config`

In this file, add the following configuration for both GitHub and GitLab:

# Creating a configuration for both platforms

```
# GitHub configuration
Host github.com
  HostName github.com
  User git
  IdentityFile ~/.ssh/id_rsa
  IdentitiesOnly yes

# GitLab configuration
Host gitlab.com
  HostName gitlab.com
  User git
  IdentityFile ~/.ssh/id_rsa
  IdentitiesOnly yes

```


Save and close the file after making the changes.

## Secure the configuration

Once you've made the changes, ensure that the SSH configuration file is only accessible by you:

`chmod 600 ~/.ssh/config`

## Adding SSH to both platforms

Next, you’ll need to add your public SSH key to both GitHub and GitLab for authentication.

First, copy your public SSH key by running:

`cat ~/.ssh/id_rsa.pub`

Then, follow these steps to add the key to both platforms:

1. **GitHub**:
   - Go to GitHub's SSH settings at `https://github.com/settings/keys`.
   - Click "New SSH key", paste the public key, and give it a title.

2. **GitLab**:
   - Go to GitLab's SSH settings at `https://gitlab.com/profile/keys`.
   - Paste the public key in the "Key" field and give it a title.

## Verify connectivity

Once you’ve added the SSH key to both platforms, verify that the connection is working by running:

`ssh -T gitlab.com`

`ssh -T github.com`

You should receive a success message like:

- For GitHub: `Hi username! You've successfully authenticated, but GitHub does not provide shell access.`
- For GitLab: `Welcome to GitLab, @username!`

## Add remotes

Now, you can add remotes for both GitHub and GitLab to your local repository. Run the following commands:

For GitLab:

`git remote add GitLab git@gitlab.com:<gitlab_user>/<some_upstream_GITLAB_repository>.git`

For GitHub:

`git remote add GitHub git@github.com/<github_user>/<some_upstream_GITHUB_repository>.git`

## Finally, add user configuration

Next, set up the email configuration for each remote.

For GitHub, run:

`git config remote.github.url git@github.com:<some_git_user>/some_GITHUB_repository.git`

`git config remote.github.email <some_email>`

For GitLab, run:

`git config remote.gitlab.url git@gitlab.com:<some_git_user>/some_GITLAB_repository.git`

`git config remote.gitlab.email <some_email>`

## Optionally, you may add an obscured email address for privacy

For privacy, you can configure both GitHub and GitLab to use their no-reply email addresses instead of your personal email.

To enable this, follow these steps:

1. **GitHub**:
   - Go to [GitHub Email Settings](https://github.com/settings/emails).
   - Enable the option to "Keep my email address private." This will generate a no-reply email address like `<your_username>@users.noreply.github.com`.

2. **GitLab**:
   - Go to [GitLab Email Settings](https://gitlab.com/profile/emails).
   - Enable the option to "Hide my email address" to use GitLab’s no-reply email address like `<your_username>@users.noreply.gitlab.com`.

Then, configure Git to use these no-reply email addresses.

For GitHub:

`git config remote.github.url git@github.com:<some_git_user>/<some_GITHUB_repository>.git`

`git config remote.github.email <private_user>@users.noreply.github.com`

For GitLab:

`git config remote.gitlab.url git@gitlab.com:<some_git_user>/<some_GITLAB_repository>.git`

`git config remote.gitlab.email <private_user>@users.noreply.gitlab.com`

## Push All

`git config --global alias.pushall '!git push GitHub && git push GitLab'`

`git pushall`


## Optionally compile this with Clang or GCC, add it to your path

OSX: `clang -O2 main.cpp -o multi_git`

Or

Linux: `g++ -O2 main.cpp -o multi_git`

Run the binary as follows:

`./mult_git`

For example, if you want to add this to your path, it's just:

`export PATH="$PATH:/path/to/your/multi_git"`
