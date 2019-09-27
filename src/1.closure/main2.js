var a = function() {
    var x = 0;

    var b = function() {
        return ++x;
    }
    var c = function() {
        return ++x;
    }
    return [b, c];
}

var pair = a()
var b = pair[0];
var c = pair[1];

console.log(b())
console.log(b())

console.log(c())
console.log(c())
