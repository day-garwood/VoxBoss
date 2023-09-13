# Contributing to VoxBoss
First of all, thanks so much for your interest in this project. With your help, we can hopefully build something great.
Whether you're looking to fix bugs, add new features, or simply correct typos, your efforts are appreciated. This document provides guidelines and instructions for contributing to the project.
Here are some tips to get you started.

### Test and Provide Feedback
If you've integrated VoxBoss into your project, it would be great to know how it went.

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

To contribute code:
1. Fork the Repository: If you're unfamiliar with forking, GitHub has a [helpful guide on this](https://docs.github.com/en/get-started/quickstart/fork-a-repo)
2. Create a New Branch from "dev": Using a branch-based development workflow helps keep things organized.
3. Commit Your Changes: Aim for clear and concise commit messages detailing your changes.
4. Test Your Changes: Please ensure that your changes don't introduce any bugs, and that they align with the purpose and scope of the project.
5. Open a Pull Request: Provide details on the changes made and the rationale behind them.

#### Core library
Code directly involved with the library (I.E. its API or its structures) must be self-contained within the vb.c and vb.h files, and must require no external dependencies.

#### Adding Handlers
For those who wish to introduce a new handler:
1. Add the handler as a separate C file.
2. In your pull request, please specify whether you intend it to be a builtin handler or an extra.
3. state the license governing the third-party speech engine.
4. Ensure it's compatible with the VoxBoss license (zlib).
5. Document any specific setups or requirements particular to the handler.

If the handler is written for the core engine and assuming it is approved, it will be merged into the vb.c/h files as appropriate.

#### Attestation
By contributing, you attest that:
* The contribution is your original work, or you have permissions to submit under the project's license.
* You grant VoxBoss the right to use and distribute your contribution under the zlib license.

## Feedback & Questions
Feedback is invaluable! Should you have any questions or concerns, please remember that [help is only a click away](https://github.com/day-garwood/VoxBoss/issues).
