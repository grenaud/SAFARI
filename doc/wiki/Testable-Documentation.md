<!-- !test program bash -eo pipefail -->
## Overview

The vg build system is able to test examples in the Github wiki, and in the vg README. This is accomplished by adding the git version of the Github wiki as a submodule in `/doc/wiki` in the vg repo, and using the `txm` tool to execute test cases in the wiki Markdown files that are annotated with special comments

## How-To

To add testable examples to a file:

1. Make sure that the file has this comment at the top, to tell the `txm` tool how to run the examples:
    ```
    <!-- !test program bash -eo pipefail -->
    ```
2. Write your example as a ` ``` ... ``` ` code block that is able to execute in the vg repo's `test` directory.
3. Put a comment before the code block that tells `txm` to run it, and gived it a name:
    ```
    <!-- !test check Your test description here -->
    ```
    The `check` part says that `txm` should make sure that the command succeeds (i.e. exits with `0`). Generally we use this because we want the commands in our documentation to run without error when users try to follow along. Tests that actually check for the correctness of the result might be better off as bash-tap or unit tests, rather than wiki examples.
4. Get your changes into your local vg checkout's `doc/wiki` submodule. If using the online wiki editor:
    1. Save your changes online.
    2. Go to `doc/wiki` in your checkout of the vg repo.
    3. Run `git fetch` to download changes from the live wiki.
    4. Inspect the changes with `git diff HEAD..origin/master` to make sure that nobody has been adding malicious test cases to our wiki.
    5. Pull the changes into the submodule with `git merge origin/master`.
    6. If you've ended up doing a non-fast-forward merge and creating a new commit, follow the steps below for editing locally to upload that commit.
    
    If editing locally in `doc/wiki`:
    1. Commit your changes on some branch (`mybranch`) **within** the `doc/wiki` submodule.
    2. Make sure you have the SSH version of the wiki remote added **within** the `doc/wiki` submodule:
        ```
        git remote add upstream git@github.com:vgteam/vg.wiki.git
        ```
    2. Run `git push upstream mybranch:master` from **within** the `doc/wiki` submodule.
    3. If Github rejects your push and you need to merge, follow the steps above for editing online to pull in changes from the online copy of the wiki.
5. Go back to the root of the vg repo.
6. Test your changes by running the script at `doc/test-docs.sh`. You can also test a single file, from the `test` directory, by running something like `txm --jobs 1 ../doc/wiki/Testable-Documentation.md`.
7. When you have it working, add your changes with and `git add doc/wiki`, and commit your changes with `git commit`.
8. Make a pull request to get your new test example merged.

## Example Test Case

Here is an example test as it appears on the wiki:

<!-- !test check Can run a trivial test -->
```
echo "racecar" | rev
```

And here is the code used to create it:


    <!-- !test check Can run a trivial test -->
    ```
    echo "racecar" | rev
    ```




