# Contributing to VoxBoss
First of all, thanks so much for your interest in this project. With your help, we can hopefully build something great together.

Whether you're looking to fix bugs, add new features, or simply correct typos, your efforts are appreciated. This document provides guidelines and instructions for contributing to the project.

Here are some tips to get you started.

### Test and Provide Feedback
If you've integrated VoxBoss into your project (or even if you compile and use the example), it would be great to know how it went.

### Spread the Word
Know someone who might benefit from VoxBoss? Share this project with them.

### Suggest features
Suggestions for features, handlers and so on are very much encouraged, especially in this early stage. Feel free to open an issue.

### Bug reports
Bug squashing is fun (splat!). So please go ahead and report them. Again, feel free to open an issue.

### Documentation
Improvements to documentation will be reviewed with pleasure. New documents, such as tutorials, API reference etc, would also be very useful.

### Contributing code
Code contributions are welcome.

Any code that you think can improve the project will be looked at, but here are some ideas if you're looking for specifics:
* Does it not do something that might be important? Feel free to extend the API.
* Do you have experience with a speech engine not yet supported? Consider contributing a handler.
* Maybe you're an expert in cross-platform development. Your contributions could improve this project by miles.

To contribute code, fork the repo and open a pull request. If you're new to this process, here's a typical method for doing this:

1. Fork the repository. If you're unfamiliar with forking, GitHub has a [helpful guide on this](https://docs.github.com/en/get-started/quickstart/fork-a-repo)

2. Clone:

```shell
git clone https://github.com/username/VoxBoss.git
```

Replace username with your GitHub account name.

3. Add a remote so you can fetch updates:

```shell
git remote add upstream https://github.com/day-garwood/VoxBoss.git
```

4. Sync your fork with the original

```shell
cd VoxBoss
git fetch upstream
```

5. Create a new branch from "dev".

```shell
git checkout dev
git merge upstream/dev
git checkout -b my-work
```

The main branch will eventually hold the stable code.

6. Work and commit changes:

```shell
git add .
git commit
```

Aim for clear commit messages detailing your changes.

7. Push your changes:

```shell
git push origin my-work
```

8. Open a pull request: Provide details on the changes made and the rationale behind them.

Please bear the following in mind:

#### Core library
Code directly involved with the library (I.E. its API or its structures) must be self-contained within the vb.c and vb.h files, and must require no external files or dependencies.

#### Adding Handlers
For those who wish to introduce a new handler:

1. Open a pull request, with your handlers as separate C/H files.

2. In the comments:

A. please specify whether you intend it to be a builtin handler or an extra.

B. State the licence governing the third-party speech engine.

3. Document any specific setups or requirements particular to the handler.

If the handler is written for the core engine and assuming it is approved, it will be merged into the vb.c/h files as appropriate.

#### Attestation
By contributing, you attest that:
* The contribution is your original work, or you have permission to submit under the project's licence.
* You grant VoxBoss the right to use and distribute your contribution under the zlib licence.

## Feedback & Questions
Feedback is invaluable! Should you have any questions or concerns, please remember that [help is only a click away](https://github.com/day-garwood/VoxBoss/issues).
