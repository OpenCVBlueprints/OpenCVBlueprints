# Contributing guidelines for the OpenCV Blueprints repository

This guidelines describe what you should pay attention to when adding an issue or pull request to this repository.

## Adding an issue to report any sort of problem

This option is used when you want to report any mistakes in the book or the code repository, or if you have encountered an issues when trying to run the software. To keep it a bit organized we ask you to keep the following rules in mind.

1. Start by going to the existing [issues page](https://github.com/OpenCVBlueprints/OpenCVBlueprints/issues) and use the search box at the top right corner of the page to look for any issues that might be related to your problem using issue specific keywords. Make sure that you do not only search the open issues, but also the closed ones.
 * If your issue already exists, then please add more information that could help solve the issue.
 * If your issue doesn't exist yet, it is time to move to step 2.
2. Hit the 'new issue' button to open up a new issue.
 * Make sure you add enough information like your OS, 32 or 64 bit, compiler used, environment, ...
 * If possible add a link to the code that is not working.
 * Mention if you got a possible solution or not.
 * Explain your problem as detailed as possible. Remember that an image is sometimes better than a thousand words.
 * You should basically make sure that the authors can reproduce your problem.
3. Make sure that you assign a correct label, from question to bug, in order to get help as fast as possible.
4. Be patient untill one of the authors picks up the issue and assigns himself to it. He will then try to help you find the solution.
5. If by any means you know how to fix the issue or you have a possible solution, then read on and take a look at how to create a pull request.

## Adding a pull request suggesting a fix of the content of the repository

If you know the solution to an existing issue, then do the following.

1. Clone the repository to your own github account by pressing the `fork` button at the right top of the [OpenCVBlueprints homepage](https://github.com/OpenCVBlueprints/OpenCVBlueprints).
2. Make a local clone on your machine of your github fork, by executing `git clone https://github.com/UserName/OpenCVBlueprints.git OpenCVBlueprints`.
3. Make sure that you check out the master branch by doing `git checkout master` inside the OpenCVBlueprints folder.
4. Make a local branch, with a meaningful name for your fix by doing `git checkout -b fix_issue1`, ofcourse replacing the issues number with your issue number.
5. Now make your local changes and see if your problem is gone. If they work, you add them to the branch with `git add file_that_you_changed`.
6. Store your changes with `git commit -m "Fix for issue 1: trying to explain github PR's"`.
7. Push your changes to your github fork of the OpenCVBlueprints repository with `git push -f --all origin`.

Now go back to the [OpenCVBlueprints pull request page](https://github.com/OpenCVBlueprints/OpenCVBlueprints/pulls) and click on the `new pull request` button. Select the branch that you made and compare it with the master branch of our repo. Again confirm that the changes are actually the changes that you want to make.

* Make sure you give a meaningful title and content message inside the pull request.
* If you made an issue for this fix, then please reference the issue from inside the pull request by copying the hyperlink inside the message body. This will link both items together.

Now wait until one of the authers evaluates your fix and merges it inside the master branch. From that moment you will be thanked and you will get mentioned on our contributers wall!

 
