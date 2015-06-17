var util = require('util');

console.log(util.format('["%s"]', process.argv.join('", "')));
