# modulaether-script
Effective javascript modularization and fine-control minification.
## What is it?
It's essentially a javascript preprocessing language, designed to improve load times on initialization.  Non-critical functions can be moved to alternate files and loaded asynchronously+deferred to ensure user experience is not impacted.  It also features inline minification toggles (carriage returns, comments, ...), allowing partial minification / exemptions.
## [Download](build/)
## Todo:
* [x] Allow multiple input files.
  * [x] in-file command to insert files.
* [ ] Differentiate/Build-in optional minification/obfuscation.
  * [x] Maintain tabs/spaces before lines.
    * [ ] For minification, optimize whitespace.
  * [x] Detect/remove single-line comments.
  * [x] Detect/Remove multi-line comments.
* [ ] Macro functionality.
  * [ ] Advanced dummy functions.
  * [ ] Expand beyond explicit function manoeuvring.
* [ ] Syntax validation.

## Instruction Reference
Instructions are lines of code prefixed with `%^` that tell the compiler to do something.  
Format: `%^COMMAND_NAME [1] [2] [3]...`
Where [1] is the first argument and so on.
### List of Instructions
* [srcdef](#srcdef-1)
* [fn](#fn-1-2)
* [thisfile](#thisfile-1)
* [defer](#defer-1)
* [root](#root-1)
* [insert](#insert-1)
* [comments](#comments-offon)
* [creturn](#creturn-offon)
### `%^srcdef [1]`
[1] is an output file to be located at [1].js Ex:
```javascript
%^srcdef extrafileone
```
### `%^fn [1] [2+]`
[1] is target file, [2+] is the function. Ex:
```javascript
%^fn extrafileone somefunction(one, two) {
        if (one%two == 0) {
                console.log(two);
        } else if (one == 90) {
                console.log(one);
        }
}
```
### `%^thisfile [1]`
[1] is the output file that this entire file should be placed in without processing. Ex:
```javascript
%^thisfile extrafileone
// Generic javascript after here.
```
### `%^defer [1]`
[1] is the number of milliseconds to wait before initiating async load of extra js files. Ex:
```javascript
%^defer 1000
%^srcdef extrafiletwo
%^srcdef extrafilethree
%^defer 2000
%^srcdef extrafilefour
```
### `%^root [1]`
[1] is the uri path to the location the source will be on the webserver. Ex:
```javascript
%^root /HTML/core/
%^srcdef extrafilefive
```
### `%^insert [1]`
[1] is the path to a local file that should be inserted here. Ex:  
main file:
```javascript
function insertexample(one) {
        // Insert the macro
        %^insert macro.js
}
```
macro.js:
```javascript
console.log('Hello, '+one);
i++;
```
result:
```javascript
function insertexample(one) {
        // insert the macro
        console.log('hello, '+one);
        i++;
}
```
### `%^comments [off/on]`
Ex:
```javascript
%^comments off
// You can't see this.
%^comments on
// This is visible.
// BUT instructions are ignored regardless
// %^comments off
// I'm still visible.
```
### `%^creturn [off/on]`
Main file:
```javascript
%^creturn off
function returnexample() {
        return undefined;
        console.log('undefined is undefined!');
}
```
Output:
```javascript
function returnexample() {        return undefined;        console.log('undefined is undefined!');}
```
Whitespace will be maintained as a safety precaution against possible issues - smart whitespace removal is in the works.
