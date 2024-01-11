const fs = require('fs');

const json = JSON.parse(fs.readFileSync('test_bim.json').toString('utf8'));

let x_min = 999999.99;
let x_max = -999999.99;
let y_min = 999999.99;
let y_max = -999999.99;
let z_min = 999999.99;
let z_max = -999999.99;

for (let i = 0; i < json["coordinates"].length; i+=3) {
    const x = json["coordinates"][i];
    const y = json["coordinates"][i+1];
    const z = json["coordinates"][i+2];

    if (x < x_min)
        x_min = x;
    if (x > x_max)
        x_max = x;

    if (y < y_min)
        y_min = y;
    if (y > y_max)
        y_max = y;

    if (z < z_min)
        z_min = z;
    if (z > z_max)
        z_max = z;

    //console.log(`${x}\t${y}\t${z}`);
}

let x_mid = (x_min + x_max)/2.0;
let y_mid = (y_min + y_max)/2.0;
let z_mid = (z_min + z_max)/2.0;

console.log(`x_len: ${(x_max-x_min)}, y_len: ${(y_max-y_min)}, z_len: ${(z_max-z_min)}`);

console.log(`x: ${x_min} -> ${x_mid} -> ${x_max}`);
console.log(`y: ${y_min} -> ${y_mid} -> ${y_max}`);
console.log(`z: ${z_min} -> ${z_mid} -> ${z_max}`);

for (let i = 0; i < json["coordinates"].length; i+=3) {
    const r = 0.8/((y_mid * -1) + y_max);

    const x = ((x_mid * -1) + json["coordinates"][i]) * r;
    const y = ((y_mid * -1) + json["coordinates"][i+1]) * r;
    const z = ((z_mid * -1) + json["coordinates"][i+2]) * r;

    console.log(`{${x}f, ${y}f, ${z}f},`);
}
console.log(`len: ${json["coordinates"].length/3}`);

for (let i = 0; i < json["indices"].length; i+=3) {
    const x = json["indices"][i];
    const y = json["indices"][i+1];
    const z = json["indices"][i+2];
    console.log(`{${x}, ${y}, ${z}},`);
}
console.log(`len: ${json["indices"].length/3}`);
