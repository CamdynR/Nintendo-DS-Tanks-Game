const fs = require('fs');
const { PNG } = require('pngjs');

if (process.argv.length < 4) {
  console.error('Usage: node png_to_array.js <input.png> <output.h>');
  process.exit(1);
}

const inputPath = process.argv[2];
const outputPath = process.argv[3];

fs.createReadStream(inputPath)
  .pipe(new PNG({ filterType: 4 }))
  .on('parsed', function () {
    let width = this.width;
    let height = this.height;
    let outputArray = [];

    for (let y = 0; y < height; y++) {
      let row = [];
      for (let x = 0; x < width; x++) {
        let idx = (width * y + x) * 4;
        let r = this.data[idx];
        let g = this.data[idx + 1];
        let b = this.data[idx + 2];
        let a = this.data[idx + 3];

        if (r === 255 && g === 255 && b === 255 && a === 255) {
          row.push(1);
        } else if (r === 0 && g === 0 && b === 0 && a === 255) {
          row.push(0);
        } else {
          console.error('Error: Image contains non-pure black/white pixels.');
          process.exit(1);
        }
      }
      outputArray.push(row);
    }

    fs.writeFileSync(outputPath, JSON.stringify(outputArray));
    console.log(`Conversion complete. Output saved to ${outputPath}`);
  });
