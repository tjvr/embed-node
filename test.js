#!./node
const {add} = require('./addon')
console.log(add(6, 9))


Electrinode.on("click", e => {
  console.log('got event', e);
  return 'hooray';
})

Electrinode.send("click")

