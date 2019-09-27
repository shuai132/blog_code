var makeCounter = function() {
    var count = 0;
    var func = function() {
        return ++count;
    };
    return func;
};

var counter = makeCounter();

console.log(counter());
console.log(counter());
