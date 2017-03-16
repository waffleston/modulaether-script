# modulaether-script
Speeding up development of modular javascript applications.
## What is it?
It's essentially a javascript preprocessing language, designed to improve load times on initialization.  Non-critical functions are moved to alternate files and loaded asynchronously+deferred to ensure user experience is not impacted.
## Todo:
* [x] Allow multiple input files.
* [ ] Differentiate/Build-in optional minification/obfuscation.
  * [x] Maintain tabs/spaces before lines.
    * [ ] For minification, optimize whitespace.
  * [ ] Detect/remove single-line comments.
  * [ ] Detect/Remove multi-line comments.
* [ ] Macro functionality.
  * [ ] Advanced dummy functions.
  * [ ] Expand beyond explicit function manoeuvring.
* [ ] Syntax validation.

