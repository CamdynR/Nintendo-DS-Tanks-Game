const fs = require('fs');
const path = require('path');
const { PNG } = require('pngjs');

const inputDir = path.join(__dirname, '../backgrounds');
const outputDir = path.join(__dirname, '../source/stages');

fs.readdir(inputDir, (err, files) => {
  if (err) {
    console.error('Error reading input directory:', err);
    process.exit(1);
  }

  files.filter(file => file.match(/^stage-.*_barriers\.png$/)).forEach(file => {
    const inputPath = path.join(inputDir, file);
    const outputFileName = file.replace('_barriers.png', '.h');
    const outputPath = path.join(outputDir, outputFileName);

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

            // Black is not a barrier for anything
            if (r === 0 && g === 0 && b === 0 && a === 255) {
              row.push(0);
            // White is barrier for tanks, bullets, and mines
            } else if (r === 255 && g === 255 && b === 255 && a === 255) {
              row.push(1);
              // Blue is a barrier for tanks only
            } else if (r === 0 && g === 0 && b === 255 && a === 255) {
              row.push(2);
              // Green is a barrier for tanks and bullets, can be destroyed by mines
            } else if (r === 0 && g === 255 && b === 0 && a === 255) {
              row.push(3);
            } else {
              console.error('Error: Image contains non-pure black/white pixels.');
              process.exit(1);
            }
          }
          outputArray.push(row);
        }

        const variableName = path.basename(outputFileName, '.h').replace(/-/g, '_').toUpperCase();
        let cppArray = `#ifndef ${variableName}_H
#define ${variableName}_H

#include "../Stage.h"
#include "../Tank.h"
#include "nds/arm9/video.h"

const int ${variableName}_WIDTH = 256;
const int ${variableName}_HEIGHT = 192;

const int ${variableName}_CELL_SIZE = 16;
std::vector<Tank *> *CREATE_${variableName}_TANKS(Stage *stage);

const int ${variableName}_BARRIERS[SCREEN_HEIGHT][SCREEN_WIDTH] = {\n`;
        cppArray += outputArray
          .map((row) => `    { ${row.join(', ')} }`)
          .join(',\n');
        cppArray += '\n};\n\n#endif // IMAGE_DATA_H\n';

        fs.writeFileSync(outputPath, cppArray);
        console.log(`Conversion complete. Output saved to ${outputPath}`);
      });
  });
});
