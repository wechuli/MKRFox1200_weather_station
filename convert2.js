const UINT16_t_MAX = 65536;
const INT16_t_MAX = 32767;

let moduleTemp = 5461;

let distance = 12997;

let temp1 = ((moduleTemp / INT16_t_MAX) * 120).toFixed(2);

let convdistance = ((distance / UINT16_t_MAX) * 600).toFixed(2);

console.log(
  `The module temp is ${temp1}, and the distance is ${convdistance} cm `
);
