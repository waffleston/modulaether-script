# modulaether-script
Effective javascript modularization and fine-control minification.
## What is it?
It's essentially a javascript preprocessing language, designed to improve load times on initialization.  Non-critical functions can be moved to alternate files and loaded asynchronously+deferred to ensure user experience is not impacted.  It also features inline minification toggles (carriage returns, comments, spaces, ...), allowing partial minification / exemptions.
## [Download](build/)
Windows temporarily unavailable.

## To-Do:
* [ ] Full rewrite (meeting specification, but not undocumented behavior).
* [ ] Make specification more specific (reduce undocumented behaviors).
* [ ] Semicolon insertion when removing carriage returns.
* [ ] Obfuscation
* [ ] Macros

## Specification is being moved to the wiki.
