const INT16_t_MAX = 32767;
const UINT16_t_MAX = 65536;

let moduleTemp = 5461;
let roomTemp = 7918;
let humidity = 5461;

let temp1 = ((moduleTemp / INT16_t_MAX) * 120).toFixed(2);
let temp2 = ((roomTemp / INT16_t_MAX) * 120).toFixed(2);
var dhtHum = ((humidity / UINT16_t_MAX) * 110).toFixed(2);

console.log(
  `The module temp is ${temp1}, the room temp is ${temp2} and humidity is ${dhtHum} `
);
