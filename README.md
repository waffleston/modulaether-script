# modulaether-script
Effective javascript modularization and fine-control minification.
## What is it?
It's essentially a javascript preprocessing language, designed to improve load times on initialization.  Non-critical functions can be moved to alternate files and loaded asynchronously+deferred to ensure user experience is not impacted.  It also features inline minification toggles (carriage returns, comments, spaces, ...), allowing partial minification / exemptions.
## [Download](build/)
Windows temporarily unavailable.

## In Progress:
* [x] Minification
  * [x] Maintain tabs/spaces before lines.
    * [x] Optimize spaces.
* [ ] Obfuscation
* [ ] Macro functionality.
  * [ ] Advanced dummy functions.
  * [ ] Expand beyond explicit function manoeuvring.
* [x] (modulaether-script specific) Syntax validation.

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
* [spaces](#spaces-offon)
* [tabs](#tabs-offon)
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
// BUT instructions are ignored in line comments
// %^comments off
// I'm still visible, and the next comments will be too.
/*
 * %^comments off
 * I'm still visible, but the next comment won't be.
 */
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
### `%^spaces [off/on]`
Main file:
```javascript
%^spaces off
function spaces(one, two) {
        var three = one + two - two;
        console.log(three + two);
}
```
Output
```javascript
function spaces(one,two){
        var three=one+two-two;
        console.log(one+two);
}
```
### `%^tabs [off/on]`
Main file:
```javascript
%^tabs off
%^spaces off
%^creturn off
function returnextwo(one, two) {
        var resultant = one * two;
        return resultant;
}
```
Output:
```javascript
function returnextwo(one,two){var resultant=one*two;return resultant;}
```
